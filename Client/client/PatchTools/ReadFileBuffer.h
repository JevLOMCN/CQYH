#ifndef CLIENT_PATCH_READ_FILE_BUFFER_H
#define CLIENT_PATCH_READ_FILE_BUFFER_H

#include <cstddef>
#include <vector>

namespace ClientPatch
{
    // CFile::Read takes a UINT. Reject unsupported lengths before narrowing or allocating.
    template<class Reader>
    bool ReadFileBuffer(Reader& file, std::vector<unsigned char>& bytes,
        size_t maximum = (unsigned int)-1)
    {
        bytes.clear();
        const unsigned long long length = file.GetLength();
        if (length > maximum || length > (unsigned int)-1)
            return false;
        bytes.resize((size_t)length);
        if (length && file.Read(&bytes[0], (unsigned int)length) != length)
        {
            bytes.clear();
            return false;
        }
        return true;
    }

    template<class Reader>
    bool HasUnicodeBom(Reader& file)
    {
        unsigned char bytes[2] = {0, 0};
        return file.Read(bytes, sizeof(bytes)) == sizeof(bytes) &&
            ((bytes[0] == 0xfe && bytes[1] == 0xff) ||
             (bytes[0] == 0xff && bytes[1] == 0xfe));
    }
}

#endif
