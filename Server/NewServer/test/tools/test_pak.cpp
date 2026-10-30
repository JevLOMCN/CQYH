#include "stdafx.h"
#include "pak_file.h"
#include "hash.h"
#include "md5.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
void require(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}

void exercise(const char* name, bool hash, bool names, bool compress_names, bool compress_info)
{
    const std::string greeting = "Archive compatibility fixture\r\n";
    std::vector<char> chunks(180000);
    unsigned state = 0x12345678;
    for (auto& byte : chunks)
    {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        byte = static_cast<char>(state);
    }
    CPakFile archive;
    require(archive.Create(name), "create archive");
    require(archive.SetPackageFlag(hash, names, compress_names, compress_info), "set flags");
    require(archive.AddFileFromData("sample\\hello.txt", 1234567890, 1234567891,
        greeting.data(), greeting.size()), "add greeting");
    require(archive.AddFileFromData("sample\\chunks.bin", 1234567892, 1234567893,
        chunks.data(), chunks.size()), "add multichunk file");
    require(archive.AddFileFromData("sample\\empty.dat", 1234567894, 1234567895, "", 0), "add empty file");
    require(archive.Close(), "close archive");
    require(archive.Open(name), "open archive");
    require(archive.GetFileCount() == 3, "file count");
    require(archive.FindFile("sample\\hello.txt") == 0, "find by legacy hash");
    require(archive.FindFile("sample\\absent.txt") == INVALID_PAK_FILE_INDEX, "missing name");
    std::vector<char> data(chunks.size());
    require(archive.ReadFileData(1, data.data(), data.size()), "read multichunk data");
    require(data == chunks, "multichunk contents");
    require(archive.VerifyFile(0) && archive.VerifyFile(1) && archive.VerifyFile(2), "MD5 verification");
    require(archive.Close(), "close reopened archive");
}

std::vector<char> read_bytes(const char* name)
{
    std::ifstream file(name, std::ios::binary);
    require(bool(file), "open fixture");
    return {std::istreambuf_iterator<char>(file), {}};
}

void write_bytes(const char* name, const std::vector<char>& bytes)
{
    std::ofstream file(name, std::ios::binary);
    file.write(bytes.data(), bytes.size());
    file.close();
    require(bool(file), "write malformed fixture");
}

void malformed_inputs()
{
    const auto original = read_bytes("plain.pak");
    PAK_FILE_HEADER header;
    std::memcpy(&header, original.data(), sizeof(header));
    auto reject = [&](const std::vector<char>& bytes)
    {
        write_bytes("malformed.pak", bytes);
        CPakFile archive;
        require(!archive.Open("malformed.pak") && !archive.IsOpened(), "reject malformed archive");
        require(archive.Open("plain.pak") && archive.Close(), "reuse after rejected open");
    };
    for (const auto length : {std::size_t(0), sizeof(PAK_FILE_HEADER) - 1, original.size() - 1})
        reject({original.begin(), original.begin() + length});
    for (const auto member : {&PAK_FILE_HEADER::dwTag, &PAK_FILE_HEADER::dwVersion,
        &PAK_FILE_HEADER::dwFileNameSize, &PAK_FILE_HEADER::dwFileInfoOffset})
    {
        auto changed = header;
        changed.*member = 0xffffffff;
        auto bytes = original;
        std::memcpy(bytes.data(), &changed, sizeof(changed));
        reject(bytes);
    }
    auto changed = header;
    changed.nFileCount = -1;
    auto bytes = original;
    std::memcpy(bytes.data(), &changed, sizeof(changed));
    reject(bytes);
    PAK_FILE_INFO info;
    std::memcpy(&info, original.data() + header.dwFileInfoOffset, sizeof(info));
    for (int mode = 0; mode < 3; ++mode)
    {
        auto invalid = info;
        if (mode == 0)
            invalid.nDataChunkCount = -1;
        else if (mode == 1)
            invalid.dwFileNameOffset = 0xffffffff;
        else
            invalid.dwChunkStartOffset = 0xffffffff;
        bytes = original;
        std::memcpy(bytes.data() + header.dwFileInfoOffset, &invalid, sizeof(invalid));
        reject(bytes);
    }
    bytes = original;
    std::fill(bytes.begin() + header.dwNameBufferOffset,
        bytes.begin() + header.dwNameBufferOffset + header.dwNameBufferSize, 'x');
    reject(bytes);
    bytes = original;
    bytes[header.dwDataOffset] ^= 0x55;
    write_bytes("damaged.pak", bytes);
    CPakFile archive;
    require(archive.Open("damaged.pak") && !archive.VerifyFile(0), "detect damaged compressed data");
    require(!archive.GetFileName(-1) && !archive.ReadFileData(99, nullptr, 0), "reject invalid indices");
    require(archive.Close(), "close damaged archive without rewriting it");
    require(read_bytes("damaged.pak") == bytes, "verification must not modify input");

    write_bytes("malformed.pak", {});
    DWORD before = 0, after = 0;
    require(GetProcessHandleCount(GetCurrentProcess(), &before), "count initial handles");
    for (int i = 0; i < 100; ++i)
        require(!archive.Open("malformed.pak"), "reject repeated malformed input");
    require(GetProcessHandleCount(GetCurrentProcess(), &after) && before == after, "failed opens must release native file handles");
}

