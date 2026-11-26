"""Build constructor regressions from the real client declarations and bodies."""
import re


def harness(game, read, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", read(path))

    def definition(file, name, kind="struct"):
        text = clean(game / file)
        match = re.search(r"\b" + kind + r"\s+" + name + r"\b[^;{]*\{", text)
        if not match:
            raise ValueError("Missing declaration: " + name)
        return function(text[match.start():], kind + " " + name) + ";"

    parts = [r'''
#include <new>
#include <utility>
#include <map>
#include <ctime>
using FLOAT = float;
struct AMMODoodadActor;
struct UDoodad;
struct UAbilityComponent;
struct UGFxObject;
struct UMMOGFXPlayer;
struct UItem;
struct FVector {
    FLOAT X, Y, Z;
    FVector() {}
    FVector(FLOAT x, FLOAT y, FLOAT z) : X(x), Y(y), Z(z) {}
};
struct FVector2D {
    FLOAT X, Y;
    FVector2D() {}
    FVector2D(FLOAT x, FLOAT y) : X(x), Y(y) {}
    FVector2D operator*(FLOAT scale) const { return FVector2D(X * scale, Y * scale); }
};
struct FIntPoint {
    INT X, Y;
    FIntPoint() {}
    FIntPoint(INT x, INT y) : X(x), Y(y) {}
};
enum { MAX_TEAM_MEMBER_COUNT = 20, GUILD_B_NUM = 25, EACTUIUF_NoUpdate = 0 };
''', definition("MMOGameEvent.h", "EGameEvent", "enum"),
        definition("MMOGameEvent.h", "MMOGameEvent"),
        definition("MailManager.h", "MailType", "enum"),
        definition("MasterStudentManager.h", "EMasterStudentType", "enum"),
        function(clean(game / "MMOAchievementData.h"), "enum\n{") + ";"]
    for file, names in [
        ("DoodadManager.h", ["MMOGameEvent_Doodad", "FDoodadDataHolder"]),
        ("MailManager.h", ["MMOGameEvent_Mail"]),
        ("MasterStudentManager.h", ["Teacher_RewardExp", "MasterDetailInfo", "MMOGameEvent_MasterStudent"]),
        ("MMOBossBloodManager.h", ["MMOGameEvent_BossBlood"]),
        ("MMOKillRecordManager.h", ["MMOGameEvent_KillRecord"]),
        ("MMOGuildManager.h", ["MMOGameEvent_Guild"]),
        ("Src/MMOHeavenManager.h", ["MMOGameEvent_Heaven"]),
        ("CommonGameEvents.h", ["MMOGameEvent_OtherPlayerLevelUp"]),
        ("ActivityManager.h", ["ActivityTeam"]),
        ("MMOEquipManager.h", ["SoulInfo"]),
        ("MMOAchievementData.h", ["AchievementData", "KillNpcAchievementData", "GuildAchievementData", "GuildConditionData"]),
        ("MMOGuildData.h", ["GuildViewInfo", "GuildFullMemberInfo", "GuildPendingMemberInfo",
                            "GuildForeignInfo", "GuildApplyAllyInfo", "GuildWareAccessInfo"]),
        ("Src/MMOArenaManager.h", ["ArenaWinPrize"]),
        ("MMOMapDataManager.h", ["FObjInMapUI", "FMapMark", "FActorInMapUI"]),
    ]:
        parts.extend(definition(file, name) for name in names)
    parts.append(function(clean(game / "MMOEquipManager.cpp"), "SoulInfo::SoulInfo()"))
    for name in ["FObjInMapUI", "FMapMark", "FActorInMapUI"]:
        parts.append(function(clean(game / "MMOMapDataManager.cpp"), name + "::" + name + "()"))

    # Keep the actual BaseComponent fields and constructor; its engine methods
    # need the separately distributed UE3 runtime and are irrelevant here.
    ability = clean(game / "AbilityProcessor.h")
    begin = ability.index("struct BaseComponent\n")
    end = ability.index("\n\tINT CreateNewId();", begin)
    parts.append(ability[begin:end] + "\n};")
    parts.append(r'''
using LONG64 = int64_t;
using FName = INT;
enum { NAME_None = 0, HIT_POINT_MAX_NUMBER = 6 };
enum EAbilityState { EAS_End };
using STLCastNodeVector = std::vector<void*>;
using STLIntVector = std::vector<INT>;
template<class T> struct TArray : std::vector<T> {
    INT Num() const { return (INT)this->size(); }
    T& operator()(INT index) { return this->at(index); }
    const T& operator()(INT index) const { return this->at(index); }
};
struct AbilityProcessor;
struct UMMOAbility;
struct FComponentNode;
struct HitNode;
struct EffectNode;
''')
    parts.append(definition("AbilityProcessor.h", "AbilityNode", "class"))

    # Use the actual revive record and enum as well as the manager constructor.
    common = game.parents[4] / "Common"
    char_defs = clean(common / "define/char_def.h")
    message_defs = clean(common / "message/g2c_message.h")
    parts.append(function(char_defs, "enum CHAR_REVIVE_TYPE") + ";")
    parts.append(function(message_defs, "struct REVIVE_INFO") + ";")
    parts.append(r'''
struct G2C_REVIVE_INFO;
template<class T> struct SingletonMeyers { virtual ~SingletonMeyers() {} };
''')
    parts.append(definition("MMODeathManager.h", "MMODeathManager", "class").replace("private:", "public:"))
    parts.append(function(clean(game / "MMODeathManager.cpp"), "MMODeathManager::MMODeathManager()"))
    parts.append(r'''
#define M_UE3_STL_ALLOC_MAP(key, value) std::less<key>
enum { EAT_FLAG_MAX = 16, DEFAULT_PRACTISE_TIME = 4 };
''')
    parts.append(definition("MMOMapImgTiles.h", "MapTilesType", "enum"))
    parts.append(definition("MMOMapImgTiles.h", "MMOMapImgTiles", "class").replace("private:", "public:"))
    tiles = clean(game / "Src/MMOMapImgTiles.cpp")
    parts.append(function(tiles, "MMOMapImgTiles::MMOMapImgTiles("))
    parts.append(function(tiles, "MMOMapImgTiles::~MMOMapImgTiles()"))
    parts.append(function(tiles, "bool MMOMapImgTiles::LoadUiTile("))
    parts.append(definition("Src/Mir2Attack.h", "AttackClassX", "class"))
    attack = clean(game / "Src/Mir2Attack.cpp")
    parts.append(function(attack, "AttackClassX::AttackClassX()"))
    parts.append(function(attack, "void AttackClassX::ClearAbilityFlag()"))
    parts.append(r'''
struct ProbeAttack : AttackClassX {
    UBOOL InPrimaryAbilityRange(FVector, UBOOL) override { return FALSE; }
    UMMOAbility* GetPrimaryAbility() override { return nullptr; }
    UBOOL DoAIAbility() override { return FALSE; }
    void Update(FLOAT) override {}
    void CheckDefaults() {
        assert(_pMainPlayer == nullptr && _fDistance == 0);
        assert(_fDistance_GridX == 0 && _fDistance_GridY == 0 && !_in8Dir);
        assert(_DesPos.X == 0 && _DesPos.Y == 0 && _DesPos.Z == 0);
        for (UBOOL flag : AbilityFlag) assert(!flag);
    }
};
''')
    # Isolate constructors whose remaining methods depend on engine services.
    mir2 = clean(game / "Src/Mir2Attack.h")
    begin = mir2.index("\tAttackClassX* _AttackClassX;")
    end = mir2.index("\n};", begin)
    parts.append("struct Mir2Attack { Mir2Attack();\n" + mir2[begin:end] + "\n};")
    parts.append(function(attack, "Mir2Attack::Mir2Attack()"))
    heaven = clean(game / "Src/MMOHeavenManager.h")
    begin = heaven.rfind("\n", 0, heaven.index("pkTargetGasSlot;"))
    end = heaven.index("public:", begin)
    declarations = re.findall(
        r"(?m)^\s*(?:GasSlot\s*\*|UINT32|INT32|INT|BOOL|time_t)\s+\w+(?:\[[^]]+\])?\s*;",
        heaven[begin:end])
    parts.append("struct GasSlot;\nenum { MAX_BLOODSEA_PAGE = 4 };\nstruct MMOHeavenManager {\n"
                 "MMOHeavenManager();\n" + "\n".join(declarations) + "\n};")
    parts.append(function(clean(game / "Src/MMOHeavenManager.cpp"), "MMOHeavenManager::MMOHeavenManager()"))
    parts.append(definition("Src/Shop.h", "ShopType", "enum"))
    parts.append(r'''
struct ServerShopData;
struct ShopManager {
    ShopManager();
    ServerShopData* _curShop;
    UINT32 _crc;
    UBOOL _hasLoadShop, _hasCheckShopCrc;
    ShopType _curShopType;
    BOOL bShowShop;
    INT _curShopId;
};
#define ShopItemCostTypeNum 2
''')
    parts.append(function(clean(game / "Src/Shop.cpp"), "ShopManager::ShopManager()"))
    parts.append(definition("Src/Shop.h", "ServerShopItemData"))
    parts.append(definition("Src/MMOChatManager.h", "ChatMessage"))
    parts.append(definition("Src/MMOChatManager.h", "ChatFilterChannelMessage"))
    parts.append("enum EEventParm { EC_EventParm };\n")
    parts.append(definition("Src/TeamManager.h", "ETeamAuctionItemState", "enum"))
    for file, names in [
        ("Src/TeamManager.h", ["MMOTeamMember", "MMONearbyPlayer", "MMOTeamAuctionItem",
                               "MMOInvitedPlayer", "MMOTeamInfo", "TeamAuctionItemColorLimitMap"]),
        ("Src/FriendManager.h", ["MMOFriendMember", "MMOFriendGroup", "MMOKillInfo"]),
    ]:
        parts.extend(definition(file, name) for name in names)

    # Exercise two complete manager constructors without calling Init().
    for name, file, declarations in [
        ("MMOHelperManager", "MMOHelperManager.cpp", "FLOAT _CurPlayerShowProgress; INT _CurPlayerShowTime;"),
        ("MMOLoginManager", "MMOLoginManager.cpp", "INT lastQueueTime; INT lastQueuePos;"),
    ]:
        parts.append("struct " + name + " { " + declarations + " " + name + "(); };")
        parts.append(function(clean(game / file), name + "::" + name + "()"))

    parts.append(definition("MMOGameEvent.h", "MMOGameEvent_Any"))
    parts.append(r'''
using TCHAR = wchar_t;
enum { UCONST_MaxChatChannelNum = 4 };
#define ChatDataFile "ChatOptionData"
namespace MMOUI { template<class T> void PostGameEvent(T*) {} }
struct ChatArchive {
    std::vector<char> data;
    size_t position = 0;
    bool error = false;
    bool IsError() const { return error; }
    void Serialize(void* destination, INT bytes) {
        if (bytes < 0 || (size_t)bytes > data.size() - position) {
            error = true;
            return;
        }
        if (bytes) memcpy(destination, data.data() + position, bytes);
        position += bytes;
    }
    template<class T> void Append(T value) {
        const size_t offset = data.size();
        data.resize(offset + sizeof(value));
        memcpy(data.data() + offset, &value, sizeof(value));
    }
    void Name(INT byteCount) {
        for (INT i = 0; i < byteCount / sizeof(TCHAR); ++i) Append<TCHAR>(L'A');
        if (byteCount > 0 && byteCount % sizeof(TCHAR)) Append<char>(0);
    }
};
using FArchive = ChatArchive;
struct MMOChatManager {
    struct Channel { INT index; std::string name; INT filter; BOOL isDefault; };
    std::vector<Channel> channels;
    INT defaultCalls = 0;
    void LoadRecord();
    void AddDefaultFilterChannel() { ++defaultCalls; }
    void SetFilterChannel(INT index, const FString& name, INT filter, BOOL isDefault) {
        channels.push_back({index, name.value, filter, isDefault});
    }
};
''')
    parts.append(function(clean(game / "Src/MMOChatManager.cpp"), "void MMOChatManager::LoadRecord()"))
    parts.append(r'''
void test_saved_chat_records() {
    for (INT length : {-1, 1, 3, 2048, 4096}) {
        ChatArchive archive;
        archive.Append<INT>(1);
        archive.Append<INT>(3);
        archive.Append<INT>(length);
        if (length > 0) archive.Name(length);
        archive.Append<UBOOL>(FALSE);
        g_chatArchive = &archive;
        MMOChatManager chat;
        chat.LoadRecord();
        assert(chat.channels.empty());
    }
    for (INT length : {0, 4, 2046}) {
        ChatArchive archive;
        archive.Append<INT>(1);
        archive.Append<INT>(3);
        archive.Append<INT>(length);
        archive.Name(length);
        archive.Append<UBOOL>(TRUE);
        g_chatArchive = &archive;
        MMOChatManager chat;
        chat.LoadRecord();
        assert(chat.channels.size() == 1 && !archive.IsError());
        assert(chat.channels[0].name.size() == length / sizeof(TCHAR));
        assert(chat.channels[0].filter == 3 && chat.channels[0].isDefault);
    }
    for (INT count : {-1, UCONST_MaxChatChannelNum + 1}) {
        ChatArchive archive;
        archive.Append<INT>(count);
        g_chatArchive = &archive;
        MMOChatManager chat;
        chat.LoadRecord();
        assert(chat.channels.empty() && chat.defaultCalls == 1);
    }
    ChatArchive truncated;
    truncated.Append<INT>(1);
    truncated.Append<INT>(3);
    truncated.Append<INT>(4);
    truncated.Name(4);
    g_chatArchive = &truncated;
    MMOChatManager chat;
    chat.LoadRecord();
    assert(chat.channels.empty() && truncated.IsError());
    g_chatArchive = nullptr;
    puts("Saved chat record boundary regressions passed");
}
''')

    parts.append(r'''
template<class T, class Check, class... Args>
void check_constructed(Check check, Args&&... args) {
    // A zero-filled static allocation would hide the original missing writes.
    for (int pattern : {0xA5, 0x5A}) {
        alignas(T) unsigned char storage[sizeof(T)];
        memset(storage, pattern, sizeof(storage));
        T* object = new (storage) T(std::forward<Args>(args)...);
        check(*object);
        object->~T();
    }
}
void test_member_initialization() {
    test_saved_chat_records();
    check_constructed<AchievementData>([](AchievementData& data) {
        assert(!data.Id && !data.BaseClass && !data.SubClass && !data.DetailType && !data.Point);
        assert(!data.Hide && !data.AutoTrace && !data.MinTraceLevel && !data.MaxTraceLevel);
        assert(!data.RewardPoint && !data.RewardTitle);
        for (INT value : data.ConditionType) assert(!value);
        for (INT value : data.ConditionValue1) assert(!value);
        for (INT value : data.ConditionValue2) assert(!value);
        for (INT value : data.ConditionValue3) assert(!value);
        for (UINT32 value : data.RewardType) assert(!value);
        for (UINT32 value : data.RewardValue) assert(!value);
        for (UINT32 value : data.PreAchievement) assert(!value);
        for (const auto& value : data.ConditionDesc) assert(value.value.empty());
    });
    check_constructed<KillNpcAchievementData>([](KillNpcAchievementData& data) {
        assert(!data.KillId && !data.NpcTemplateId && !data.NpcGroupId && !data.BaseClass && !data.SubClass);
        for (INT value : data.KillCount) assert(!value);
        for (UINT32 value : data.Point) assert(!value);
        for (UINT32 value : data.RewardExp) assert(!value);
        for (UBOOL value : data.Hide) assert(!value);
        for (UBOOL value : data.AutoTrace) assert(!value);
        for (UINT32 value : data.MinTraceLevel) assert(!value);
        for (UINT32 value : data.MaxTraceLevel) assert(!value);
    });
    check_constructed<GuildAchievementData>([](GuildAchievementData& data) {
        assert(!data.Id && !data.BaseClass && !data.SubClass && !data.Hide);
        for (UINT16 value : data.ConditionType) assert(!value);
        for (UINT16 value : data.ConditionParam) assert(!value);
        for (UINT32 value : data.RewardType) assert(!value);
        for (UINT32 value : data.RewardTemp) assert(!value);
        for (UINT32 value : data.RewardCount) assert(!value);
    });
    check_constructed<GuildConditionData>([](GuildConditionData& data) {
        assert(!data.ConditionType && !data.ConditionParam && !data.ConditionPos);
    });
    check_constructed<GuildFullMemberInfo>([](GuildFullMemberInfo& member) {
        assert(!member.Id && !member.Post && !member.Level && !member.Job && !member.SceneID);
        assert(!member.LastLoginTime && !member.IsForbitChat && !member.Salary);
    });
    check_constructed<GuildPendingMemberInfo>([](GuildPendingMemberInfo& member) {
        assert(!member.Id && !member.Level && !member.Job && !member.ApplyTime);
    });
    check_constructed<GuildForeignInfo>([](GuildForeignInfo& info) {
        assert(!info.Id && !info.Level && !info.MemberCount && !info.LeftTime);
    });
    check_constructed<GuildApplyAllyInfo>([](GuildApplyAllyInfo& info) {
        assert(!info.Id && !info.Level && !info.MemberCount && !info.LeftTime);
    });
    check_constructed<GuildWareAccessInfo>([](GuildWareAccessInfo& info) {
        assert(!info.RoleId && !info.ItemTempId && !info.Count && !info.Time && !info.Type);
    });
    check_constructed<MMOTeamMember>([](MMOTeamMember& member) {
        assert(!member.ActorServerId && !member.Lv && !member.ClassId && !member.isLeader);
        assert(!member.HPPecent && !member.MPPecent && !member.Sex && !member.Online);
        assert(!member.UIPos && !member.HP && !member.MapID && !member.MapLineID);
        assert(!member.WorldPos.X && !member.WorldPos.Y && !member.WorldPos.Z);
        assert(member.ActorName.value.empty());
        member.ActorName = "valid string";
    });
    check_constructed<MMONearbyPlayer>([](MMONearbyPlayer& member) {
        assert(!member.ActorServerId && !member.Lv && !member.ClassId && !member.Sex && !member.InTeam);
    });
    check_constructed<MMOTeamAuctionItem>([](MMOTeamAuctionItem& item) {
        assert(item.BidState == ETAIS_NoPermit && !item.RewardMemberCount && !item.Index);
    });
    check_constructed<MMOInvitedPlayer>([](MMOInvitedPlayer& member) {
        assert(!member.ActorServerId && !member.Job && !member.Level);
        assert(member.ActorName.value.empty());
        member.ActorName = "valid string";
    });
    check_constructed<MMOTeamInfo>([](MMOTeamInfo& team) {
        assert(!team.MemberCount && !team.MaxMemberCount && !team.LeadLv && !team.TeamID && !team.TeamLeaderServeId);
    });
    check_constructed<TeamAuctionItemColorLimitMap>([](TeamAuctionItemColorLimitMap& limit) {
        assert(!limit.Color);
    });
    for (bool event : {false, true}) {
        auto check = [event](MMOFriendMember& member) {
            assert(!member.RoleId && !member.Lv && !member.ClassId && !member.Sex && !member.Online && !member.nIntimacy);
            assert(member.SceneTempID == (event ? 0 : (UINT8)-1));
            assert(member.ActorName.value.empty() && member.Sign.value.empty() && member.Location.value.empty());
            member.ActorName = "valid string";
        };
        if (event) check_constructed<MMOFriendMember>(check, EC_EventParm);
        else check_constructed<MMOFriendMember>(check);
    }
    check_constructed<MMOFriendGroup>([](MMOFriendGroup& group) { assert(!group.IsDefault); });
    check_constructed<MMOKillInfo>([](MMOKillInfo& info) { assert(!info.Time && !info.RoleId && !info.Killed); });
    check_constructed<MMOGameEvent_Doodad>([](MMOGameEvent_Doodad& event) {
        assert(event.DoodadActor == nullptr && event.OldOperateCount == 7);
    }, nullptr, (BYTE)7);
    check_constructed<MMOGameEvent_Mail>([](MMOGameEvent_Mail& event) {
        assert(event.eMailType == UpdateMailList && event._MailCount == 4);
        assert(!event.selectValue);
    }, UpdateMailList, 4);
    check_constructed<MMOGameEvent_MasterStudent>([](MMOGameEvent_MasterStudent& event) {
        assert(event.EventType == EMST_MasterInfo && event.ID == 42);
        assert(event.level == 0 && event.masterExp == 0);
    }, EMST_MasterInfo, 42u);
    check_constructed<MMOGameEvent_BossBlood>([](MMOGameEvent_BossBlood& event) {
        assert(event.nCurHp == 0 && event.nMaxHp == 1 && event.nLevel == 1);
        assert(event.byteQuality == 0);
    }, MMOGameEvent_BossBlood::EBossBlood_Event_Appear);
    check_constructed<MMOGameEvent_KillRecord>([](MMOGameEvent_KillRecord& event) {
        assert(event.nCount == 0 && event.nExp == 0 && !event.bNew);
    }, MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterCount);
    check_constructed<MMOGameEvent_Guild>([](MMOGameEvent_Guild& event) {
        assert(event._CurPost == 0 && event._BuildingId == 0 && event._WelfareId == 0);
        assert(event._ViewType == 0 && event._ViewSize == 0);
    }, MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
    check_constructed<MMOGameEvent_Heaven>([](MMOGameEvent_Heaven& event) {
        assert(event.SlotID == 0 && event.SlotContainerID == 0 && event.nValue == 0);
    }, MMOGameEvent_Heaven::EHeavenEvent_SynData);
    check_constructed<MMOGameEvent_OtherPlayerLevelUp>([](MMOGameEvent_OtherPlayerLevelUp& event) {
        assert(event.roleID == 0 && event.lv == 0);
    });
    check_constructed<MasterDetailInfo>([](MasterDetailInfo& info) {
        assert(info.nRoleID == 0 && info.nLevel == 0 && info.nClass == 0);
        assert(info.nCZLevel == 0 && info.nStudentNum == 0 && info.reward.nRoleID == 0);
    });
    check_constructed<FDoodadDataHolder>([](FDoodadDataHolder& info) {
        assert(info.DoodadID == 0 && info.DoodadTemplate == nullptr);
    });
    check_constructed<ActivityTeam>([](ActivityTeam& team) {
        assert(team.TeamId == 0 && team.PlayerCount == 0);
        for (INT id : team.MemberId) assert(id == 0);
        team.MemberId[MAX_TEAM_MEMBER_COUNT - 1] = 42;
        team.Clear();
        for (INT id : team.MemberId) assert(id == 0);
    });
    check_constructed<SoulInfo>([](SoulInfo& info) {
        assert(info._ID == -1 && info._ItemID == -1 && info._AttrID == -1);
    });
    check_constructed<GuildViewInfo>([](GuildViewInfo& info) {
        for (UINT8 level : info.BuildingLevel) assert(level == 0);
    });
    check_constructed<ArenaWinPrize>([](ArenaWinPrize& prize) {
        assert(prize._MoneyToAward == 0 && prize._MaxPlayerGetNum == 0 && prize._PerPlayerPect == 0);
    });
    check_constructed<BaseComponent>([](BaseComponent& component) {
        assert(component.ComponentId == 0 && component.ComponentType == 0);
        assert(component.ComponentData == nullptr && component.CreatorId == 0);
        assert(component.TargetPoint.X == 0 && component.TargetPoint.Y == 0 && component.TargetPoint.Z == 0);
        assert(component.bStopped && component.Rate == 1 && component.WeaponHitType == -1);
    });
    check_constructed<FMapMark>([](FMapMark& mark) {
        assert(mark._Pos.X == 0 && mark._Pos.Y == 0 && mark._Pos.Z == 0);
        assert(mark._MarkID == -1 && !mark._isValid);
    });
    check_constructed<FActorInMapUI>([](FActorInMapUI& actor) {
        assert(actor._OrigInMapPos.X == 0 && actor._OrigInMapPos.Y == 0 && actor._OrigInMapPos.Z == 0);
        assert(actor._CurMapPos2D.X == 0 && actor._CurMapPos2D.Y == 0);
    });
    check_constructed<MMOHelperManager>([](MMOHelperManager& manager) {
        assert(manager._CurPlayerShowProgress == 0 && manager._CurPlayerShowTime == 0);
    });
    check_constructed<MMOLoginManager>([](MMOLoginManager& manager) {
        assert(manager.lastQueueTime == 0 && manager.lastQueuePos == 0);
    });
    check_constructed<AbilityNode>([](AbilityNode& ability) {
        assert(ability.TargetPoint.X == 0 && ability.TargetPoint.Y == 0 && ability.TargetPoint.Z == 0);
        for (HitNode* node : ability.PrepareEffectList) assert(node == nullptr);
        assert(ability.Rate == 1 && ability.State == EAS_End);
    });
    check_constructed<MMODeathManager>([](MMODeathManager& manager) {
        assert(manager.GetWaitTime() == 0 && manager.GetReviveTime() == 0);
        assert(!manager.GetPlayerKilled() && manager.KillerID == 0);
        for (const auto& revive : manager.ReviveInfo) {
            assert(revive.byReviveType == 0 && revive.nRequireItemID == 0 && revive.nRequireItemCount == 0);
        }
    });
    check_constructed<MMOMapImgTiles>([](MMOMapImgTiles& tiles) {
        assert(!tiles.isReady() && tiles._InterTempTile && tiles._ShowTile);
        assert(tiles._Map3Dto2D.X == 0 && tiles._Map3Dto2D.Y == 0);
        assert(tiles._MapOffset.X == 0 && tiles._MapOffset.Y == 0);
        assert(tiles._MapOffset2.X == 0 && tiles._MapOffset2.Y == 0);
        assert(tiles._MapTileSize.X == 0 && tiles._MapTileSize.Y == 0);
        assert(tiles._MainTilePos.X == 0 && tiles._MainTilePos.Y == 0);
        assert(tiles._LastTileIdx.X == 0 && tiles._LastTileIdx.Y == 0);
        assert(tiles._MapTileImgIdx.X == 0 && tiles._MapTileImgIdx.Y == 0);
    }, (UINT16)2, (UINT16)3);
    check_constructed<ProbeAttack>([](ProbeAttack& attack) { attack.CheckDefaults(); });
    check_constructed<Mir2Attack>([](Mir2Attack& attack) {
        assert(!attack.m_bPracticeOpen && attack.m_PracticeTime == 0 && attack.m_PracticeAbilityID == 0);
        assert(attack.m_TryToMinePos.X == 0 && attack.m_TryToMinePos.Y == 0 && attack.m_TryToMinePos.Z == 0);
    });
    check_constructed<MMOHeavenManager>([](MMOHeavenManager& manager) {
        assert(!manager.pkTargetGasSlot && manager.dwLastOfflineTime == 0 && manager.dwLastExp2PtsTime == 0);
        assert(manager.nDailyExp2PtsTime == 0 && manager.nCostMoneyAll == 0 && manager.nCurHeavenExp == 0);
        assert(manager.tNowTime == 0 && manager.nNowNeedTime == 0 && manager.gasPractiseState == 1);
        for (INT state : manager.nBloodState) assert(state == 0);
        assert(manager.curMaxID == 0 && manager.nHeavenLevel == 0);
        assert(manager.bSoulLimited && manager.bLevelLimited && manager.bServerWaiting);
        assert(manager.nCurConvertType == 0 && manager.nGasOperateType == 0 && manager.nTempGasOperateType == 0);
    });
    check_constructed<ShopManager>([](ShopManager& manager) {
        assert(manager._curShopId == 0 && !manager._curShop && !manager.bShowShop);
    });
    check_constructed<ServerShopItemData>([](ServerShopItemData& item) {
        assert(!item.ShopItem && item.ShopSlot == 0 && item.RequireForceID == 0 && item.Reputation == 0);
        assert(item.RequireGuildBuildingID == 0 && item.BuildingLevel == 0 && item.ItemCount == 0);
        assert(item.LimitType == 0 && item.MaxCoolDownTime == 0 && item.CurrentCoolDown == 0 && item.ShopTypeSell == 0);
        for (INT value : item.CostType) assert(value == 0);
        for (INT value : item.CostValue) assert(value == 0);
    });
    check_constructed<ChatFilterChannelMessage>([](ChatFilterChannelMessage& channel) {
        assert(channel.ChannelFilterType == 0 && !channel.IsDefaultChannel);
    });
    puts("Constructor initialization regressions passed (46 types, two memory patterns)");
}
''')
    return "\n".join(parts)
