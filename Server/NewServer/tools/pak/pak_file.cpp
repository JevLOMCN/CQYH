#include "stdafx.h"
#include "pak_file.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <format>
#include <limits>
#include <span>
#include <stdexcept>
#include <sys/stat.h>
#include <type_traits>

#include "hash.h"
#include "md5.h"
#include "zlib.h"

namespace
{
namespace fs = std::filesystem;
constexpr std::size_t max_name = 128;
constexpr std::size_t max_files = 128 * 1024;
constexpr std::size_t max_chunks = 8 * max_files;
constexpr std::size_t max_metadata = max_files * sizeof(PAK_FILE_INFO) + max_chunks * sizeof(PAK_CHUNK_INFO);
static_assert(sizeof(PAK_FILE_HEADER) == 52 && sizeof(PAK_FILE_INFO) == 80 && sizeof(PAK_CHUNK_INFO) == 8);
static_assert(std::is_trivially_copyable_v<PAK_FILE_HEADER> && std::is_trivially_copyable_v<PAK_FILE_INFO>);

void check(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}

template<class F>
BOOL attempt(F&& operation)
{
    try
    {
        operation();
        return TRUE;
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "pak: %s\n", error.what());
        return FALSE;
    }
}

UINT32 narrow(std::uint64_t value)
{
    check(value <= std::numeric_limits<UINT32>::max(), "Archive exceeds the 32-bit PAK format");
    return static_cast<UINT32>(value);
}

UINT32 position(FILE* file)
{
    const auto offset = _ftelli64(file);
    check(offset >= 0, "Cannot get archive position");
    return narrow(offset);
}

void seek(FILE* file, std::uint64_t offset)
{
    check(_fseeki64(file, narrow(offset), SEEK_SET) == 0, "Cannot seek in archive");
}

UINT32 file_size(FILE* file)
{
    check(_fseeki64(file, 0, SEEK_END) == 0, "Cannot seek to archive end");
    return position(file);
}

void read(FILE* file, void* data, std::size_t size)
{
    check(size == 0 || std::fread(data, 1, size, file) == size, "Truncated archive data");
}

void write(FILE* file, const void* data, std::size_t size)
{
    check(size == 0 || std::fwrite(data, 1, size, file) == size, "Cannot write archive data");
}

std::vector<char> read_region(FILE* file, UINT32 offset, UINT32 size, UINT32 total)
{
    check(offset <= total && size <= total - offset, "Archive section is outside the file");
    std::vector<char> data(size);
    seek(file, offset);
    read(file, data.data(), data.size());
    return data;
}

std::vector<char> compressed(std::span<const char> source)
{
    uLongf size = compressBound(narrow(source.size()));
    std::vector<char> result(size);
    check(compress(reinterpret_cast<Bytef*>(result.data()), &size,
        reinterpret_cast<const Bytef*>(source.data()), narrow(source.size())) == Z_OK, "Compression failed");
    result.resize(size);
    return result;
}

std::vector<char> decompressed(std::span<const char> source, std::size_t limit)
{
    // Metadata compression ratios are not limited to the old fixed factor of four.
    std::vector<char> result(std::min<std::size_t>(1024, limit));
    for (;;)
    {
        uLongf size = narrow(result.size());
        const int status = uncompress(reinterpret_cast<Bytef*>(result.data()), &size,
            reinterpret_cast<const Bytef*>(source.data()), narrow(source.size()));
        if (status == Z_OK)
        {
            result.resize(size);
            return result;
        }
        check(status == Z_BUF_ERROR && result.size() < limit, "Invalid or oversized compressed metadata");
        result.resize(std::min(limit, std::max<std::size_t>(1, result.size() * 2)));
    }
}

template<class T>
T consume(std::span<const char>& bytes)
{
    check(bytes.size() >= sizeof(T), "Truncated file or chunk record");
    T record;
    std::memcpy(&record, bytes.data(), sizeof(T));
    bytes = bytes.subspan(sizeof(T));
    return record;
}

template<class T>
void append(std::vector<char>& bytes, const T& record)
{
    const auto data = std::as_bytes(std::span(&record, 1));
    const auto begin = reinterpret_cast<const char*>(data.data());
    bytes.insert(bytes.end(), begin, begin + data.size());
}

void digest(UINT8 (&result)[16], std::span<const char> bytes)
{
    calc_md5(result, reinterpret_cast<UINT8*>(const_cast<char*>(bytes.data())), narrow(bytes.size()));
}

