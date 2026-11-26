"""Production dialogue audio indexing and editor avatar texture fallback."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    manager = clean(source(game / "MMOQuestManager.cpp"))
    quest = clean(source(game / "Src/Quest.cpp"))
    capacity = re.search(r"#define MAX_PAGENUMBER\s+(\d+)", source(game / "MMOQuestManager.h")).group(1)
    parts = [r'''
namespace QuestAvatarTests {
using FString = AttributeRecordTests::FString;
template<class T> struct TArray : SkillQuestTests::TArray<T> {
    void Remove(INT start, INT count) {
        assert(start >= 0 && count >= 0 && start + count <= this->Num());
        this->data.erase(this->data.begin() + start, this->data.begin() + start + count);
    }
};
struct USoundCue {} sound;
int loads = 0;
template<class T> T* LoadUEObject(const FString& path, const char*) {
    ++loads; return path.empty() ? nullptr : &sound;
}
struct UQuest {
    struct VoicePage { FString PageSoundPath; USoundCue* PageSound = nullptr; };
    TArray<VoicePage> QuestPageSoundAccept, QuestPageSoundSubmit;
    int GetTemplateID() const { return 123; }
    USoundCue* GetQuestPageSoundAccept(int);
    USoundCue* GetQuestPageSoundSubmit(int);
} quest;
struct AMMOPlayer {
    bool IsInMatinee = false;
    int plays = 0;
    void PlaySound(USoundCue* cue) { assert(cue == &sound); ++plays; }
} player;
struct MMOFacade {
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    bool available = true;
    int stops = 0;
    AMMOPlayer* GetMainPlayer() { return available ? &player : nullptr; }
    AMMOPlayer* GetMainActor() { return GetMainPlayer(); }
    void StopAllVoiceSound(BOOL, double, double) { ++stops; }
};
struct MMOQuestData {
    static MMOQuestData& getSingleton() { static MMOQuestData value; return value; }
    bool found = true, available = true, accepted = false, compassable = false;
    const UQuest* GetQuestData(INT) { return found ? &quest : nullptr; }
    bool IsQuestAvailable(INT) { return available; }
    bool IsQuestAccepted(INT) { return accepted; }
    bool IsQuestCompassable(INT) { return compassable; }
};
enum { INVALID_QUEST_ID = 0, EGUET_QuestPageDown = 123 };
struct MMOGameEvent_Guide { explicit MMOGameEvent_Guide(int) {} };
namespace MMOUI { int posts = 0; void PostGameEvent(MMOGameEvent_Guide*) { ++posts; } }
struct MMOQuestManager {
    int _CurPage = 0, _CurPageQuestId = 123;
    void SetQuestDialogCurrentPage(INT);
};
''', "enum { MAX_PAGENUMBER = " + capacity + " };",
        function(manager, "void MMOQuestManager::SetQuestDialogCurrentPage("),
        function(quest, "USoundCue* UQuest::GetQuestPageSoundAccept("),
        function(quest, "USoundCue* UQuest::GetQuestPageSoundSubmit("), r'''
void QuestTests() {
    auto& data = MMOQuestData::getSingleton(); auto& facade = MMOFacade::getSingleton();
    MMOQuestManager manager;
    for (bool submitting : {false, true}) {
        data.available = !submitting; data.accepted = data.compassable = submitting;
        for (int count = 0; count <= MAX_PAGENUMBER; ++count) {
            for (int page = 0; page < MAX_PAGENUMBER; ++page) {
                quest.QuestPageSoundAccept.data.assign(count, UQuest::VoicePage());
                quest.QuestPageSoundSubmit.data.assign(count, UQuest::VoicePage());
                for (auto& entry : quest.QuestPageSoundAccept.data) entry.PageSoundPath = "voice";
                for (auto& entry : quest.QuestPageSoundSubmit.data) entry.PageSoundPath = "voice";
                int before = player.plays, oldLoads = loads;
                manager.SetQuestDialogCurrentPage(page);
                assert(manager._CurPage == page);
                assert(player.plays == before + (page < count));
                assert(loads == oldLoads + (page < count));
                manager.SetQuestDialogCurrentPage(page);
                assert(player.plays == before + 2 * (page < count));
                assert(loads == oldLoads + (page < count));
            }
        }
    }
    for (int invalid : {-2147483647 - 1, -1, int(MAX_PAGENUMBER), 2147483647}) {
        int oldPage = manager._CurPage, posts = MMOUI::posts;
        manager.SetQuestDialogCurrentPage(invalid);
        assert(manager._CurPage == oldPage && posts == MMOUI::posts);
        assert(!quest.GetQuestPageSoundAccept(invalid) && !quest.GetQuestPageSoundSubmit(invalid));
    }
    int before = player.plays;
    player.IsInMatinee = true; manager.SetQuestDialogCurrentPage(0); player.IsInMatinee = false;
    facade.available = false; manager.SetQuestDialogCurrentPage(0); facade.available = true;
    data.found = false; manager.SetQuestDialogCurrentPage(0); data.found = true;
    manager._CurPageQuestId = INVALID_QUEST_ID; manager.SetQuestDialogCurrentPage(0);
    assert(before == player.plays);
    puts("Quest pages with shorter accept/submit sound lists, cached audio and invalid pages passed");
}
bool GIsEditor = true;
void debugf(const char*, ...) {}
enum EEventParm { EC_EventParm };
using FLOAT = float;
struct USkeletalMesh {} mesh;
struct UTexture2D { int id; } texture0{1}, texture1{2};
struct USkeletalMeshComponent { void SetSkeletalMesh(USkeletalMesh*) {} } base, merged;
struct UEquipMesh {
    BYTE MeshType = 0;
    USkeletalMesh* SkeletalMesh = &mesh;
    INT UsedColorTextureID = 0;
    TArray<UTexture2D*> ColorTextures;
    bool bNeedSkinTexture = true;
    UTexture2D* RegionTexture = nullptr;
    FString GetName() const { return "equip"; }
};
struct FBodyPart {
    USkeletalMesh* Mesh = nullptr;
    int OrigX = 0, OrigY = 0, SizeX = 0, SizeY = 0;
    TArray<UTexture2D*> Textures, Textures_Base;
    explicit FBodyPart(EEventParm) {}
};
struct UMMOSkeletalMeshMerge {
    int TargetTexSizeX = 0, TargetTexSizeY = 0;
    TArray<FBodyPart> Parts;
    void MarkPendingKill() {}
    static int StaticClass() { return 1; }
    bool PrepareTextures() { return true; }
} mergeObject;
UMMOSkeletalMeshMerge* StaticConstructObject(int) { mergeObject.Parts.Empty(); return &mergeObject; }
template<class T> T* Cast(UMMOSkeletalMeshMerge* object) { return object; }
struct UAvatarData {
    bool UseMergedMesh = false, MergeShoulder = false, bIsPreparing = false, bIsMerging = false;
    INT TestSkinColorIdx = 0;
    USkeletalMeshComponent* BaseComponent = nullptr;
    USkeletalMeshComponent* MergedComp = nullptr;
    UMMOSkeletalMeshMerge* MergedMesh = nullptr;
    TArray<UEquipMesh*> DefaultMeshListTemplate;
    TArray<UTexture2D*> SkinTextures, SkinTexturesBase;
    void RebulidMesh(USkeletalMeshComponent*, USkeletalMeshComponent*);
};
''']
    generated = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    parts.append(function(generated, "enum EBodyMeshType\n") + ";")
    parts.append(function(clean(source(game / "Src/AvatarData.cpp")), "void UAvatarData::RebulidMesh("))
    parts.append(r'''
void AvatarTests() {
    UEquipMesh parts[6];
    UAvatarData avatar;
    for (int i = 0; i < 6; ++i) {
        parts[i].MeshType = i;
        parts[i].ColorTextures.data = {&texture0, &texture1};
        avatar.DefaultMeshListTemplate.AddItem(&parts[i]);
    }
    for (INT index : {-2147483647 - 1, -1, 0, 1, 2, 2147483647}) {
        avatar.TestSkinColorIdx = index;
        avatar.RebulidMesh(&base, &merged);
        assert(avatar.bIsPreparing && !avatar.bIsMerging);
        for (int i = 0; i < EBMT_Waist; ++i) {
            assert(avatar.SkinTextures(i) == (index == 1 ? &texture1 : &texture0));
            assert(avatar.SkinTexturesBase(i) == &texture0);
        }
    }
    for (auto& part : parts) part.ColorTextures.Empty();
    avatar.TestSkinColorIdx = -1; avatar.RebulidMesh(&base, &merged);
    for (int i = 0; i < EBMT_Waist; ++i) assert(!avatar.SkinTextures(i) && !avatar.SkinTexturesBase(i));
    puts("Avatar texture negative/large indices, valid colors and empty-list fallback passed");
}
}
''')
    return "\n".join(parts)
