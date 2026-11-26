#ifndef CLIENT_PATCH_PATH_H
#define CLIENT_PATCH_PATH_H

#include <windows.h>
#include <cstddef>
#include <string>
#include <vector>

namespace ClientPatch
{
    template<size_t Size>
    bool ToWide(const char* input, wchar_t (&output)[Size])
    {
        output[0] = 0;
        if (!input || !MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, input, -1, output, (int)Size))
        {
            output[0] = 0;
            return false;
        }
        return true;
    }

    template<size_t Size>
    bool ToAnsi(const wchar_t* input, char (&output)[Size])
    {
        output[0] = 0;
        if (!input || !WideCharToMultiByte(CP_ACP, 0, input, -1, output, (int)Size, NULL, NULL))
        {
            output[0] = 0;
            return false;
        }
        return true;
    }

    inline DWORD ReadDirectory(DWORD capacity, char* output) { return ::GetCurrentDirectoryA(capacity, output); }
    inline DWORD ReadDirectory(DWORD capacity, wchar_t* output) { return ::GetCurrentDirectoryW(capacity, output); }
    inline BOOL ChangeDirectory(const char* path) { return ::SetCurrentDirectoryA(path); }
    inline BOOL ChangeDirectory(const wchar_t* path) { return ::SetCurrentDirectoryW(path); }
    inline DWORD ReadModule(DWORD capacity, char* output) { return ::GetModuleFileNameA(NULL, output, capacity); }
    inline DWORD ReadModule(DWORD capacity, wchar_t* output) { return ::GetModuleFileNameW(NULL, output, capacity); }

    template<class Character, size_t Size>
    bool ReadCurrentDirectory(Character (&output)[Size])
    {
        const DWORD length = ReadDirectory((DWORD)Size, output);
        if (!length || length >= Size)
        {
            output[0] = 0;
            return false;
        }
        return true;
    }

    template<class Character, size_t Size>
    bool ReadModulePath(Character (&output)[Size])
    {
        const DWORD length = ReadModule((DWORD)Size, output);
        if (!length || length >= Size)
        {
            output[0] = 0;
            return false;
        }
        return true;
    }

    template<class Character>
    class ScopedCurrentDirectory
    {
        std::basic_string<Character> previous;
        bool changed;
        ScopedCurrentDirectory(const ScopedCurrentDirectory&);
        ScopedCurrentDirectory& operator=(const ScopedCurrentDirectory&);
    public:
        ScopedCurrentDirectory() : changed(false) {}
        ~ScopedCurrentDirectory() { Restore(); }

        bool Change(const Character* path)
        {
            if (changed || !path)
                return false;
            const DWORD capacity = ReadDirectory(0, (Character*)NULL);
            if (!capacity)
                return false;
            std::vector<Character> buffer(capacity);
            const DWORD length = ReadDirectory(capacity, &buffer[0]);
            if (!length || length >= capacity)
                return false;
            previous.assign(&buffer[0], length);
            if (!ChangeDirectory(path))
                return false;
            changed = true;
            return true;
        }

        bool Restore()
        {
            if (changed && !ChangeDirectory(previous.c_str()))
                return false;
            changed = false;
            return true;
        }
    };
}

#endif