struct Md5Deleter
{
    void operator()(md5_ctx* context) const noexcept
    {
        UINT8 ignored[16];
        md5_final(ignored, context);
    }
};
}

CPakFile::~CPakFile()
{
    Close();
}

void CPakFile::Reset() noexcept
{
    m_file.reset();
    m_header = {};
    m_entries.clear();
    m_hash_table.clear();
    m_fallback_name.clear();
    m_open = m_dirty = m_hash_dirty = m_write_failed = false;
}

CPakFile::Entry& CPakFile::At(int index)
{
    check(m_open && index >= 0 && static_cast<std::size_t>(index) < m_entries.size(), "Invalid PAK file index");
    return m_entries[index];
}

void CPakFile::UpdateHashTable()
{
    if (!m_hash_dirty)
        return;
    m_hash_table.assign(m_entries.size(), INVALID_FILE_POS);
    for (std::size_t i = 0; i < m_entries.size(); ++i)
    {
        auto& info = m_entries[i].info;
        auto slot = info.dwNameHashA % m_entries.size();
        while (m_hash_table[slot] != INVALID_FILE_POS)
            slot = (slot + 1) % m_entries.size();
        m_hash_table[slot] = static_cast<UINT32>(i);
        info.dwHashTableIndex = static_cast<UINT32>(slot);
    }
    m_hash_dirty = false;
}

BOOL CPakFile::Create(const char* file_name)
{
    if (m_open)
        return FALSE;
    Reset();
    const auto result = attempt([&]
    {
        check(file_name && *file_name, "Missing archive filename");
        m_file.reset(std::fopen(file_name, "wb+"));
        check(bool(m_file), "Cannot create archive");
        m_header.dwTag = PAK_FILE_TAG;
        m_header.dwVersion = PAK_FILE_VERSION;
        m_header.dwDataOffset = sizeof(m_header);
        write(m_file.get(), &m_header, sizeof(m_header));
        m_open = m_dirty = m_hash_dirty = true;
    });
    if (!result)
        Reset();
    return result;
}

BOOL CPakFile::Open(const char* file_name)
{
    if (m_open)
        return FALSE;
    Reset();
    const auto result = attempt([&]
    {
        check(file_name && *file_name, "Missing archive filename");
        m_file.reset(std::fopen(file_name, "rb+"));
        check(bool(m_file), "Cannot open archive");
        const auto total = file_size(m_file.get());
        seek(m_file.get(), 0);
        read(m_file.get(), &m_header, sizeof(m_header));
        check(m_header.dwTag == PAK_FILE_TAG && m_header.dwVersion == PAK_FILE_VERSION, "Unsupported PAK header");
        check(m_header.nFileCount >= 0 && m_header.nFileCount <= max_files, "Invalid archive file count");
        check(m_header.dwDataOffset >= sizeof(m_header) && m_header.dwDataOffset <= total, "Invalid data offset");
        check(m_header.dwFileNameSize <= max_name * max_files, "Oversized archive name buffer");
        check(m_header.dwFileInfoSize <= compressBound(max_metadata), "Oversized archive metadata");

        std::vector<char> names;
        if (!(m_header.dwPackageFlag & PackageFlag_NoNameBuffer))
        {
            check(m_header.dwNameBufferSize <= compressBound(max_name * max_files), "Oversized compressed names");
            names = read_region(m_file.get(), m_header.dwNameBufferOffset, m_header.dwNameBufferSize, total);
            if (m_header.dwPackageFlag & PackageFlag_NameBufferCompressed)
                names = decompressed(names, max_name * max_files);
            check(names.size() == m_header.dwFileNameSize, "Archive name buffer size mismatch");
        }
        auto metadata = read_region(m_file.get(), m_header.dwFileInfoOffset, m_header.dwFileInfoSize, total);
        if ((m_header.dwPackageFlag & PackageFlag_FileInfoCompressed) && !metadata.empty())
            metadata = decompressed(metadata, max_metadata);
        std::span<const char> cursor(metadata);
        std::size_t chunk_count = 0;
        m_entries.reserve(m_header.nFileCount);
        for (int index = 0; index < m_header.nFileCount; ++index)
        {
            Entry entry;
            entry.info = consume<PAK_FILE_INFO>(cursor);
            const auto& info = entry.info;
            check(info.nDataChunkCount >= 0 && info.nDataChunkCount <= max_chunks - chunk_count, "Invalid archive chunk count");
            chunk_count += info.nDataChunkCount;
            if (!(m_header.dwPackageFlag & PackageFlag_NoNameBuffer))
            {
                check(info.dwFileNameOffset < names.size(), "Invalid archive name offset");
                const std::string_view tail(names.data() + info.dwFileNameOffset, names.size() - info.dwFileNameOffset);
                const auto end = tail.find('\0');
                check(end > 0 && end < max_name, "Invalid or unterminated archive name");
                entry.name = tail.substr(0, end);
            }
            entry.chunks.reserve(info.nDataChunkCount);
            std::uint64_t offset = std::uint64_t(m_header.dwDataOffset) + info.dwChunkStartOffset;
            const auto stored_size = info.dwFileFlag & FileFlag_NoCompressed ? info.dwOriginalSize : info.dwCompressedSize;
            check(offset <= total && stored_size <= total - offset, "Invalid archive data range");
            std::uint64_t chunk_bytes = 0;
            for (int i = 0; i < info.nDataChunkCount; ++i)
            {
                const auto chunk = consume<PAK_CHUNK_INFO>(cursor);
                check(std::uint64_t(m_header.dwDataOffset) + chunk.dwChunkOffset == offset, "Invalid archive chunk offset");
                offset += chunk.dwChunkSize;
                check(offset <= total, "Truncated archive chunk");
                chunk_bytes += chunk.dwChunkSize;
                entry.chunks.push_back(chunk);
            }
            check(chunk_bytes == stored_size, "Archive chunks do not match the stored file size");
            m_entries.push_back(std::move(entry));
        }
        check(cursor.empty(), "Unexpected trailing metadata records");
        // Rebuild with the existing filename hash algorithm; never trust file-provided indices.
        m_hash_dirty = true;
        UpdateHashTable();
        m_open = true;
    });
    if (!result)
        Reset();
    return result;
}

