#ifndef CQYH_BASELIB_DIR_H
#define CQYH_BASELIB_DIR_H

#include <filesystem>
#include <system_error>

#if defined(_WIN32)
#include <Windows.h>
#else
#include "std_type.h"
#endif

#include "process_error.h"

// Nonrecursive creation fails if the directory already exists.
// Recursive creation also succeeds for an existing directory.
BOOL g_CreateDirectory(const char* dirName, BOOL bforce);
BOOL g_DirectoryExists(const char* dirName);

// Removes a directory tree synchronously. Missing paths and regular files fail.
BOOL g_DeleteDirectory(const char* dirName);

// Visits matching immediate children, skipping dot names and Windows hidden entries.
// Returns FALSE on a filesystem error or when the callback returns false.
template<class TFunc>
inline BOOL g_DirectoryTraverse(const char* szDirectoryPath, TFunc& rFunc)
{
    if (szDirectoryPath == nullptr || *szDirectoryPath == '\0')
        return FALSE;

    std::error_code error;
    std::filesystem::directory_iterator entry(szDirectoryPath, error);
    const std::filesystem::directory_iterator end;
    if (error)
        return FALSE;

    while (entry != end)
    {
        const auto name = entry->path().filename().string();
        if (!name.empty() && name.front() != '.')
        {
#if defined(_WIN32)
            // Hidden attributes have no standard-library equivalent.
            const auto attributes = GetFileAttributesW(entry->path().c_str());
            if (attributes == INVALID_FILE_ATTRIBUTES)
                return FALSE;
            const bool hidden = (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;
#else
            const bool hidden = false;
#endif
            if (!hidden)
            {
                const bool directory = entry->is_directory(error);
                if (error)
                    return FALSE;
                if (directory == static_cast<bool>(rFunc.RequireDirectory()))
                {
                    const auto callbackSucceeded = static_cast<bool>(rFunc(name.c_str()));
                    LOG_CHECK_ERROR(callbackSucceeded);
                    if (!callbackSucceeded)
                        return FALSE;
                }
            }
        }
        entry.increment(error);
        if (error)
            return FALSE;
    }
    return TRUE;
}

#endif  // CQYH_BASELIB_DIR_H
