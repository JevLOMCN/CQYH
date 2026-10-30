#include "stdafx.h"
#include "dir.h"

#include <filesystem>
#include <system_error>

BOOL g_CreateDirectory(const char* dirName, BOOL bforce)
{
    if (dirName == nullptr || *dirName == '\0')
        return FALSE;

    const std::filesystem::path path(dirName);
    std::error_code error;
    if (!bforce)
        return std::filesystem::create_directory(path, error) && !error;

    if (std::filesystem::create_directories(path, error))
        return TRUE;

    return !error && std::filesystem::is_directory(path, error) && !error;
}

BOOL g_DirectoryExists(const char* dirName)
{
    if (dirName == nullptr || *dirName == '\0')
        return FALSE;

    std::error_code error;
    return std::filesystem::is_directory(dirName, error) && !error;
}

BOOL g_DeleteDirectory(const char* dirName)
{
    if (dirName == nullptr || *dirName == '\0')
        return FALSE;

    const std::filesystem::path path(dirName);
    std::error_code error;
    if (!std::filesystem::is_directory(path, error) || error)
    {
        if (error)
            ERR("Cannot delete directory %s (filesystem error=%d)", dirName, error.value());
        else
            ERR("Cannot delete directory %s: path is not a directory", dirName);
        return FALSE;
    }

    INF("Deleting directory %s", dirName);
    const auto removed = std::filesystem::remove_all(path, error);
    if (error)
        ERR("Failed to delete directory %s (filesystem error=%d)", dirName, error.value());
    return !error && removed != 0;
}

