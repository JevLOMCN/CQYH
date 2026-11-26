#ifndef CLIENT_PATCH_DOWNLOAD_BUFFER_H
#define CLIENT_PATCH_DOWNLOAD_BUFFER_H

#include <cstddef>
#include <vector>

namespace ClientPatch
{
    // Grow from the bytes actually received. Never use an HTTP header as an
    // allocation size or expose bytes which the reader has not supplied.
    template<class Reader>
    bool ReadDownload(Reader& reader, std::vector<unsigned char>& bytes, size_t limit)
    {
        bytes.clear();
        unsigned char chunk[4096];
        for (;;)
        {
            const unsigned int count = reader.Read(chunk, sizeof(chunk));
            if (count == 0)
                return true;
            if (count > sizeof(chunk) || bytes.size() > limit || count > limit - bytes.size())
            {
                bytes.clear();
                return false;
            }
            bytes.insert(bytes.end(), chunk, chunk + count);
        }
    }
}

#endif