BOOL CPakFile::Close()
{
    auto result = attempt([&]
    {
        check(!m_write_failed, "An earlier archive write failed");
        if (!m_open || !m_dirty)
            return;
        UpdateHashTable();
        std::vector<char> names;
        std::vector<char> metadata;
        for (auto& entry : m_entries)
        {
            entry.info.dwFileNameOffset = narrow(names.size());
            names.insert(names.end(), entry.name.begin(), entry.name.end());
            names.push_back('\0');
            append(metadata, entry.info);
            for (const auto& chunk : entry.chunks)
                append(metadata, chunk);
        }
        check(metadata.size() <= max_metadata, "Too many archive records");
        if ((m_header.dwPackageFlag & PackageFlag_FileInfoCompressed) && !metadata.empty())
            metadata = compressed(metadata);
        if (m_header.dwPackageFlag & PackageFlag_NoNameBuffer)
            names.clear();
        m_header.dwFileNameSize = narrow(names.size());
        if (m_header.dwPackageFlag & PackageFlag_NameBufferCompressed)
            names = compressed(names);
        const auto total = file_size(m_file.get());
        const std::uint64_t old_size = std::uint64_t(m_header.dwHashTableSize) + m_header.dwNameBufferSize + m_header.dwFileInfoSize;
        auto end = total;
        for (const auto start : {m_header.dwHashTableOffset, m_header.dwNameBufferOffset, m_header.dwFileInfoOffset})
        {
            if (start != 0 && std::uint64_t(start) + old_size == total)
            {
                end = start;
                break;
            }
        }
        seek(m_file.get(), end);
        m_header.dwHashTableOffset = m_header.dwHashTableSize = 0;
        if (!(m_header.dwPackageFlag & PackageFlag_NoHashTable))
        {
            m_header.dwHashTableOffset = position(m_file.get());
            m_header.dwHashTableSize = narrow(m_hash_table.size() * sizeof(UINT32));
            write(m_file.get(), m_hash_table.data(), m_header.dwHashTableSize);
        }
        m_header.dwNameBufferOffset = m_header.dwNameBufferSize = 0;
        if (!(m_header.dwPackageFlag & PackageFlag_NoNameBuffer))
        {
            m_header.dwNameBufferOffset = position(m_file.get());
            m_header.dwNameBufferSize = narrow(names.size());
            write(m_file.get(), names.data(), names.size());
        }
        m_header.dwFileInfoOffset = position(m_file.get());
        m_header.dwFileInfoSize = narrow(metadata.size());
        write(m_file.get(), metadata.data(), metadata.size());
        narrow(position(m_file.get()));
        seek(m_file.get(), 0);
        write(m_file.get(), &m_header, sizeof(m_header));
    });
    if (m_file && std::fclose(m_file.release()) != 0)
        result = FALSE;
    Reset();
    return result;
}

