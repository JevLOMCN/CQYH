"""Exercise production substring and web configuration-path construction."""
import re


def harness(game, source, function):
    parts = [r'''
namespace ChatStringTests {
struct FString {
    std::string value;
    FString(const char* text = "") : value(text) {}
    FString(std::string text) : value(text) {}
    INT Len() const { return (INT)value.size(); }
    FString Mid(INT start, INT count) const {
        assert(start >= 0 && count >= 0 && start <= Len() && count <= Len() - start);
        return value.substr(start, count);
    }
    const char* operator*() const { return value.c_str(); }
    FString operator+(const char* text) const { return value + text; }
};
using TCHAR = char;
FString directory;
FString appGameConfigDir() { return directory; }
''']
    chat = (game / "CensoredWordList.cpp").read_text(encoding="utf-16")
    parts.append(function(chat, "FString _GetStrInStr("))
    paths = [game / "WebHtmlManager.cpp", *game.glob("WebHtmlManager - *.cpp")]
    for index, path in enumerate(paths):
        statements = re.findall(
            r'const FString ipFileName = [^\n]+;\s*const TCHAR\* ipFile = [^\n]+;',
            source(path))
        if len(statements) != 8:
            raise ValueError("Expected eight configuration paths: " + str(path))
        parts.append("void Paths" + str(index) + "() {")
        for statement in statements:
            parts.append("{ " + statement + r'''
                assert(std::string(ipFile) == directory.value + "IP.ini");
            }''')
        parts.append("}")
    parts.append(r'''
void Run() {
    for (const char* text : {"", "x", "abcdef"}) {
        FString input(text);
        const std::string original = input.value;
        for (int start : {-2147483647 - 1, -1, 0, 1, 4, 6, 2147483647}) {
            for (int end : {-2147483647 - 1, -1, 0, 1, 4, 6, 2147483647}) {
                const std::string expected = start >= 0 && end > start && end < input.Len()
                    ? original.substr(start, end - start) : "";
                assert(_GetStrInStr(input, start, end).value == expected);
                assert(input.value == original);
            }
        }
    }
    FString link("prefix,19,123,456,suffix");
    assert(_GetStrInStr(link, 7, 9).value == "19");
    assert(_GetStrInStr(link, 10, 13).value == "123");
    for (int length : {0, 1, 1017, 1018, 1023, 1024, 65536}) {
        directory.value.assign(length, 'D');
        Paths0(); Paths1();
    }
    puts("Chat substring bounds and long web configuration paths passed");
}
}
''')
    return "\n".join(parts)
