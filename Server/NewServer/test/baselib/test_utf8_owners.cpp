#include "fake_codepages.h"
#undef MultiByteToWideChar
#undef WideCharToMultiByte
#include "utf8.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <vector>

namespace
{
std::array<void*, 32> arrays{};
int liveArrays = 0, failAllocation = 0, apiCalls = 0, failApi = 0;
int checks = 0, failures = 0;
void check(bool condition, const char* description)
{
    ++checks;
    if (!condition)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
std::string native_conversion(const char* input, UINT from, UINT to)
{
    const int wideCount = ::MultiByteToWideChar(from, 0, input, -1, nullptr, 0);
    std::vector<wchar_t> wide(wideCount);
    ::MultiByteToWideChar(from, 0, input, -1, wide.data(), wideCount);
    const int bytes = ::WideCharToMultiByte(to, 0, wide.data(), -1, nullptr, 0, nullptr, nullptr);
    std::string output(bytes, '\0');
    ::WideCharToMultiByte(to, 0, wide.data(), -1, output.data(), bytes, nullptr, nullptr);
    output.pop_back();
    return output;
}
}

void* operator new[](std::size_t size)
{
    if (failAllocation > 0 && --failAllocation == 0)
        throw std::bad_alloc();
    void* memory = std::malloc(size ? size : 1);
    if (!memory)
        throw std::bad_alloc();
    for (auto& slot : arrays)
        if (!slot)
        {
            slot = memory;
            ++liveArrays;
            return memory;
        }
    std::free(memory);
    throw std::bad_alloc();
}
void operator delete[](void* memory) noexcept
{
    if (!memory)
        return;
    for (auto& slot : arrays)
        if (slot == memory)
        {
            slot = nullptr;
            --liveArrays;
            break;
        }
    std::free(memory);
}
void operator delete[](void* memory, std::size_t) noexcept { ::operator delete[](memory); }

int WINAPI TestMultiByteToWideChar(UINT page, DWORD flags, LPCCH source, int length, LPWSTR output, int capacity)
{
    if (++apiCalls == failApi)
        return 0;
    return ::MultiByteToWideChar(page, flags, source, length, output, capacity);
}
int WINAPI TestWideCharToMultiByte(UINT page, DWORD flags, LPCWCH source, int length, LPSTR output,
                                 int capacity, LPCCH fallback, LPBOOL substituted)
{
    if (++apiCalls == failApi)
        return 0;
    return ::WideCharToMultiByte(page, flags, source, length, output, capacity, fallback, substituted);
}

int main(int argc, char** argv)
{
    const bool baseline = argc > 1 && std::string_view(argv[1]) == "--baseline";
    for (const char* sample : {"", "plain text 100%", "caf\xC3\xA9 \xE4\xBD\xA0", "\xFF"})
        for (const bool toUtf8 : {false, true})
        {
            std::string input(sample);
            const auto expected = native_conversion(input.c_str(), toUtf8 ? CP_ACP : CP_UTF8,
                                                     toUtf8 ? CP_UTF8 : CP_ACP);
            apiCalls = 0;
            {
                std::unique_ptr<char[]> result(toUtf8 ? char_to_utf8(input.data()) : utf8_to_char(input.c_str()));
                check(result && std::string(result.get()) == expected && liveArrays == 1,
                      "native code-page flags, substitution and caller-owned result remain compatible");
            }
            check(liveArrays == 0, "successful conversion releases its temporary and returned arrays");
        }
    if (!baseline)
        check(!char_to_utf8(nullptr) && !utf8_to_char(nullptr) && liveArrays == 0,
              "null input is rejected without allocating or reading a buffer");
    for (const bool toUtf8 : {false, true})
    {
        char input[] = "allocation failure";
        apiCalls = 0;
        failAllocation = 2;
        bool caught = false;
        try
        {
            std::unique_ptr<char[]> result(toUtf8 ? char_to_utf8(input) : utf8_to_char(input));
        }
        catch (const std::bad_alloc&) { caught = true; }
        failAllocation = 0;
        check(caught && liveArrays == 0, "failure allocating the output releases the already allocated wide buffer");
        // Release the old implementation's reproduced leak before another case.
        for (auto* allocation : arrays)
            if (allocation)
                ::operator delete[](allocation);
        if (!baseline)
            for (int failure = 1; failure <= 4; ++failure)
            {
                apiCalls = 0;
                failApi = failure;
                std::unique_ptr<char[]> result(toUtf8 ? char_to_utf8(input) : utf8_to_char(input));
                check(!result && liveArrays == 0, "each failed size/fill conversion releases all temporary ownership");
            }
        failApi = 0;
    }
    std::printf("%d encoding ownership checks, %d failures (ACP %u)\n", checks, failures, GetACP());
    return failures ? 1 : 0;
}
