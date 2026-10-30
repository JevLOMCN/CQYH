#include "stdafx.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "pak_file.h"

namespace
{
namespace fs = std::filesystem;

class CurrentDirectory
{
public:
    explicit CurrentDirectory(const fs::path& directory) : previous(fs::current_path())
    {
        fs::create_directories(directory);
        fs::current_path(directory);
    }
    ~CurrentDirectory()
    {
        std::error_code error;
        fs::current_path(previous, error);
    }
    CurrentDirectory(const CurrentDirectory&) = delete;
    CurrentDirectory& operator=(const CurrentDirectory&) = delete;
private:
    fs::path previous;
};

bool traverse(CPakFile& archive, const fs::path& directory, bool compare)
{
    std::cout << "Traversing directory " << directory.string() << "...\n";
    for (const auto& entry : fs::directory_iterator(directory))
    {
        const auto name = entry.path().filename().string();
        const auto attributes = GetFileAttributesW(entry.path().c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES)
            throw std::runtime_error("Cannot read file attributes");
        if (name.starts_with('.') || (attributes & FILE_ATTRIBUTE_HIDDEN) || name == "Thumbs.db" ||
            name.ends_with(".max") || name.ends_with(".log") || name.ends_with(".bak"))
        {
            std::cout << "\tSkip " << name << '\n';
            continue;
        }
        // Following directory links can leave the input tree or form recursive cycles.
        if (entry.is_symlink() || (attributes & FILE_ATTRIBUTE_REPARSE_POINT))
        {
            std::cout << "\tSkip " << name << '\n';
            continue;
        }
        if (entry.is_directory())
        {
            if (!traverse(archive, entry.path(), compare))
                return false;
            continue;
        }
        if (!entry.is_regular_file())
            continue;
        const auto path = entry.path().string();
        if (!compare)
        {
            if (!archive.AddFile(path.c_str(), path.c_str()))
                return false;
            std::cout << "Added " << path << '\n';
            continue;
        }
        const int index = archive.FindFile(path.c_str());
        if (index == INVALID_PAK_FILE_INDEX || fs::file_size(entry.path()) != archive.GetFileSize(index))
        {
            std::cerr << "Missing file or different file size: " << path << '\n';
            return false;
        }
        std::vector<char> packed(archive.GetFileSize(index));
        if (!archive.ReadFileData(index, packed.data(), packed.size()))
            return false;
        std::ifstream file(entry.path(), std::ios::binary);
        if (!file)
            return false;
        const std::vector<char> disk{std::istreambuf_iterator<char>(file), {}};
        if (file.bad() || disk != packed)
        {
            std::cerr << "Different file contents: " << path << '\n';
            return false;
        }
        std::cout << "Verified " << path << '\n';
    }
    return true;
}

bool compare_package(const char* folder, const char* name)
{
    CPakFile archive;
    return archive.Open(name) && traverse(archive, folder, true) && archive.Close();
}

bool create_package(const char* folder, bool names)
{
    const auto name = std::string(folder) + ".pak";
    CPakFile archive;
    if (!fs::is_directory(folder) || !archive.Create(name.c_str()) ||
        !archive.SetPackageFlag(false, names, names, true) || !traverse(archive, folder, false) || !archive.Close())
        return false;
    return compare_package(folder, name.c_str());
}

bool verify_package(const char* name)
{
    CPakFile archive;
    if (!archive.Open(name))
        return false;
    std::unordered_map<unsigned long, int> hashes;
    for (int index = 0; index < archive.GetFileCount(); ++index)
    {
        unsigned long hash_a = 0, hash_b = 0;
        if (!archive.GetFileHashValue(index, hash_a, hash_b))
            return false;
        const std::string entry_name(archive.GetFileName(index));
        const auto [previous, inserted] = hashes.try_emplace(hash_b, index);
        if (!inserted)
            std::cout << "Hash value conflicted " << entry_name << " : " << archive.GetFileName(previous->second) << '\n';
        if (!archive.VerifyFile(index))
            return false;
        std::cout << "Verified " << entry_name << '\n';
    }
    return archive.Close();
}

bool extract_package(const char* name, const char* destination)
{
    CPakFile archive;
    if (!archive.Open(name))
        return false;
    CurrentDirectory directory(destination);
    for (int index = 0; index < archive.GetFileCount(); ++index)
    {
        if (archive.IsDeletedFile(index))
            continue;
        if (!archive.ExtractFile(index))
            return false;
        std::cout << "Extracted " << archive.GetFileName(index) << '\n';
    }
    return archive.Close();
}

bool same_file(const char* first, const char* second)
{
    std::error_code error;
    return fs::equivalent(first, second, error) || fs::weakly_canonical(first) == fs::weakly_canonical(second);
}

bool append_package(const char* destination, const char* source)
{
    if (same_file(destination, source))
        throw std::runtime_error("Source and destination archives must differ");
    CPakFile dest;
    CPakFile src;
    if (!dest.Open(destination) || !src.Open(source))
        return false;
    for (int index = 0; index < src.GetFileCount(); ++index)
    {
        const std::string name(src.GetFileName(index));
        if (src.IsDeletedFile(index))
        {
            // A repeated deletion is already applied when the destination has no entry.
            if (dest.FindFile(name.c_str()) != INVALID_PAK_FILE_INDEX && !dest.DelFile(name.c_str()))
                return false;
        }
        else if (!dest.AddFileFromPak(name.c_str(), &src, index))
            return false;
    }
    return src.Close() && dest.Close() && verify_package(destination);
}

bool generate_patch(const char* old_name, const char* new_name, const char* patch_name)
{
    if (same_file(old_name, patch_name) || same_file(new_name, patch_name))
        throw std::runtime_error("Patch output must differ from both input archives");
    CPakFile old_archive, new_archive, patch;
    if (!old_archive.Open(old_name) || !new_archive.Open(new_name) || !patch.Create(patch_name))
        return false;
    for (int index = 0; index < new_archive.GetFileCount(); ++index)
    {
        if (new_archive.IsDeletedFile(index))
            continue;
        const std::string name(new_archive.GetFileName(index));
        const int old_index = old_archive.FindFile(name.c_str());
        bool changed = old_index == INVALID_PAK_FILE_INDEX || old_archive.IsDeletedFile(old_index);
        if (!changed)
        {
            unsigned char old_md5[16], new_md5[16];
            if (!old_archive.GetFileMD5Value(old_index, old_md5) || !new_archive.GetFileMD5Value(index, new_md5))
                return false;
            changed = !std::ranges::equal(old_md5, new_md5);
        }
        if (changed && !patch.AddFileFromPak(name.c_str(), &new_archive, index))
            return false;
    }
    for (int index = 0; index < old_archive.GetFileCount(); ++index)
    {
        const std::string name(old_archive.GetFileName(index));
        if (new_archive.FindFile(name.c_str()) == INVALID_PAK_FILE_INDEX)
        {
            constexpr unsigned deleted = 0xdddddddd;
            if (!patch.AddFileFromData(name.c_str(), 0, 0, reinterpret_cast<const char*>(&deleted), sizeof(deleted)) ||
                !patch.DelFile(name.c_str()))
                return false;
        }
    }
    return old_archive.Close() && new_archive.Close() && patch.Close() && verify_package(patch_name);
}

bool list_package(const char* name, bool md5)
{
    std::cout << (md5 ? "File MD5 list in " : "File list in ") << name << ":\n";
    CPakFile archive;
    if (!archive.Open(name))
        return false;
    for (int index = 0; index < archive.GetFileCount(); ++index)
    {
        const char* entry_name = archive.GetFileName(index);
        if (archive.IsDeletedFile(index))
            std::cout << " Deleted " << entry_name << '\n';
        else if (!md5)
            std::cout << std::format(" {:8} {}\n", archive.GetFileSize(index), entry_name);
        else
        {
            unsigned char digest[16];
            if (!archive.GetFileMD5Value(index, digest))
                return false;
            for (const auto byte : digest)
                std::printf("%02X", byte);
            std::printf(" *%s\n", entry_name);
        }
    }
    return archive.Close();
}

void help()
{
    std::cout << "pak create FolderName\npak createraw FolderName\npak list FileName\n"
        "pak listmd5 FileName\npak verify [FolderName] FileName\npak extract FileName [FolderName]\n"
        "pak append DestFileName SourceFileName\npak patch OldFileName NewFileName PatchFileName\n";
}
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        help();
        return EXIT_FAILURE;
    }
    try
    {
        std::string command(argv[1]);
        std::ranges::transform(command, command.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        bool result = false;
        if (command == "create" && argc == 3)
            result = create_package(argv[2], true);
        else if (command == "createraw" && argc == 3)
            result = create_package(argv[2], false);
        else if (command == "verify" && argc == 3)
            result = verify_package(argv[2]);
        else if (command == "verify" && argc == 4)
            result = compare_package(argv[2], argv[3]);
        else if (command == "extract" && (argc == 3 || argc == 4))
            result = extract_package(argv[2], argc == 4 ? argv[3] : ".");
        else if (command == "append" && argc == 4)
            result = append_package(argv[2], argv[3]);
        else if (command == "patch" && argc == 5)
            result = generate_patch(argv[2], argv[3], argv[4]);
        else if ((command == "list" || command == "listmd5") && argc == 3)
            result = list_package(argv[2], command == "listmd5");
        else
        {
            help();
            return EXIT_FAILURE;
        }
        if (!result)
            std::cerr << "pak: operation failed\n";
        return result ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    catch (const std::exception& error)
    {
        std::cerr << "pak: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