BOOL CPakFile::IsOpened()
{
    return m_open;
}

BOOL CPakFile::SetPackageFlag(BOOL hash, BOOL names, BOOL compress_names, BOOL compress_info)
{
    if (!m_open)
        return FALSE;
    m_header.dwPackageFlag = (hash ? 0 : PackageFlag_NoHashTable) |
        (names ? 0 : PackageFlag_NoNameBuffer) | (compress_names ? PackageFlag_NameBufferCompressed : 0) |
        (compress_info ? PackageFlag_FileInfoCompressed : 0);
    m_dirty = true;
    return TRUE;
}

BOOL CPakFile::AddFile(const char* package_name, const char* disk_name)
{
    return attempt([&]
    {
        check(disk_name, "Missing input filename");
        cqyh::CFilePtr file(std::fopen(disk_name, "rb"));
        check(bool(file), "Cannot open input file");
        const auto size = file_size(file.get());
        seek(file.get(), 0);
        std::vector<char> bytes(size);
        read(file.get(), bytes.data(), bytes.size());
        struct _stat64 info{};
        check(_fstat64(_fileno(file.get()), &info) == 0, "Cannot read file timestamps");
        check(AddFileFromData(package_name, info.st_ctime, info.st_mtime,
            bytes.empty() ? "" : bytes.data(), bytes.size()), "Cannot add input file");
    });
}

BOOL CPakFile::AddFileFromData(const char* package_name, time_t created, time_t modified, const char* data, size_t size)
{
    return attempt([&]
    {
        check(m_open && package_name && *package_name && (data || size == 0), "Invalid archive input");
        check(std::strlen(package_name) < max_name && m_entries.size() < max_files, "Archive name or file limit exceeded");
        check(FindFile(package_name) == INVALID_PAK_FILE_INDEX, "Duplicate archive filename");
        Entry entry;
        entry.name = package_name;
        auto& info = entry.info;
        info.dwCreateTime = static_cast<UINT32>(created);
        info.dwModifyTime = static_cast<UINT32>(modified);
        info.dwOriginalSize = narrow(size);
        info.dwChunkSize = narrow(size < ONE_CHUNK_SIZE ? size : DEFAULT_CHUNK_SIZE);
        hash_file_name(package_name, &info.dwNameHashA, &info.dwNameHashB);
        digest(info.md5OriginDataHash, {data, size});
        const auto chunk_count = size == 0 ? 0 : 1 + (size - 1) / info.dwChunkSize;
        std::size_t existing_chunks = 0;
        for (const auto& existing : m_entries)
            existing_chunks += existing.chunks.size();
        check(chunk_count <= max_chunks - existing_chunks, "Too many archive chunks");
        entry.chunks.reserve(chunk_count);
        if (m_entries.size() == m_entries.capacity())
            m_entries.reserve(std::min(max_files, std::max<std::size_t>(16, m_entries.size() * 2)));
        std::unique_ptr<md5_ctx, Md5Deleter> context(md5_init());
        check(bool(context), "Cannot create MD5 context");
        const auto end = file_size(m_file.get());
        check(end >= m_header.dwDataOffset, "Invalid archive data offset");
        info.dwChunkStartOffset = end - m_header.dwDataOffset;
        std::uint64_t stored_size = 0;
        // Keep compression scratch storage bounded by one chunk, even for large input files.
        for (std::size_t offset = 0; offset < size;)
        {
            const auto count = std::min<std::size_t>(info.dwChunkSize, size - offset);
            auto chunk = compressed({data + offset, count});
            const auto chunk_offset = narrow(std::uint64_t(info.dwChunkStartOffset) + stored_size);
            stored_size += chunk.size();
            narrow(std::uint64_t(end) + stored_size);
            narrow(std::uint64_t(m_header.dwDataSize) + stored_size);
            md5_update(context.get(), reinterpret_cast<UINT8*>(chunk.data()), narrow(chunk.size()));
            m_write_failed = true;
            write(m_file.get(), chunk.data(), chunk.size());
            entry.chunks.push_back({chunk_offset, narrow(chunk.size())});
            offset += count;
        }
        md5_final(info.md5CompressedDataHash, context.release());
        info.dwCompressedSize = narrow(stored_size);
        info.nDataChunkCount = static_cast<INT32>(entry.chunks.size());
        m_header.dwDataSize = narrow(std::uint64_t(m_header.dwDataSize) + stored_size);
        m_entries.push_back(std::move(entry));
        m_header.nFileCount = static_cast<INT32>(m_entries.size());
        m_dirty = m_hash_dirty = true;
        m_write_failed = false;
    });
}

