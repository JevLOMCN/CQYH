"""Actual encode/decode utility entry point with injected CRT I/O failures."""
import re


def harness(client, source, function):
    text = source(client / "client/UE3/Development/Tools/encry/encry/encry.cpp")
    text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
    parts = [r'''
#include <cerrno>
#pragma push_macro("_T")
#undef _T
#define _T(value) L##value
#pragma push_macro("PATH_SEPARATOR")
#undef PATH_SEPARATOR
#define PATH_SEPARATOR L"\\"
#pragma push_macro("appIsPathSeparator")
#undef appIsPathSeparator
#define appIsPathSeparator(ch) ((ch) == L'\\')
namespace EncodingToolTests {
using TCHAR = wchar_t;
std::vector<std::wstring> directories;
bool fail_directory = false, exists_directory = false;
INT _tmkdir(const wchar_t* path) {
    directories.push_back(path);
    errno = exists_directory ? EEXIST : EACCES;
    return fail_directory || exists_directory ? -1 : 0;
}
size_t _tcslen(const wchar_t* path) { return wcslen(path); }
''', function(text, "bool _MakeDirectory("), r'''
''', function(text, "class FArchiveEncodeDncode\n") + ";", r'''
struct FILE { bool output; bool closed = false; } input_file{false}, output_file{true};
std::vector<unsigned char> input, output;
std::wstring output_name;
enum Failure { None, InputOpen, SeekEnd, Tell, SeekStart, Allocate, ShortRead, InputClose, OutputOpen, ShortWrite, OutputClose };
Failure failure = None;
INT opens = 0, closes = 0, allocations = 0;
INT _tfopen_s(FILE** file, const wchar_t* name, const wchar_t* mode) {
    ++opens; bool write = mode[0] == L'w';
    *file = nullptr;
    if (failure == (write ? OutputOpen : InputOpen)) return 1;
    *file = write ? &output_file : &input_file;
    assert(!(*file)->closed);
    if (write) output_name = name;
    return 0;
}
INT fseek(FILE* file, long, INT origin) {
    assert(file == &input_file && !file->closed);
    return (failure == SeekEnd && origin == SEEK_END) || (failure == SeekStart && origin == SEEK_SET) ? -1 : 0;
}
long ftell(FILE* file) { assert(file == &input_file && !file->closed); return failure == Tell ? -1 : (long)input.size(); }
size_t fread(void* data, size_t width, size_t count, FILE* file) {
    assert(file == &input_file && !file->closed && width == 1 && count == input.size());
    size_t supplied = failure == ShortRead && count ? count - 1 : count;
    if (supplied) memcpy(data, input.data(), supplied);
    return supplied;
}
size_t fwrite(const void* data, size_t width, size_t count, FILE* file) {
    assert(file == &output_file && !file->closed && width == 1 && count == input.size());
    size_t written = failure == ShortWrite && count ? count - 1 : count;
    const auto* bytes = static_cast<const unsigned char*>(data);
    output.assign(bytes, bytes + written);
    return written;
}
INT fclose(FILE* file) {
    assert(file && !file->closed); file->closed = true; ++closes;
    return failure == (file->output ? OutputClose : InputClose) ? -1 : 0;
}
void* malloc(size_t bytes) {
    if (failure == Allocate) return nullptr;
    ++allocations; return std::malloc(bytes);
}
void free(void* data) { if (data) { --allocations; std::free(data); } }
template<class... Args> INT _tprintf(const wchar_t*, Args...) { return 0; }
''', function(text, "int _tmain(int argc, TCHAR* argv[])"), r'''
void Reset(Failure fail = None, size_t length = 256) {
    assert(!allocations);
    failure = fail; opens = closes = 0; input_file = {false}; output_file = {true};
    input.resize(length); output.clear(); output_name.clear();
    for (size_t i = 0; i < length; ++i) input[i] = (unsigned char)i;
}
void Run() {
    directories.clear();
    assert(!_MakeDirectory(nullptr) && !_MakeDirectory(L""));
    for (size_t length : {size_t(256), size_t(257), size_t(65536)})
        assert(!_MakeDirectory(std::wstring(length, L'x').c_str()));
    assert(directories.empty());
    assert(_MakeDirectory(std::wstring(255, L'x').c_str()) && directories.size() == 1);
    directories.clear();
    assert(_MakeDirectory(L"a\\b\\c") && directories == std::vector<std::wstring>({L"a", L"a\\b", L"a\\b\\c"}));
    fail_directory = true; assert(!_MakeDirectory(L"a\\b")); fail_directory = false;
    exists_directory = true; assert(_MakeDirectory(L"a\\b")); exists_directory = false;
    wchar_t program[] = L"encry", from[] = L"input", to[] = L"output", mode[] = L"decode";
    wchar_t* none[] = {nullptr};
    wchar_t* program_only[] = {program, nullptr};
    wchar_t* missing_input[] = {program, nullptr, to, nullptr};
    wchar_t* missing_output[] = {program, from, nullptr, nullptr};
    Reset();
    assert(_tmain(0, nullptr) == -1 && _tmain(0, none) == -1 && _tmain(1, program_only) == -1);
    assert(_tmain(3, missing_input) == -1 && _tmain(3, missing_output) == -1 && !opens);
    // Exact-size argv includes only argc arguments plus the null terminator;
    // automatic output used to print argv[2], which is null in this case.
    wchar_t* automatic[] = {program, from, nullptr};
    wchar_t* explicit_output[] = {program, from, to, nullptr};
    wchar_t* explicit_decode[] = {program, from, to, mode, nullptr};
    for (size_t length : {size_t(0), size_t(1), size_t(256), size_t(4097)}) {
        Reset(None, length);
        auto original = input;
        assert(_tmain(3, explicit_output) == 0 && opens == 2 && closes == 2 && !allocations && output_name == L"output");
        auto encoded = output;
        assert(encoded.size() == original.size());
        Reset(None, length); input = encoded;
        assert(_tmain(4, explicit_decode) == 0 && output == original && closes == 2 && !allocations);
        Reset(None, length); input = encoded;
        assert(_tmain(2, automatic) == 0 && output == original && output_name == L"input.txt" && closes == 2 && !allocations);
    }
    for (Failure fail : {InputOpen, SeekEnd, Tell, SeekStart, Allocate, ShortRead, InputClose, OutputOpen, ShortWrite, OutputClose}) {
        Reset(fail);
        assert(_tmain(2, automatic) == -1 && !allocations);
        if (fail == InputOpen) assert(opens == 1 && !closes);
        else if (fail <= InputClose) assert(opens == 1 && closes == 1 && output.empty());
        else if (fail == OutputOpen) assert(opens == 2 && closes == 1 && output.empty());
        else assert(opens == 2 && closes == 2);
    }
    Reset();
    puts("Encoding utility bounds directory paths, terminates creation recursion, rejects missing arguments/short I/O and preserves encode/decode round trips");
}
}
#pragma pop_macro("appIsPathSeparator")
#pragma pop_macro("PATH_SEPARATOR")
#pragma pop_macro("_T")
''']
    return "\n".join(parts)
