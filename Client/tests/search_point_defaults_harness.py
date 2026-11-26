"""Actual generated no-init string/record and search-point XML loading method."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', text)

    parts = [r'''
#pragma push_macro("ANSI_TO_TCHAR")
#pragma push_macro("TCHAR_TO_ANSI")
#undef ANSI_TO_TCHAR
#undef TCHAR_TO_ANSI
#define ANSI_TO_TCHAR(value) (value)
#define TCHAR_TO_ANSI(value) (value)
namespace SearchPointDefaultsTests {
using TCHAR = char;
enum EEventParm { EC_EventParm };
enum ENoInit { E_NoInit };
enum EForceInit { E_ForceInit };
size_t appStrlen(const char* value) { return strlen(value); }
std::set<void*> owned;
struct Allocator {
    char* data;
    void ResizeAllocation(INT, INT count, size_t size) {
        if (data) { assert(owned.erase(data) == 1); free(data); }
        data = count ? static_cast<char*>(malloc(size_t(count) * size)) : nullptr;
        if (data) assert(owned.insert(data).second);
    }
};
// Retain raw allocator/count state as in FString. The no-init base constructor
// deliberately poisons that state, making the missing event initializer fail.
struct FString {
    Allocator AllocatorInstance;
    INT ArrayNum, ArrayMax;
    FString() : AllocatorInstance{nullptr}, ArrayNum(0), ArrayMax(0) {}
    explicit FString(ENoInit) { memset(this, 0xa5, sizeof(*this)); }
    FString(const char* value) : FString() { *this = value; }
    FString(const FString& value) : FString() { *this = *value; }
    ~FString() { AllocatorInstance.ResizeAllocation(0, 0, 1); }
    FString& operator=(const char* value) {
        INT count = *value ? INT(strlen(value) + 1) : 0;
        AllocatorInstance.ResizeAllocation(0, count, 1); ArrayNum = ArrayMax = count;
        if (count) memcpy(GetData(), value, count); return *this;
    }
    FString& operator=(const FString& value) { if (this != &value) *this = *value; return *this; }
    char* GetTypedData() const { return AllocatorInstance.data; }
    char* GetData() const { return AllocatorInstance.data; }
    const char* operator*() const { return ArrayNum ? GetData() : ""; }
    INT Num() const { return ArrayNum; }
    INT Len() const { return ArrayNum ? ArrayNum - 1 : 0; }
    FString Left(INT count) const { return FString(std::string(**this).substr(0, std::max(0, count)).c_str()); }
    FString operator+(const FString& value) const { return FString((std::string(**this) + *value).c_str()); }
};
''']
    parts.append(function(clean(source(game.parent / "Core/Inc/UnString.h")), "struct FStringNoInit :") + ";")
    raw = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = raw.decode("utf-16" if raw.startswith(b"\xff\xfe") else "latin1").replace("\r", "")
    parts.append(function(generated, "struct FSearchPointObject\n") + ";")
    parts.append(r'''
template<class T> struct TArray : ItemMeshBoundsTests::TArray<T> {};
struct Node {
    std::string tag;
    std::map<std::string, std::string> attributes;
};
std::map<std::string, std::vector<Node>> documents;
struct XMLElement {
    const std::vector<Node>* nodes = nullptr;
    INT index = -1;
    XMLElement firstChild() const { return {nodes, 0}; }
    bool isNull() const { return !nodes || index < 0 || index >= INT(nodes->size()); }
    std::string name() const { return nodes->at(index).tag; }
    std::string getAttribute(const char* key) const {
        const auto& attributes = nodes->at(index).attributes;
        auto found = attributes.find(key); return found == attributes.end() ? "" : found->second;
    }
    XMLElement nextSibling() const { return {nodes, index + 1}; }
};
struct XMLDoc {
    const std::vector<Node>* nodes = nullptr;
    bool loadFromFile(const char* file) {
        auto found = documents.find(file);
        if (found == documents.end()) return false;
        nodes = &found->second; return true;
    }
    XMLElement getRoot() const { return {nodes, -1}; }
};
INT getIntAttribuate(const char* key, const XMLElement& element) { return atoi(element.getAttribute(key).c_str()); }
struct Config {
    void GetString(const char*, const char*, FString& out, const char*) { out = "maps"; }
} config;
Config* GConfig = &config;
const char* GGameIni = "game.ini";
struct FileManager {
    std::string current = "original";
    std::vector<std::string> files;
    FString GetCurrentDirectory() const { return FString(current.c_str()); }
    void SetCurDirectory(const char* value) { current = value; }
    void FindFiles(TArray<FString>& out, const char*, UBOOL, UBOOL) {
        for (auto& file : files) out.AddItem(FString(file.c_str()));
    }
} files;
FileManager* GFileManager = &files;
struct Warning {
    INT begins = 0, ends = 0;
    void BeginSlowTask(const char*, UBOOL) { ++begins; }
    void EndSlowTask() { ++ends; }
    void StatusUpdatef(INT, INT, const char*, const char*) {}
} warning;
Warning* GWarn = &warning;
struct UGlobalMapInfo {
    TArray<FSearchPointObject> SearchPoints;
    static UGlobalMapInfo* active;
    static UGlobalMapInfo* Singleton() { return active; }
    INT FindMapID(const FString& name) { return !strncmp(*name, "valid", 5) ? 1 : 0; }
    void LoadSearchPointFromXMLFiles();
};
UGlobalMapInfo* UGlobalMapInfo::active = nullptr;
''')
    parts.append(function(clean(source(game / "Src/GlobalMapInfo.cpp")),
                          "void UGlobalMapInfo::LoadSearchPointFromXMLFiles()"))
    parts.append(r'''
void Run() {
    {
        UGlobalMapInfo map; UGlobalMapInfo::active = &map;
        files.files = {"valid.search", "invalid.search", "valid_missing.search"};
        documents["valid.search"] = {{"Other", {}}, {"SearchPoint", {{"SearchPointID", "7"}, {"SearchPointName", "town"}}},
            {"SearchPoint", {{"SearchPointID", "8"}, {"SearchPointName", "gate"}}}, {"SearchPoint", {}}};
        documents["invalid.search"] = {{"SearchPoint", {{"SearchPointID", "99"}}}};
        map.LoadSearchPointFromXMLFiles();
        assert(map.SearchPoints.Num() == 3 && files.current == "original");
        assert(map.SearchPoints(0).SearchPointID == 7 && !strcmp(*map.SearchPoints(0).SearchPointName, "town"));
        assert(map.SearchPoints(1).SearchPointID == 8 && !strcmp(*map.SearchPoints(1).SearchPointName, "gate"));
        assert(!map.SearchPoints(2).SearchPointID && !map.SearchPoints(2).SearchPointName.Num());
        documents["valid.search"][1].attributes["SearchPointName"] = "changed";
        assert(!strcmp(*map.SearchPoints(0).SearchPointName, "town"));
        for (INT length : {0, 1, 1024, 65536}) {
            std::string text(length, 'x');
            documents["valid.search"] = {{"SearchPoint", {{"SearchPointID", "42"}, {"SearchPointName", text}}}};
            map.LoadSearchPointFromXMLFiles();
            assert(map.SearchPoints.Num() == 1 && map.SearchPoints(0).SearchPointID == 42);
            assert(std::string(*map.SearchPoints(0).SearchPointName) == text);
            FSearchPointObject copy = map.SearchPoints(0);
            map.SearchPoints.Empty();
            assert(std::string(*copy.SearchPointName) == text); // Copied record owns its string.
        }
        files.files.clear(); map.LoadSearchPointFromXMLFiles();
        assert(!map.SearchPoints.Num() && warning.begins == warning.ends && files.current == "original");
    }
    assert(owned.empty()); UGlobalMapInfo::active = nullptr; documents.clear();
    puts("Search-point XML records initialize real FStringNoInit allocator state; assignment/copies/reload cleanup passed");
}
}
#pragma pop_macro("TCHAR_TO_ANSI")
#pragma pop_macro("ANSI_TO_TCHAR")
''')
    return "\n".join(parts)