BOOL CPakFile::AddRaw(const char* package_name, const Entry& source, const std::vector<char>& bytes)
{
    return attempt([&]
    {
        check(m_open && package_name && *package_name && std::strlen(package_name) < max_name, "Invalid destination name");
        auto index = FindFile(package_name);
        if (index != INVALID_PAK_FILE_INDEX && !(m_entries[index].info.dwFileFlag & FileFlag_Deleted) &&
            m_entries[index].info.dwOriginalSize == source.info.dwOriginalSize &&
            std::ranges::equal(m_entries[index].info.md5OriginDataHash, source.info.md5OriginDataHash))
            return;
        check(index != INVALID_PAK_FILE_INDEX || m_entries.size() < max_files, "Too many archive files");
        Entry entry = source;
        entry.name = package_name;
        if (index == INVALID_PAK_FILE_INDEX && m_entries.size() == m_entries.capacity())
            m_entries.reserve(std::min(max_files, std::max<std::size_t>(16, m_entries.size() * 2)));
        std::size_t chunk_count = entry.chunks.size();
        for (std::size_t i = 0; i < m_entries.size(); ++i)
        {
            if (index == INVALID_PAK_FILE_INDEX || i != static_cast<std::size_t>(index))
                chunk_count += m_entries[i].chunks.size();
        }
        check(chunk_count <= max_chunks, "Too many archive chunks");
        const auto end = file_size(m_file.get());
        check(end >= m_header.dwDataOffset, "Invalid archive data offset");
        narrow(std::uint64_t(end) + bytes.size());
        entry.info.dwChunkStartOffset = end - m_header.dwDataOffset;
        auto offset = entry.info.dwChunkStartOffset;
        for (auto& chunk : entry.chunks)
        {
            chunk.dwChunkOffset = offset;
            offset = narrow(std::uint64_t(offset) + chunk.dwChunkSize);
        }
        const auto data_size = narrow(std::uint64_t(m_header.dwDataSize) + bytes.size());
        m_write_failed = true;
        write(m_file.get(), bytes.data(), bytes.size());
        m_write_failed = false;
        if (index == INVALID_PAK_FILE_INDEX)
            m_entries.push_back(std::move(entry));
        else
            m_entries[index] = std::move(entry);
        m_header.dwDataSize = data_size;
        m_header.nFileCount = static_cast<INT32>(m_entries.size());
        m_dirty = m_hash_dirty = true;
    });
}

BOOL CPakFile::AddFileFromPak(const char* package_name, CPakFile* source, int index)
{
    return attempt([&]
    {
        check(source && source != this, "Invalid source archive");
        const auto& entry = source->At(index);
        check(!(entry.info.dwFileFlag & FileFlag_Deleted), "Cannot copy a deleted entry");
        const auto size = entry.info.dwFileFlag & FileFlag_NoCompressed ? entry.info.dwOriginalSize : entry.info.dwCompressedSize;
        auto bytes = read_region(source->m_file.get(), narrow(std::uint64_t(source->m_header.dwDataOffset) +
            entry.info.dwChunkStartOffset), size, file_size(source->m_file.get()));
        check(AddRaw(package_name, entry, bytes), "Cannot copy archive entry");
    });
}

int CPakFile::FindFile(const char* package_name)
{
    if (!m_open || !package_name || m_entries.empty())
        return INVALID_PAK_FILE_INDEX;
    int result = INVALID_PAK_FILE_INDEX;
    attempt([&]
    {
        UpdateHashTable();
        UINT32 hash_a, hash_b;
        hash_file_name(package_name, &hash_a, &hash_b);
        auto slot = hash_a % m_entries.size();
        for (std::size_t tried = 0; tried < m_entries.size(); ++tried)
        {
            const auto index = m_hash_table[slot];
            if (m_entries[index].info.dwNameHashB == hash_b)
            {
                result = static_cast<int>(index);
                break;
            }
            slot = (slot + 1) % m_entries.size();
        }
    });
    return result;
}