void updates_and_limits()
{
    CPakFile source, destination;
    require(source.Open("named.pak") && destination.Create("updated.pak"), "open source and destination");
    require(!source.Open("plain.pak") && !source.Create("must-not-create.pak"), "prevent replacing an open archive");
    require(destination.AddFileFromPak("sample\\hello.txt", &source, 0), "copy compressed entry");
    require(destination.DelFile("sample\\hello.txt") && destination.IsDeletedFile(0), "mark deletion");
    require(destination.AddFileFromPak("sample\\hello.txt", &source, 0) && !destination.IsDeletedFile(0), "restore equal-content deleted entry");
    require(!destination.AddFileFromData("sample\\hello.txt", 0, 0, "", 0), "reject duplicate name");
    const std::string long_name(128, 'a');
    require(!destination.AddFileFromData(long_name.c_str(), 0, 0, "", 0), "reject overlong archive name");
    require(destination.Close() && destination.Open("updated.pak") && destination.VerifyFile(0), "persist copied entry");
    require(destination.Close() && source.Close(), "close update archives");

    require(destination.Create("empty.pak") && destination.SetPackageFlag(false, true, true, true) && destination.Close(), "create empty archive");
    require(destination.Open("empty.pak") && destination.GetFileCount() == 0 && destination.Close(), "read empty archive");
    require(destination.Create("ratio.pak") && destination.SetPackageFlag(false, true, true, true), "create metadata ratio archive");
    for (int i = 0; i < 200; ++i)
    {
        const auto name = std::string("repeated-empty-file-") + std::to_string(i);
        require(destination.AddFileFromData(name.c_str(), 0, 0, "", 0), "add highly compressible metadata");
    }
    require(destination.Close() && destination.Open("ratio.pak") && destination.GetFileCount() == 200 && destination.Close(), "decode metadata beyond the old four-times buffer");
    require(destination.Create("escaping.pak"), "create extraction path fixture");
    require(destination.AddFileFromData("..\\outside.txt", 0, 0, "", 0), "store legacy relative name");
    require(!destination.ExtractFile(0), "reject escaping extraction path");
    require(destination.Close(), "close extraction fixture");
}

void uncompressed_entry()
{
    // Earlier writers can store raw payload chunks independently of container compression flags.
    const char name[] = "native\\raw.txt";
    const char payload[] = "plain";
    PAK_FILE_HEADER header{};
    header.dwTag = PAK_FILE_TAG;
    header.dwVersion = PAK_FILE_VERSION;
    header.dwPackageFlag = PackageFlag_NoHashTable;
    header.nFileCount = 1;
    header.dwDataOffset = sizeof(header);
    header.dwDataSize = sizeof(payload) - 1;
    header.dwFileNameSize = sizeof(name);
    header.dwNameBufferOffset = sizeof(header) + header.dwDataSize;
    header.dwNameBufferSize = sizeof(name);
    header.dwFileInfoOffset = header.dwNameBufferOffset + sizeof(name);
    header.dwFileInfoSize = sizeof(PAK_FILE_INFO) + sizeof(PAK_CHUNK_INFO);
    PAK_FILE_INFO info{};
    info.dwFileFlag = FileFlag_NoCompressed;
    info.dwOriginalSize = info.dwChunkSize = header.dwDataSize;
    info.nDataChunkCount = 1;
    hash_file_name(name, &info.dwNameHashA, &info.dwNameHashB);
    calc_md5(info.md5OriginDataHash, reinterpret_cast<UINT8*>(const_cast<char*>(payload)), info.dwOriginalSize);
    const PAK_CHUNK_INFO chunk{0, header.dwDataSize};
    std::ofstream file("native-raw.pak", std::ios::binary);
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    file.write(payload, sizeof(payload) - 1);
    file.write(name, sizeof(name));
    file.write(reinterpret_cast<const char*>(&info), sizeof(info));
    file.write(reinterpret_cast<const char*>(&chunk), sizeof(chunk));
    file.close();
    require(bool(file), "write native raw-entry fixture");
    CPakFile source, destination;
    require(source.Open("native-raw.pak") && source.VerifyFile(0), "read uncompressed payload");
    std::array<char, 2> partial;
    require(source.ReadFileData(0, partial.data(), partial.size()) && partial[0] == 'p' && partial[1] == 'l', "read raw prefix");
    require(destination.Create("copied-raw.pak") && destination.AddFileFromPak(name, &source, 0) && destination.Close(), "copy raw entry");
    require(destination.Open("copied-raw.pak") && destination.VerifyFile(0) && destination.Close() && source.Close(), "verify copied raw entry");
}
}

int main(int argc, char* argv[])
{
    try
    {
        require(argc == 2, "expected output directory");
        std::filesystem::create_directories(argv[1]);
        std::filesystem::current_path(argv[1]);
        exercise("named.pak", false, true, true, true);
        exercise("raw.pak", false, false, false, true);
        exercise("plain.pak", true, true, false, false);
        malformed_inputs();
        updates_and_limits();
        uncompressed_entry();
        std::cout << "PAK format, hash, MD5, empty-file and multichunk checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
