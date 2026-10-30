#ifndef CQYH_BASELIB_FILE_OWNER_H
#define CQYH_BASELIB_FILE_OWNER_H

#include <cstdio>
#include <memory>

#include "std_type.h"
#include "ifile.h"

namespace cqyh
{
// IFile::Close destroys the concrete object; deleting an IFile* is invalid.
struct IFileDeleter
{
    void operator()(IFile* file) const noexcept
    {
        if (file != nullptr)
            file->Close();
    }
};

using FilePtr = std::unique_ptr<IFile, IFileDeleter>;

[[nodiscard]] inline FilePtr OpenFile(const char* name, const char* mode)
{
    return FilePtr(g_OpenFile(name, mode));
}

struct CFileDeleter
{
    void operator()(std::FILE* file) const noexcept
    {
        if (file != nullptr)
            std::fclose(file);
    }
};

using CFilePtr = std::unique_ptr<std::FILE, CFileDeleter>;
}

#endif  // CQYH_BASELIB_FILE_OWNER_H
