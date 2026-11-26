"""Focused regressions; run with Python 3 on Windows with MSVC installed.

The native harness compiles the actual affected functions in isolation with
AddressSanitizer, without requiring the separately distributed UE3 assets.
"""
import ctypes
import importlib.util
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest


CLIENT = Path(__file__).resolve().parents[1]
GAME = CLIENT / "client/UE3/Development/Src/MMOGame"
sys.dont_write_bytecode = True


def source(path):
    return path.read_text(encoding="latin1").removeprefix("\xef\xbb\xbf")


def function(text, signature):
    start = text.index(signature)
    opening = text.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


def native_harness():
    handler = source(GAME / "net/message_handler.cpp")
    facade = source(GAME / "MMOFacade.h")
    # Retain the real fields, constructor and bounded reader. UI-only methods
    # require the engine, so exclude them from this standalone translation unit.
    item = facade[facade.index("struct ItemInfo"):facade.index("    UINT8 GetSocketType")]
    item += facade[facade.index("    template<typename T>"):facade.index("\n\tINT GetPotential")]
    item += "\n};\n"
    preamble = r'''
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>
using UINT8 = uint8_t; using INT8 = int8_t;
using UINT16 = uint16_t; using UINT32 = uint32_t; using UINT64 = uint64_t;
using INT32 = int32_t; using INT64 = int64_t; using BOOL = int;
using INT = int; using UINT = unsigned int;
using UBOOL = int;
using BYTE = unsigned char; using WORD = uint16_t;
using WCHAR = wchar_t; using ACHAR = char; using NCHAR = char;
#define TRUE 1
#define FALSE 0
#define LOG_PROCESS_ERROR(c) do { if (!(c)) goto Exit0; } while (0)
#define TEXT(s) s
#define warnf(...) ((void)0)
#define appMemzero(p, n) memset(p, 0, n)
#define appMemcpy(p, q, n) memcpy(p, q, n)
#define MAX_RANDOM_ATTR_COUNT 4
#define MAX_RUNE_COUNT 6
#define MAX_SOCKET_COUNT 3
namespace ExternalMessage {
enum { max_message_count = 1024, flex_message_size = -1 };
#pragma pack(push, 1)
struct MESSAGE_HEADER { UINT16 wMsg; };
struct FLEX_MESSAGE_HEADER : MESSAGE_HEADER { UINT16 wSize; };
#pragma pack(pop)
}
using namespace ExternalMessage;
INT32 g_nS2CMessageSize[max_message_count] = {};
class CSocketSession {
public:
    UINT16 m_wRecvMsgSize = 0;
    char m_szTempMsg[64 * 1024 + sizeof(FLEX_MESSAGE_HEADER)] = {};
    INT32 GetNeedRecvSize();
};
struct FStringA {
    std::string value;
    FStringA() {}
    FStringA(const char* s) : value(s) {}
    FStringA(const wchar_t* s) { while (*s) value += (char)*s++; }
    FStringA& operator=(const char* s) { value = s; return *this; }
    operator const char*() const { return value.c_str(); }
    const char* GetString() const { return value.c_str(); }
    size_t GetLength() const { return value.size(); }
    void Empty() { value.clear(); }
    void Append(const char* s, size_t n) { value.append(s, n); }
};
struct FStringW {
    std::wstring value;
    const wchar_t* GetString() const { return value.c_str(); }
    size_t GetLength() const { return value.size(); }
    void Empty() { value.clear(); }
    void Append(const wchar_t* s, size_t n) { value.append(s, n); }
};
using FString = FStringA;
#define FDECLARE_CLASS(x)
#define FINLINE inline
#define _FT(s) s
#define _FTA(s) s
#define _FTW(s) L##s
bool fail_allocation = false;
void* checked_calloc(size_t count, size_t size) {
    return fail_allocation ? nullptr : std::calloc(count, size);
}
#define calloc checked_calloc
'''
    pieces = [preamble, source(CLIENT / "client/DevTools/ReplayView/FMod_MemFile.h"),
              "\n#undef calloc\n", item,
              function(handler, "size_t strxcpy(char *dst, const char *src"),
              function(handler, "size_t strxcat("),
              function(handler, "static BOOL IsValidReceivedMessage("),
              function(source(GAME / "net/simple_net.cpp"), "INT32 CSocketSession::GetNeedRecvSize()")]
    jigsaw = source(GAME / "MMOJigsawManager.cpp")
    pieces.append('''
enum { JIGSAW_ANSWER_COUNT = 3 };
class MMOJigsawManager {
public:
    BOOL isBuildingTexture = FALSE;
    char _TargetTextureData[16] = {};
    char _AnswerTextureData[JIGSAW_ANSWER_COUNT][8] = {};
    void CopyTargetTextureData(char*, INT);
    void CopyAnswerTextureData(char*, INT, INT);
};
''')
    pieces.extend(function(jigsaw, signature) for signature in [
        "void MMOJigsawManager::CopyTargetTextureData(",
        "void MMOJigsawManager::CopyAnswerTextureData("])
    # Exercise the actual map allocation/serialization block with a fake archive.
    marks = source(GAME / "MMOMapDataManager.cpp")
    start = marks.index("\t\tINT TotalSize = pArchive->TotalSize();")
    end = marks.index("\n\t}\n\tMMOFacade::", start)
    pieces.append(r'''
struct FMapMarkExport { char record[16]; };
struct FakeArchive {
    INT size;
    bool error = false;
    bool serialized = false;
    INT TotalSize() { return size; }
    bool IsError() { return error; }
    void Serialize(void* data, INT bytes) {
        serialized = true;
        memset(data, 'M', bytes);
    }
};
struct AMMOPlayer;
struct ChatArchive;
ChatArchive* g_chatArchive = nullptr;
class MMOFacade {
public:
    static MMOFacade& getSingleton() { static MMOFacade facade; return facade; }
    void EndLoadAccountBinary(FakeArchive*) {}
    AMMOPlayer* GetMainPlayer() { return nullptr; }
    BOOL BeginLoadAccountBinary(ChatArchive** archive, const char*) {
        *archive = g_chatArchive;
        return *archive != nullptr;
    }
    void EndLoadAccountBinary(ChatArchive*) {}
};
struct MarkArray {
    std::vector<FMapMarkExport> records;
    void Add(INT count) { records.resize(count); }
    void* GetData() { return records.data(); }
};
BOOL LoadMarks(FakeArchive* pArchive) {
    MarkArray vecUserMarkData;
''')
    pieces.extend([marks[start:end], "\nreturn TRUE;\n}\n"])
    pieces.append(r'''
struct CooldownData {
    INT32 m_nCdGrpCount;
    struct { INT32 m_nCdGrpID, m_nCdGrpValue; } m_CdGrp[2];
};
char* SplitStr(char*& value, char delimiter) {
    if (!value || !*value) return nullptr;
    char* token = value;
    value = strchr(value, delimiter);
    if (value) *value++ = 0;
    return token;
}
#define CONVERT_TIME(value) (value)
''')
    for name in ["item", "spell"]:
        config = source(CLIENT / ("Common/fusion/src/data/" + name + "_data.cpp"))
        loop = function(config, "while" if name == "item" else "while (1)")
        pieces.extend([
            "BOOL Parse_" + name + "(char* pValue, CooldownData* pData) {\n"
            "INT nRetCode = 0; pData->m_nCdGrpCount = 0;\n", loop,
            "\nreturn TRUE;\nExit0: return FALSE;\n}\n"])
    from member_initialization_harness import harness
    pieces.append(harness(GAME, source, function))
    from io_safety_harness import harness as io_harness
    pieces.append(io_harness(CLIENT, GAME, source, function))
    from auction_safety_harness import harness as auction_harness
    pieces.append(auction_harness(CLIENT, GAME, source, function))
    from network_safety_harness import harness as network_harness
    pieces.append(network_harness(CLIENT, GAME, source, function))
    from guild_npc_safety_harness import harness as guild_npc_harness
    pieces.append(guild_npc_harness(CLIENT, GAME, source, function))
    from html_reader_harness import harness as html_harness
    pieces.append(html_harness(GAME, source, function))
    from account_cache_harness import harness as account_harness
    pieces.append(account_harness(CLIENT, GAME, source, function))
    from container_replay_harness import harness as container_harness
    pieces.append(container_harness(CLIENT, GAME, source, function))
    from login_codec_harness import harness as login_harness
    pieces.append(login_harness(CLIENT, GAME, source, function))
    from runtime_replay_harness import harness as runtime_replay_harness
    pieces.append(runtime_replay_harness(GAME, source, function))
    from lua_fixed_string_harness import harness as lua_fixed_harness
    pieces.append(lua_fixed_harness(CLIENT, source, function))
    from message_button_harness import harness as message_button_harness
    pieces.append(message_button_harness(CLIENT, GAME, source, function))
    from chat_string_harness import harness as chat_string_harness
    pieces.append(chat_string_harness(GAME, source, function))
    from gpu_info_harness import harness as gpu_info_harness
    pieces.append(gpu_info_harness(GAME, source, function))
    from tips_stall_harness import harness as tips_stall_harness
    pieces.append(tips_stall_harness(GAME, source, function))
    from font_registration_harness import harness as font_registration_harness
    pieces.append(font_registration_harness(GAME, source, function))
    from navigation_queue_harness import harness as navigation_queue_harness
    pieces.append(navigation_queue_harness(GAME, source, function))
    from manager_defaults_harness import harness as manager_defaults_harness
    pieces.append(manager_defaults_harness(CLIENT, GAME, source, function))
    from asset_safety_harness import harness as asset_safety_harness
    pieces.append(asset_safety_harness(CLIENT, GAME, source, function))
    from equip_slot_harness import harness as equip_slot_harness
    pieces.append(equip_slot_harness(CLIENT, GAME, source, function))
    from mall_safety_harness import harness as mall_safety_harness
    pieces.append(mall_safety_harness(CLIENT, GAME, source, function))
    from master_student_harness import harness as master_student_harness
    pieces.append(master_student_harness(CLIENT, GAME, source, function))
    from exchange_soul_harness import harness as exchange_soul_harness
    pieces.append(exchange_soul_harness(CLIENT, GAME, source, function))
    from spirit_safety_harness import harness as spirit_safety_harness
    pieces.append(spirit_safety_harness(CLIENT, GAME, source, function))
    from doodad_safety_harness import harness as doodad_safety_harness
    pieces.append(doodad_safety_harness(CLIENT, GAME, source, function))
    from ranking_defaults_harness import harness as ranking_defaults_harness
    pieces.append(ranking_defaults_harness(CLIENT, GAME, source, function))
    from cache_equipment_harness import harness as cache_equipment_harness
    pieces.append(cache_equipment_harness(CLIENT, GAME, source, function))
    from facade_records_harness import harness as facade_records_harness
    pieces.append(facade_records_harness(CLIENT, GAME, source, function))
    from allocator_export_harness import harness as allocator_export_harness
    pieces.append(allocator_export_harness(GAME, source, function))
    from activity_time_harness import harness as activity_time_harness
    pieces.append(activity_time_harness(CLIENT, GAME, source, function))
    from friend_enum_harness import harness as friend_enum_harness
    pieces.append(friend_enum_harness(GAME, source, function))
    from attribute_records_harness import harness as attribute_records_harness
    pieces.append(attribute_records_harness(GAME, source, function))
    from skill_quest_harness import harness as skill_quest_harness
    pieces.append(skill_quest_harness(GAME, source, function))
    from quest_avatar_harness import harness as quest_avatar_harness
    pieces.append(quest_avatar_harness(GAME, source, function))
    from scene_records_harness import harness as scene_records_harness
    pieces.append(scene_records_harness(CLIENT, GAME, source, function))
    from equipment_growth_harness import harness as equipment_growth_harness
    pieces.append(equipment_growth_harness(CLIENT, GAME, source, function))
    from vip_defaults_harness import harness as vip_defaults_harness
    pieces.append(vip_defaults_harness(GAME, source, function))
    from quest_map_defaults_harness import harness as quest_map_defaults_harness
    pieces.append(quest_map_defaults_harness(CLIENT, GAME, source, function))
    from ability_component_defaults_harness import harness as ability_component_defaults_harness
    pieces.append(ability_component_defaults_harness(GAME, source, function))
    from remaining_index_harness import harness as remaining_index_harness
    pieces.append(remaining_index_harness(CLIENT, GAME, source, function))
    from guild_text_defaults_harness import harness as guild_text_defaults_harness
    pieces.append(guild_text_defaults_harness(CLIENT, GAME, source, function))
    from facade_money_defaults_harness import harness as facade_money_defaults_harness
    pieces.append(facade_money_defaults_harness(CLIENT, GAME, source, function))
    from screen_record_defaults_harness import harness as screen_record_defaults_harness
    pieces.append(screen_record_defaults_harness(GAME, source, function))
    from event_tile_safety_harness import harness as event_tile_safety_harness
    pieces.append(event_tile_safety_harness(GAME, source, function))
    from runtime_defaults_harness import harness as runtime_defaults_harness
    pieces.append(runtime_defaults_harness(GAME, source, function))
    from state_option_safety_harness import harness as state_option_safety_harness
    pieces.append(state_option_safety_harness(GAME, source, function))
    from inventory_lookup_harness import harness as inventory_lookup_harness
    pieces.append(inventory_lookup_harness(CLIENT, GAME, source, function))
    from position_name_harness import harness as position_name_harness
    pieces.append(position_name_harness(GAME, source, function))
    from scaleform_defaults_harness import harness as scaleform_defaults_harness
    pieces.append(scaleform_defaults_harness(GAME, source, function))
    from gameplay_lookup_harness import harness as gameplay_lookup_harness
    pieces.append(gameplay_lookup_harness(GAME, source, function))
    from navigation_marker_harness import harness as navigation_marker_harness
    pieces.append(navigation_marker_harness(GAME, source, function))
    from encoding_tool_harness import harness as encoding_tool_harness
    pieces.append(encoding_tool_harness(CLIENT, source, function))
    from buff_achievement_harness import harness as buff_achievement_harness
    pieces.append(buff_achievement_harness(GAME, source, function))
    from guide_bounds_harness import harness as guide_bounds_harness
    pieces.append(guide_bounds_harness(GAME, source, function))
    from handbook_table_harness import harness as handbook_table_harness
    pieces.append(handbook_table_harness(CLIENT, source, function))
    from quest_zone_harness import harness as quest_zone_harness
    pieces.append(quest_zone_harness(CLIENT, GAME, source, function))
    from animation_query_harness import harness as animation_query_harness
    pieces.append(animation_query_harness(GAME, source, function))
    from navigation_helper_harness import harness as navigation_helper_harness
    pieces.append(navigation_helper_harness(GAME, source, function))
    from crowd_particle_harness import harness as crowd_particle_harness
    pieces.append(crowd_particle_harness(GAME, source, function))
    from viewport_damage_harness import harness as viewport_damage_harness
    pieces.append(viewport_damage_harness(GAME, source, function))
    from texture_merge_safety_harness import harness as texture_merge_safety_harness
    pieces.append(texture_merge_safety_harness(GAME, source, function))
    from texture_copy_harness import harness as texture_copy_harness
    pieces.append(texture_copy_harness(GAME, source, function))
    from item_mesh_reputation_harness import harness as item_mesh_reputation_harness
    pieces.append(item_mesh_reputation_harness(GAME, source, function))
    from appearance_setup_harness import harness as appearance_setup_harness
    pieces.append(appearance_setup_harness(GAME, source, function))
    from search_point_defaults_harness import harness as search_point_defaults_harness
    pieces.append(search_point_defaults_harness(GAME, source, function))
    from as_value_safety_harness import harness as as_value_safety_harness
    pieces.append(as_value_safety_harness(GAME, source, function))
    from record_flag_harness import harness as record_flag_harness
    pieces.append(record_flag_harness(GAME, source, function))
    from npc_body_bounds_harness import harness as npc_body_bounds_harness
    pieces.append(npc_body_bounds_harness(GAME, source, function))
    from shadow_instance_harness import harness as shadow_instance_harness
    pieces.append(shadow_instance_harness(GAME, source, function))
    from money_visibility_harness import harness as money_visibility_harness
    pieces.append(money_visibility_harness(GAME, source, function))
    from lua_array_bounds_harness import harness as lua_array_bounds_harness
    pieces.append(lua_array_bounds_harness(CLIENT, source, function))
    from attribute_table_loading_harness import harness as attribute_table_loading_harness
    pieces.append(attribute_table_loading_harness(CLIENT, GAME, source, function))
    from array_construction_harness import harness as array_construction_harness
    pieces.append(array_construction_harness(GAME, source, function))
    from actorx_ini_path_harness import harness as actorx_ini_path_harness
    pieces.append(actorx_ini_path_harness(CLIENT, source, function))
    from actorx_string_harness import harness as actorx_string_harness
    pieces.append(actorx_string_harness(CLIENT, source, function))
    from actorx_diagnostics_harness import harness as actorx_diagnostics_harness
    pieces.append(actorx_diagnostics_harness(CLIENT, source, function))
    from actorx_name_cleaning_harness import harness as actorx_name_cleaning_harness
    pieces.append(actorx_name_cleaning_harness(CLIENT, source, function))
    from actorx_export_path_harness import harness as actorx_export_path_harness
    pieces.append(actorx_export_path_harness(CLIENT, source, function))
    pieces.append(r'''
int main() {
    MessageButtonTests::Run();
    ChatStringTests::Run();
    GpuInfoTests::Run();
    TipsStallTests::Run();
    FontTests::Run();
    NavigationTests::Run();
    ManagerDefaultsTests::Run();
    EquipSlotTests::Run();
    MallSafetyTests::Run();
    MasterStudentTests::Run();
    ExchangeSoulTests::Run();
    SpiritSafetyTests::Run();
    DoodadSafetyTests::Run();
    RankingDefaultsTests::Run();
    CacheEquipmentTests::Run();
    FacadeRecordTests::FacadeTests(); KillDefaultsTests::Run(); NpcPathTests::Run();
    AllocatorExportTests::Run();
    ActivityTimeTests::Run();
    FriendEnumTests::FriendTests(); TableEnumTests::Run();
    AttributeRecordTests::Run(); AttributeRecordTests::DropTests(); AttributeRecordTests::ArrayTests();
    SkillQuestTests::RecordTests(); SkillQuestTests::SkillTests(); SkillQuestTests::BarTests();
    QuestAvatarTests::QuestTests(); QuestAvatarTests::AvatarTests();
    SceneRecordTests::PacketTests(); SceneRecordTests::RecordTests();
    EquipmentGrowthTests::RecordTests(); EquipmentGrowthTests::Run();
    VipDefaultTests::Run();
    QuestMapDefaultTests::RecordTests(); QuestMapDefaultTests::Run();
    AbilityComponentDefaults0Tests::Run(); AbilityComponentDefaults1Tests::Run();
    RemainingIndexTests::IndexTests(); RemainingIndexTests::QuestTests();
    GuildTextDefaultTests::PacketTests(); GuildTextDefaultTests::RecordTests();
    FacadeMoneyDefaultTests::RecordTests(); FacadeMoneyDefaultTests::MoneyTests(); FacadeMoneyDefaultTests::ReloadTests();
    ScreenRecordDefaultsTests::RecordTests(); ScreenRecordDefaultsTests::QueueTests();
    EventTileSafetyTests::EventTests(); EventTileSafetyTests::TileTests();
    RuntimeDefaultTests::PerformanceTests(); RuntimeDefaultTests::RecordTests();
    StateOptionSafetyTests::StateTests(); StateOptionSafetyTests::OptionTests();
    InventoryLookupTests::InventoryTests(); InventoryLookupTests::LookupTests();
    NameBufferTests::Run(); PositionDefaultTests::Run();
    ScaleformDefault0Tests::Run(); ScaleformDefault1Tests::Run();
    GameplayLookupTests::RecordTests(); ShopHeavenLookupTests::Run();
    NavigationMarkerTests::Run();
    EncodingToolTests::Run();
    BuffAchievementTests::Run();
    GuideBoundsTests::Run();
    HandbookTableTests::Run();
    QuestZoneSafetyTests::Run();
    AnimationQueryTests::Run();
    NavigationHelper0Tests::Run(); NavigationHelper1Tests::Run();
    CrowdParticleTests::Run();
    ViewportQueryTests::Run(); DamageRecordTests::Run();
    TextureMergeSafetyTests::Run(); MaterialTextureSafetyTests::Run();
    TextureCopyTests::Run();
    ItemMeshBoundsTests::Run(); RenderCloneBoundsTests::Run(); ReputationRecordTests::Run();
    AppearanceSetupTests::Run();
    SearchPointDefaultsTests::Run();
    ASValue0_0Tests::Run(); ASValue0_1Tests::Run(); ASValue1_0Tests::Run(); ASValue1_1Tests::Run();
    RecordFlagTests::Run();
    NpcBodyBounds0::Run(); NpcBodyBounds1::Run();
    ShadowInstanceTests::Run();
    MoneyVisibilityTests::Run();
    RunLuaArrayBoundsTests();
    AttributeTableLoadingTests::Run();
    ArrayConstructionTests::Run();
    ActorXIniPathTests::Run();
    RunActorXStringTests();
    RunActorXDiagnosticTests();
    RunActorXNameCleaningTests();
    RunActorXExportPathTests();
    RecipeTests::Run(); JigsawUploadTests::Run(); ErrorPacketTests::Run();
    TestLuaFixedStrings();
    test_io_safety();
    test_member_initialization();
    AuctionTests::Run();
    NetworkTests::Run();
    GuildNpcTests::Run();
    HtmlTests::Run();
    AccountTests::Run();
    ContainerTests::Run();
    ReplayViewTests::Run();
    LoginCodecTests::Run();
    RuntimeReplayTests::Run();
    // Unterminated, empty, full, one-byte and zero-sized string buffers.
    for (size_t capacity = 1; capacity < 32; ++capacity) {
        std::vector<char> bytes(capacity + 2, 'X');
        assert(strxcat(bytes.data() + 1, "abc", capacity) == capacity - 1);
        assert(bytes[0] == 'X' && bytes[capacity + 1] == 'X');
        assert(bytes[capacity] == 0);
        bytes[1] = 0;
        strxcat(bytes.data() + 1, "abcdefghijklmnopqrstuvwxyz", capacity);
        assert(bytes[capacity] == 0);
    }
    char normal[16] = "foo";
    assert(strxcat(normal, "bar", sizeof(normal)) == 6);
    assert(!strcmp(normal, "foobar"));
    assert(strxcat(nullptr, "x", 0) == 0);

    g_nS2CMessageSize[1] = 8;
    g_nS2CMessageSize[2] = flex_message_size;
    char packet[8] = {1};
    assert(IsValidReceivedMessage(packet, 8));
    for (UINT32 size = 0; size < 8; ++size)
        assert(!IsValidReceivedMessage(packet, size));
    assert(!IsValidReceivedMessage(nullptr, 8));
    UINT16 bad_id = 65535;
    memcpy(packet, &bad_id, sizeof(bad_id));
    assert(!IsValidReceivedMessage(packet, 8));
    packet[0] = 2; packet[1] = 0; packet[2] = 8;
    assert(IsValidReceivedMessage(packet, 8));
    assert(!IsValidReceivedMessage(packet, 7));

    CSocketSession session;
    assert(session.GetNeedRecvSize() == 2);
    session.m_wRecvMsgSize = 2;
    session.m_szTempMsg[0] = 1;
    assert(session.GetNeedRecvSize() == 6);
    session.m_wRecvMsgSize = 8;
    assert(session.GetNeedRecvSize() == 0);
    g_nS2CMessageSize[1] = 7;
    assert(session.GetNeedRecvSize() < 0);
    g_nS2CMessageSize[1] = 70000;
    assert(session.GetNeedRecvSize() < 0);
    session.m_szTempMsg[0] = 2;
    session.m_wRecvMsgSize = 4;
    for (UINT16 length = 0; length < 4; ++length) {
        memcpy(session.m_szTempMsg + 2, &length, sizeof(length));
        assert(session.GetNeedRecvSize() < 0);
    }
    UINT16 length = 65535;
    memcpy(session.m_szTempMsg + 2, &length, sizeof(length));
    assert(session.GetNeedRecvSize() == 65531);
    memcpy(session.m_szTempMsg, &bad_id, sizeof(bad_id));
    assert(session.GetNeedRecvSize() < 0);

    // Every truncation of a valid variable-length item must be rejected.
    for (UINT32 flags = 0; flags <= 0x7ffff; flags += 7919) {
        std::vector<char> encoded(128, 0);
        encoded[0] = 3;
        memcpy(encoded.data() + 24, &flags, sizeof(flags));
        ItemInfo item;
        UINT32 used = item.Load(encoded.data(), (UINT32)encoded.size());
        assert(used >= 28 && used < encoded.size());
        for (UINT32 n = 0; n < used; ++n) {
            std::vector<char> truncated(encoded.begin(), encoded.begin() + n);
            assert(item.Load(truncated.data(), n) == 0);
        }
        assert(item.Load(encoded.data(), used) == used);
    }
    ItemInfo item;
    assert(item.Load(nullptr, 128) == 0);
    char wrong_version[128] = {2};
    assert(item.Load(wrong_version, sizeof(wrong_version)) == 0);

    FMemFile memory(8);
    BYTE zeros[8];
    memset(zeros, 255, sizeof(zeros));
    assert(memory.ReadByte(zeros, 1, sizeof(zeros)));
    for (BYTE value : zeros) assert(value == 0);
    memory.Seek((size_t)-1);
    assert(memory.Size() == memory.Capacity());
    assert(!memory.ReadByte(zeros, 1, 1));
    assert(!memory.ReadByte(zeros, (size_t)-1, 2));
    assert(!memory.WriteByte(zeros, (size_t)-1, 2));
    fail_allocation = true;
    assert(!memory.WriteByte(zeros, 1, 1));
    assert(memory.Size() == 8);
    fail_allocation = false;
    memory.Seek(0);
    assert(memory.WriteByte(zeros, 1, sizeof(zeros)));
    assert(memory.WriteByte(zeros, 1, sizeof(zeros)));
    assert(memory.Size() == 16);
    memory.Seek(memory.Capacity());
    assert(memory.WriteByte(memory.Get(), 1, sizeof(zeros)));
    assert(!memory.WriteByte(memory.Get() + memory.Capacity() - 1, 1, 2));
    assert(!memory.WriteByte(memory.Get() + memory.Capacity(), 1, 1));
    memory.Seek(0);
    for (size_t i = 0; i < memory.Capacity(); ++i) memory.Get()[i] = (BYTE)i;
    assert(memory.ReadByte(memory.Get() + 1, 1, 6));
    for (size_t i = 1; i <= 6; ++i) assert(memory.Get()[i] == i - 1);
    memory.Seek(0);
    assert(!memory.ReadByte(memory.Get() + memory.Capacity() - 1, 1, 2));
    assert(!memory.ReadByte(memory.Get() + memory.Capacity(), 1, 1));
    assert(memory.Size() == 0);
    FMemFile missing("file-that-does-not-exist", "rb");
    assert(missing.FileSize() == 0 && !missing.IsLoad());

    MMOJigsawManager jigsaw;
    char texture[32];
    memset(texture, 7, sizeof(texture));
    jigsaw.CopyTargetTextureData(texture, 16);
    jigsaw.CopyTargetTextureData(texture, -1);
    jigsaw.CopyTargetTextureData(texture, 17);
    jigsaw.CopyTargetTextureData(nullptr, 16);
    for (char value : jigsaw._TargetTextureData) assert(value == 7);
    jigsaw.CopyTargetTextureData(texture, 1);
    for (size_t i = 1; i < 16; ++i) assert(jigsaw._TargetTextureData[i] == 0);
    jigsaw.CopyAnswerTextureData(texture, -1, 8);
    jigsaw.CopyAnswerTextureData(texture, JIGSAW_ANSWER_COUNT, 8);
    jigsaw.CopyAnswerTextureData(texture, 0, -1);
    jigsaw.CopyAnswerTextureData(texture, 0, 9);
    for (char value : jigsaw._AnswerTextureData[0]) assert(value == 0);
    jigsaw.CopyAnswerTextureData(texture, 0, 8);
    for (char value : jigsaw._AnswerTextureData[0]) assert(value == 7);

    for (INT length = -1; length <= 65; ++length) {
        FakeArchive archive{length};
        assert(LoadMarks(&archive) == (length >= 0 && length % 16 == 0));
        assert(archive.serialized == (length > 0 && length % 16 == 0));
    }
    FakeArchive broken{16, true};
    assert(!LoadMarks(&broken));
    for (auto parse : {Parse_item, Parse_spell}) {
        CooldownData cooldowns;
        char valid[] = "1-100;2-200";
        assert(parse(valid, &cooldowns));
        assert(cooldowns.m_nCdGrpCount == 2);
        char too_many[] = "1-100;2-200;3-300";
        assert(!parse(too_many, &cooldowns));
        char malformed[] = "1";
        assert(!parse(malformed, &cooldowns));
        char invalid[] = "invalid";
        assert(!parse(invalid, &cooldowns));
    }
    puts("Native memory-safety regressions passed");
}
''')
    return "\n".join(pieces)