BOOL CPakFile::DelFile(const char* package_name)
{
    const int index = FindFile(package_name);
    if (index == INVALID_PAK_FILE_INDEX)
        return FALSE;
    m_entries[index].info.dwFileFlag |= FileFlag_Deleted;
    m_dirty = true;
    return TRUE;
}

int CPakFile::GetFileCount()
{
    return m_open ? static_cast<int>(m_entries.size()) : -1;
}

size_t CPakFile::GetFileSize(int index)
{
    size_t result = 0;
    attempt([&] { result = At(index).info.dwOriginalSize; });
    return result;
}

const char* CPakFile::GetFileName(int index)
{
    const char* result = nullptr;
    attempt([&]
    {
        const auto& entry = At(index);
        if (m_header.dwPackageFlag & PackageFlag_NoNameBuffer)
        {
            m_fallback_name = std::format("{:08X}_{:08X}.dat", entry.info.dwNameHashA, entry.info.dwNameHashB);
            result = m_fallback_name.c_str();
        }
        else
            result = entry.name.c_str();
    });
    return result;
}

BOOL CPakFile::GetFileHashValue(int index, unsigned long& hash_a, unsigned long& hash_b)
{
    return attempt([&] { const auto& info = At(index).info; hash_a = info.dwNameHashA; hash_b = info.dwNameHashB; });
}

BOOL CPakFile::IsDeletedFile(int index)
{
    bool deleted = false;
    attempt([&] { deleted = (At(index).info.dwFileFlag & FileFlag_Deleted) != 0; });
    return deleted;
}

BOOL CPakFile::GetFileMD5Value(int index, unsigned char result[16])
{
    return attempt([&]
    {
        check(result, "Missing MD5 output buffer");
        std::ranges::copy(At(index).info.md5OriginDataHash, result);
    });
}

BOOL CPakFile::ReadFileData(int index, char* data, size_t size)
{
    return attempt([&]
    {
        const auto& entry = At(index);
        const auto& info = entry.info;
        check((data || size == 0) && size <= info.dwOriginalSize, "Invalid output buffer size");
        seek(m_file.get(), std::uint64_t(m_header.dwDataOffset) + info.dwChunkStartOffset);
        if (info.dwFileFlag & FileFlag_NoCompressed)
        {
            read(m_file.get(), data, size);
            return;
        }
        check(size == info.dwOriginalSize, "Compressed files require a complete output buffer");
        std::size_t written = 0;
        for (const auto& chunk : entry.chunks)
        {
            std::vector<char> bytes(chunk.dwChunkSize);
            read(m_file.get(), bytes.data(), bytes.size());
            uLongf count = narrow(size - written);
            check(uncompress(reinterpret_cast<Bytef*>(data + written), &count,
                reinterpret_cast<const Bytef*>(bytes.data()), narrow(bytes.size())) == Z_OK, "Invalid compressed file data");
            written += count;
        }
        check(written == size, "Decompressed file size mismatch");
    });
}

BOOL CPakFile::VerifyFile(int index)
{
    return attempt([&]
    {
        const auto& entry = At(index);
        std::vector<char> bytes(entry.info.dwOriginalSize);
        check(ReadFileData(index, bytes.data(), bytes.size()), "Cannot read file for verification");
        UINT8 actual[16];
        digest(actual, bytes);
        check(std::ranges::equal(actual, entry.info.md5OriginDataHash), "File MD5 mismatch");
    });
}

BOOL CPakFile::ExtractFile(int index)
{
    return attempt([&]
    {
        const auto& entry = At(index);
        const auto* stored_name = GetFileName(index);
        check(stored_name, "Cannot get extraction filename");
        const fs::path name(stored_name);
        check(!name.empty() && !name.has_root_path() && name.string().find(':') == std::string::npos, "Invalid extraction path");
        for (const auto& component : name)
            check(component != "..", "Extraction path escapes its destination");
        const auto root = fs::current_path();
        const auto destination = fs::weakly_canonical(root / name);
        const auto relative = destination.lexically_relative(root);
        check(!relative.empty() && *relative.begin() != "..", "Extraction path escapes its destination");
        std::vector<char> bytes(entry.info.dwOriginalSize);
        check(ReadFileData(index, bytes.data(), bytes.size()), "Cannot read file for extraction");
        fs::create_directories(destination.parent_path());
        cqyh::CFilePtr file(_wfopen(destination.c_str(), L"wb"));
        check(bool(file), "Cannot create extracted file");
        write(file.get(), bytes.data(), bytes.size());
        check(std::fclose(file.release()) == 0, "Cannot close extracted file");
    });
}
