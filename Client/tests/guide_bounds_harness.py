"""Real guide package loader and the picture-selection block from attachment."""
import re


def harness(game, source, function):
    def clean(path):
        # Preserve comment-like sequences inside literals such as "img://".
        return re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                      lambda m: "" if m.group().startswith(("//", "/*")) else m.group(), source(game / path))

    header = clean("MMOGuideData.h")
    data = clean("MMOGuideData.cpp")
    parts = [r'''
#pragma push_macro("debugIDf")
#undef debugIDf
#define debugIDf(...) ((void)0)
namespace GuideBoundsTests {
using FString = AttributeRecordTests::FString;
using TCHAR = char;
using FVector2D = ::FVector2D;
using EGuideEventType = INT; using EDOODAD_TYPE = INT; using EPlayerClass = INT;
using EGuideBoxType = INT; using EGuideArrowTipsType = INT; using ETraceMenuBarType = INT; using EMenuType = INT;
enum { EGDT_Null, EGUET_Null, E_dt_Invalid, EPCT_None, EGBT_Null, EGATT_Null,
       ETMBT_Right, EMenuType_Bag, EMenuType_Child_Bag, JT_TOTAL, LOAD_Verify };
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    T& operator()(INT index) { return values.at(index); }
    const T& operator()(INT index) const { return values.at(index); }
    void AddItem(const T& value) { values.push_back(value); }
};
''']
    parts.append(function(header, "enum\n") + ";")
    parts.append(function(header, "struct GuideData\n") + ";")
    parts.append(r'''
struct Resource { FString path; FString GetPathName() { return path; } };
struct Requirement { INT ItemId = 0, SkillId = 0; };
struct GuidePicture { Resource* Picture = nullptr; };
struct GuideTargetComponent { FString TargetComponentName; };
struct UGuide : GuideData {
    INT template_id = 1, GuideType = 1, ActivateGuideType = 0, DeactiveGuideType = 0;
    FString GuideDesc = "description";
    TArray<Requirement> ItemReqNew, SkillReq;
    TArray<GuidePicture> Picture;
    Resource *GuideSound = nullptr, *GuideSoundIcon = nullptr;
    TArray<GuideTargetComponent> TargetComponent;
    FVector2D TargetComponentOffset{0, 0};
    UBOOL WithBox = FALSE;
    struct { INT Width = 0, Height = 0; } BoxSize;
    INT rooted = 0;
    UGuide() { Enable = TRUE; }
    INT GetTemplateID() { return template_id; }
    void AddToRoot() { ++rooted; }
};
struct UPackage {
    std::vector<UGuide*> guides;
    bool full = false;
    bool IsFullyLoaded() { return full; }
    void FullyLoad() { full = true; }
    INT GetNetObjectCount(INT) { return (INT)guides.size(); }
    UGuide* GetNetObjectAtIndex(INT index) { return guides.at(index); }
} package;
bool load_ok = true;
struct UObject {
    static UPackage* LoadPackage(void*, const char*, INT) { return load_ok ? &package : nullptr; }
};
template<class T> T* Cast(UGuide* guide) { return static_cast<T*>(guide); }
struct Config {
    void GetArray(const char*, const char*, TArray<FString>& dirs, INT) { dirs.AddItem("directory"); }
} config;
Config* GConfig = &config;
INT GGameIni = 0;
namespace MMO { void FormatTempleteDataLoadingDir(TArray<FString>&) {} }
void appFindFilesInDirectory(TArray<FString>& dirs, const char*, UBOOL, UBOOL) { dirs.AddItem("package"); }
void M_AddDataError(const char*, const FString&) {}
using GuideDataMap = std::map<UINT32, GuideData>;
using GuideList = std::vector<UINT32>;
using GuideEventMap = std::map<EGuideEventType, GuideList>;
struct MMOGuideData {
    GuideDataMap _GuideDataMap;
    GuideEventMap _ActivateGuideEventMap, _DeactiveGuideEventMap;
    UBOOL LoadGuideDataFromUpk();
    void AddGuideData(const GuideData&);
};
''')
    parts.append(function(data, "UBOOL MMOGuideData::LoadGuideDataFromUpk()"))
    parts.append(function(data, "void MMOGuideData::AddGuideData("))
    parts.append(r'''
struct AMMOActor {
    INT class_id = 0;
    INT GetClassId() { return class_id; }
};
struct MMOFacade {
    AMMOActor* actor = nullptr;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    AMMOActor* GetMainActor() { return actor; }
};
enum EEventParm { EC_EventParm };
enum { AS_String };
struct FASValue { INT Type = -1; FString S; explicit FASValue(EEventParm) {} };
FString SelectPicture(const GuideData& data) {
    TArray<FASValue> args;
''')
    attach = function(clean("MMOGuideManager.cpp"), "void MMOGuideManager::_AttachGuidePanel(")
    start = attach.index("FASValue pictureName(EC_EventParm);")
    end = attach.index("args.AddItem(pictureName);", start) + len("args.AddItem(pictureName);")
    parts.append(attach[start:end])
    parts.append(r'''
    assert(args.Num() == 1 && args(0).Type == AS_String);
    return args(0).S;
}
void Run() {
    Resource picture{"picture"}, sound{"sound"}, icon{"icon"};
    MMOGuideData manager;
    for (INT count : {0, 1, MAX_TARGET_COMPONENT_COUNT - 1, int(MAX_TARGET_COMPONENT_COUNT), MAX_TARGET_COMPONENT_COUNT + 3}) {
        UGuide guide, disabled, invalid;
        disabled.Enable = FALSE; invalid.template_id = 0;
        guide.GuideSound = &sound; guide.GuideSoundIcon = &icon;
        for (INT i = 0; i < count; ++i) {
            guide.TargetComponent.AddItem({FString("component") + std::to_string(i)});
            guide.ItemReqNew.AddItem({100 + i, 0}); guide.SkillReq.AddItem({0, 200 + i});
            guide.Picture.AddItem({i % 2 ? nullptr : &picture});
        }
        package.guides = {nullptr, &disabled, &invalid, &guide, &guide}; package.full = false;
        assert(manager.LoadGuideDataFromUpk() && package.full && manager._GuideDataMap.size() == 1);
        const GuideData& found = manager._GuideDataMap.at(1);
        assert(found.Sound == "sound" && found.SoundIcon == "icon");
        for (INT i = 0; i < MAX_TARGET_COMPONENT_COUNT; ++i) {
            assert(found.TargetComponentName[i] == (i < count ? FString("component") + std::to_string(i) : ""));
        }
        for (INT i = 0; i < MAX_PLAYER_CLASS_COUNT; ++i) {
            assert(found.ItemReq[i] == (i < count ? 100 + i : 0));
            assert(found.SkillReq[i] == (i < count ? 200 + i : 0));
            assert(found.Picture[i] == (i < count && i % 2 == 0 ? "picture" : ""));
        }
    }
    load_ok = false;
    assert(manager.LoadGuideDataFromUpk() && manager._GuideDataMap.empty());
    load_ok = true; package.guides.clear();
    assert(manager.LoadGuideDataFromUpk() && manager._GuideDataMap.empty());
    GuideData data;
    for (INT i = 0; i < MAX_PLAYER_CLASS_COUNT; ++i) data.Picture[i] = FString("picture") + std::to_string(i);
    assert(SelectPicture(data).empty());
    AMMOActor actor; MMOFacade::getSingleton().actor = &actor;
    for (INT index : {(-2147483647 - 1), -1, 0, MAX_PLAYER_CLASS_COUNT - 1, int(MAX_PLAYER_CLASS_COUNT), 2147483647}) {
        actor.class_id = index;
        FString selected = SelectPicture(data);
        assert(selected == (index >= 0 && index < MAX_PLAYER_CLASS_COUNT ? FString("img://picture") + std::to_string(index) : ""));
    }
    MMOFacade::getSingleton().actor = nullptr;
    puts("Guide package imports bound short/oversized component lists; picture selection handles missing actors and invalid class indices");
}
}
#pragma pop_macro("debugIDf")
''')
    return "\n".join(parts)