class MemorySafetyTests(unittest.TestCase):
    def test_generated_lua_strings(self):
        from lua_fixed_string_harness import FILES
        script = CLIENT / "Common/fusion/src/script/FixLuaStrings.ps1"
        powershell = Path(os.environ["SystemRoot"]) / "System32/WindowsPowerShell/v1.0/powershell.exe"
        with tempfile.TemporaryDirectory(prefix="lua-fixed-strings-", dir=CLIENT / "tests") as temporary:
            for filename in FILES:
                expected = (CLIENT / "Common/fusion/src/script" / filename).read_bytes()
                original = expected.decode("latin1").replace("\r\n", "\n")
                original = original.replace('#include "../../../utility/fixed_string.h"\n', '')
                original = original.replace('#include "../../../utility/lua_array_index.h"\n', '')
                original, array_checks = re.subn(
                    r'if \(!self \|\| !ClientLuaArray::Index\(tolua_tonumber\(tolua_S,2,0\), '
                    r'self->\w+, tolua_index\)\) return 0;',
                    'tolua_index = (int)tolua_tonumber(tolua_S,2,0);', original)
                self.assertGreater(array_checks, 0)
                # Recreate the generator's unsafe copies from current bindings so
                # this check still exercises regeneration after these fixes are committed.
                original, copies = re.subn(
                    r'(?:if \(!self\) return 0;\n )?ClientFixedString::Assign\((self->\w+|\w+), '
                    r'tolua_tostring\(tolua_S,2,0\)\);',
                    lambda m: 'strncpy((char*)\n' + m[1] +
                    ',(const char*)tolua_tostring(tolua_S,2,0),sizeof(' + m[1] + ')-1);', original)
                original, getters = re.subn(
                    r'(?:if \(!self\) return 0;\n )?lua_pushlstring\(tolua_S, (self->\w+|\w+), '
                    r'ClientFixedString::Length\(\1\)\);',
                    lambda m: 'tolua_pushstring(tolua_S,(const char*)' + m[1] + ');', original)
                self.assertGreater(copies, 0)
                self.assertEqual(copies, getters)
                fixture = Path(temporary) / filename
                fixture.write_bytes(original.replace("\n", "\r\n").encode("latin1"))
                command = [str(powershell), "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
                           str(script), "-Path", str(fixture)]
                processed = subprocess.run(command, capture_output=True, text=True, timeout=30)
                self.assertEqual(processed.returncode, 0, processed.stdout + processed.stderr)
                self.assertEqual(fixture.read_bytes(), expected, filename)
                repeated = subprocess.run(command, capture_output=True, text=True, timeout=30)
                self.assertEqual(repeated.returncode, 0, repeated.stdout + repeated.stderr)
                self.assertEqual(fixture.read_bytes(), expected, "not idempotent: " + filename)

    def test_native_with_address_sanitizer(self):
        for architecture in ["x86", "x64"]:
            with self.subTest(architecture=architecture):
                self._run_native_harness(architecture)

    def _run_native_harness(self, architecture):
        vswhere = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio/Installer/vswhere.exe"
        installation = subprocess.check_output([
            str(vswhere), "-latest", "-products", "*", "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"
        ], text=True).strip()
        self.assertTrue(installation, "MSVC C++ tools are required")
        toolset = sorted((Path(installation) / "VC/Tools/MSVC").iterdir())[-1]
        compiler_dir = toolset / "bin/Hostx64" / architecture
        kits = Path(os.environ["ProgramFiles(x86)"]) / "Windows Kits/10"
        sdk = sorted((kits / "Include").iterdir())[-1]
        environment = os.environ.copy()
        environment["PATH"] = str(compiler_dir) + os.pathsep + environment["PATH"]
        environment["INCLUDE"] = os.pathsep.join(str(p) for p in [
            toolset / "include", sdk / "ucrt", sdk / "shared", sdk / "um", sdk / "winrt"])
        environment["LIB"] = os.pathsep.join(str(p) for p in [
            toolset / "lib" / architecture, kits / "Lib" / sdk.name / "ucrt" / architecture,
            kits / "Lib" / sdk.name / "um" / architecture])
        with tempfile.TemporaryDirectory(prefix="client-memory-safety-", dir=CLIENT / "tests") as temporary:
            directory = Path(temporary)
            (directory / "regression.cpp").write_text(native_harness(), encoding="utf-8")
            zlib_sources = CLIENT / "client/UE3/Development/External/zlib/Src"
            zlib_build = subprocess.run([
                str(compiler_dir / "cl.exe"), "/nologo", "/c", "/fsanitize=address", "/Zi", "/MD",
                *[str(zlib_sources / (name + ".c")) for name in [
                    "uncompr", "inflate", "inffast", "inftrees", "adler32", "crc32", "zutil"]]
            ], cwd=directory, env=environment, capture_output=True, text=True)
            self.assertEqual(zlib_build.returncode, 0, zlib_build.stdout + zlib_build.stderr)
            build = subprocess.run([
                str(compiler_dir / "cl.exe"), "/nologo", "/EHsc", "/std:c++17", "/utf-8", "/bigobj",
                "/fsanitize=address", "/Zi", "/MD", "/Fe:regression.exe", "regression.cpp",
                *[str(path) for path in directory.glob("*.obj")]
            ], cwd=directory, env=environment, capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            run = subprocess.run([str(directory / "regression.exe")], cwd=directory,
                                 env=environment, capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
            print(architecture + ": " + run.stdout)
            from patch_conversion_harness import harness as patch_harness
            (directory / "patch.cpp").write_text(
                patch_harness(CLIENT, source, function), encoding="utf-8")
            build = subprocess.run([
                str(compiler_dir / "cl.exe"), "/nologo", "/EHsc", "/std:c++17", "/utf-8",
                "/fsanitize=address", "/Zi", "/MD", "/Fe:conversion_regression.exe", "patch.cpp", "kernel32.lib"
            ], cwd=directory, env=environment, capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            run = subprocess.run([str(directory / "conversion_regression.exe")], cwd=directory,
                                 env=environment, capture_output=True, text=True, timeout=45)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
            print(architecture + ": " + run.stdout)

    def test_process_enumeration_failures(self):
        for relative in ["_AutoBuild/python_client8089/win32_process.py",
                         "client/ClientBuildServer/build_python/ClientBuildServer/win32_process.py"]:
            spec = importlib.util.spec_from_file_location("win32_process_test", CLIENT / relative)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            self.assertEqual(module.kernel.OpenProcess.restype, ctypes.wintypes.HANDLE)
            closed = []
            class FakeKernel:
                def OpenProcess(self, *args):
                    return 0x100000001
                def CloseHandle(self, handle):
                    closed.append(handle)
            class FakePsapi:
                def EnumProcesses(self, ids, size, needed):
                    ids[0] = 42
                    needed._obj.value = ctypes.sizeof(ctypes.wintypes.DWORD)
                    return True
                def EnumProcessModules(self, handle, module, size, needed):
                    return False
            module.kernel = FakeKernel()
            module.psapi = FakePsapi()
            self.assertEqual(module.EnumProcesses(), {})
            self.assertEqual(closed, [0x100000001])
            class FailingPsapi(FakePsapi):
                def EnumProcesses(self, *args):
                    return False
            module.psapi = FailingPsapi()
            self.assertEqual(module.EnumProcesses(), {})
            class NamedPsapi(FakePsapi):
                def EnumProcessModules(self, handle, module, size, needed):
                    module._obj.value = 0x100000002
                    needed._obj.value = size
                    return True
                def GetModuleBaseNameA(self, handle, module, name, size):
                    name.value = b"MMOGame.exe"
                    return len(name.value)
            module.psapi = NamedPsapi()
            self.assertEqual(module.EnumProcesses(), {b"MMOGame.exe": 42})
            class FailedNamePsapi(NamedPsapi):
                def GetModuleBaseNameA(self, *args):
                    return 0
            module.psapi = FailedNamePsapi()
            self.assertEqual(module.EnumProcesses(), {})


if __name__ == "__main__":
    unittest.main()
