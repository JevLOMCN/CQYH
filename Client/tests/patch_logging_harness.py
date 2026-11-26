"""Use production logging functions/call sites with percent signs and long paths."""
import re


def fixtures(client, source, function):
    text = source(client / "client/PatchTools/pakpatcher/PakPatcher.cpp")
    parts = [r'''
#include <cstdarg>
namespace PatchLogTests {
using PFNMergeDirInfoProc = size_t (WINAPI*)(LPCWSTR, int);
HANDLE g_hConsole = nullptr;
std::wstring output;
int captured = 0;
int wprintf(const wchar_t* format, ...) {
    assert(!wcscmp(format, L"%ls"));
    va_list args; va_start(args, format);
    output = va_arg(args, const wchar_t*);
    va_end(args);
    return (int)output.size();
}
size_t WINAPI Capture(LPCWSTR message, int) { ++captured; output = message; return output.size(); }
''', function(text, "size_t WINAPI BuildinLog("), function(text, "void OutputInfo("), r'''
void Callers(PFNMergeDirInfoProc pfn, const std::string& text) {
''']
    calls = re.findall(r'OutputInfo\((pfn|NULL), (?:"%s", )?text.c_str\(\)\);', text)
    assert calls
    # Preserve each real call expression so unsafe dynamic formats cannot regress unnoticed.
    parts.extend(match.group(0) for match in re.finditer(
        r'OutputInfo\((?:pfn|NULL), (?:"%s", )?text.c_str\(\)\);', text))
    parts.append(r'''
}
void Run() {
    const std::string filename = "C:\\100%\\file %08x %s %n.txt";
    std::wstring wide_filename(filename.begin(), filename.end());
    captured = 0;
    Callers(Capture, filename);
''')
    parts.append("assert(captured == %d && output == wide_filename);" % calls.count("pfn"))
    parts.append(r'''
    BuildinLog(wide_filename.c_str(), 0);
    assert(output == wide_filename);
    for (int size : {0, 1, 1022, 1023, 1024, 4096}) {
        std::string message(size, 'M');
        captured = 0;
        OutputInfo(Capture, "%s", message.c_str());
        assert(captured == 1 && output == std::wstring(std::min(size, 1023), L'M'));
    }
    captured = 0;
    OutputInfo(Capture, nullptr);
    OutputInfo(nullptr, "%s", "ignored");
    assert(!captured);
    puts("Patch log literal-format and long-message regressions passed");
}
}
''')
    return "\n".join(parts)
