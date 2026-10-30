#include "stdafx.h"
#include "file_owner.h"

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <memory>
#include <sys/stat.h>

#include "pak_loader.h"

CPakLoader g_PakLoader;

namespace
{
class CNormalFile final : public IFile
{
public:
    BOOL Init(const char* name, const char* mode)
    {
        if (m_file || name == nullptr || mode == nullptr)
        {
            LOG_CHECK_ERROR(!m_file && name != nullptr && mode != nullptr);
            return FALSE;
        }
        m_file.reset(std::fopen(name, mode));
        return m_file != nullptr;
    }

    BOOL Close() override
    {
        delete this;
        return TRUE;
    }

    UINT32 Read(void* data, UINT32 length) override
    {
        if (data == nullptr || length == 0)
            return 0;
        return static_cast<UINT32>(std::fread(data, 1, length, m_file.get()));
    }

    virtual UINT32 Write(const void* data, UINT32 length)
    {
        if (data == nullptr || length == 0)
            return 0;
        return static_cast<UINT32>(std::fwrite(data, 1, length, m_file.get()));
    }

    BOOL Seek(INT32 offset, INT32 origin) override
    {
        return std::fseek(m_file.get(), offset, origin) == 0;
    }

    BOOL Eof() override
    {
        return !m_file || std::feof(m_file.get());
    }

    UINT32 Size() override
    {
        if (!m_file)
        {
            LOG_CHECK_ERROR(m_file);
            return 0;
        }
        const auto current = std::ftell(m_file.get());
        if (current < 0 || std::fseek(m_file.get(), 0, SEEK_END) != 0)
        {
            ERR("Cannot query file size (errno=%d)", errno);
            return 0;
        }
        const auto size = std::ftell(m_file.get());
        if (std::fseek(m_file.get(), current, SEEK_SET) != 0 || size < 0)
        {
            ERR("Cannot restore file position or determine file size (errno=%d)", errno);
            return 0;
        }
        return static_cast<UINT32>(size);
    }

private:
    cqyh::CFilePtr m_file;
};

class CPakFileDummy final : public IFile
{
public:
    BOOL Init(INT32 index)
    {
        m_size = g_PakLoader.GetFileSize(index);
        m_data = std::make_unique_for_overwrite<char[]>(m_size);
        if (!g_PakLoader.ReadFileData(index, m_data.get(), m_size))
        {
            ERR("Cannot read packed file at index %d (%u bytes)", index, m_size);
            return FALSE;
        }
        m_index = index;
        m_offset = 0;
        return TRUE;
    }

    BOOL Close() override
    {
        delete this;
        return TRUE;
    }

    UINT32 Read(void* data, UINT32 length) override
    {
        if (data == nullptr || length == 0)
            return 0;
        // Subtraction avoids overflow when callers request UINT32_MAX bytes.
        const auto size = (std::min)(length, m_size - m_offset);
        if (size != 0)
            std::memcpy(data, m_data.get() + m_offset, size);
        m_offset += size;
        return size;
    }

    BOOL Seek(INT32 offset, INT32 origin) override
    {
        std::int64_t position = offset;
        if (origin == SEEK_CUR)
            position += m_offset;
        else if (origin == SEEK_END)
            // Preserve the pak reader's legacy convention: END,0 is the last byte.
            position = static_cast<std::int64_t>(m_size) - offset - 1;
        else if (origin != SEEK_SET)
            return FALSE;
        if (position < 0 || position > m_size)
            return FALSE;
        m_offset = static_cast<UINT32>(position);
        return TRUE;
    }

    BOOL Eof() override { return m_offset == m_size && m_index != INVALID_PAK_FILE_INDEX; }
    UINT32 Size() override { return m_size; }

private:
    INT32 m_index = INVALID_PAK_FILE_INDEX;
    std::unique_ptr<char[]> m_data;
    UINT32 m_size = 0;
    UINT32 m_offset = 0;
};

UINT32 FileTime(const char* name, bool creation)
{
    if (name == nullptr)
    {
        LOG_CHECK_ERROR(name != nullptr);
        return 0;
    }
    cqyh::CFilePtr file(std::fopen(name, "rb"));
    if (file)
    {
        const int descriptor = fileno(file.get());
        struct stat info {};
        if (descriptor == -1 || fstat(descriptor, &info) != 0)
        {
            ERR("Cannot read file timestamps for %s (errno=%d)", name, errno);
            return 0;
        }
        return static_cast<UINT32>(creation ? info.st_ctime : info.st_mtime);
    }
    if (g_PakLoader.GetFileCount() > 0)
    {
        const auto index = g_PakLoader.FindFile(name);
        if (index != INVALID_PAK_FILE_INDEX)
            return creation ? g_PakLoader.GetFileCreateTime(index) : g_PakLoader.GetFileModifyTime(index);
    }
    return 0;
}
}

BOOL g_OpenPakFolder(const char* name) { return g_PakLoader.Init(name); }
BOOL g_ClosePakFolder() { return g_PakLoader.UnInit(); }
BOOL g_ReloadPakFolder(const char* name) { return g_PakLoader.Reload(name); }

IFile* g_OpenFile(const char* name, const char* mode)
{
    if (name == nullptr || mode == nullptr)
    {
        LOG_CHECK_ERROR(name != nullptr && mode != nullptr);
        return nullptr;
    }

    // Keep concrete ownership until initialization succeeds. IFile has no virtual destructor.
    auto normal = std::make_unique<CNormalFile>();
    if (normal->Init(name, mode))
        return normal.release();

    if (g_PakLoader.GetFileCount() > 0)
    {
        const auto index = g_PakLoader.FindFile(name);
        if (index != INVALID_PAK_FILE_INDEX)
        {
            auto packed = std::make_unique<CPakFileDummy>();
            if (packed->Init(index))
                return packed.release();
            ERR("Failed to open packed file %s", name);
        }
    }
    return nullptr;
}

BOOL _FileExists(const char* name)
{
    if (name == nullptr)
        return FALSE;
    return cqyh::CFilePtr(std::fopen(name, "r")) != nullptr;
}

BOOL g_IsFileExist(const char* name)
{
    if (name == nullptr)
        return FALSE;
    return _FileExists(name) || (g_PakLoader.GetFileCount() > 0 &&
                                g_PakLoader.FindFile(name) != INVALID_PAK_FILE_INDEX);
}

UINT32 g_GetFileCreateTime(const char* name) { return FileTime(name, true); }
UINT32 g_GetFileModifyTime(const char* name) { return FileTime(name, false); }
