"""Compile real guild entry checks and NPC cache updates with wire declarations."""
import re


def harness(client, game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(path))

    m2c = clean(client / "Common/message/m2c_message.h")
    g2c = clean(client / "Common/message/g2c_message.h")
    guild = clean(game / "net/message_handler_Guild.cpp")
    npc = clean(game / "net/message_handler_NPC.cpp")
    parsers = [
        ("m2c_view_guild_list_ack", "M2C_VIEW_GUILD_LIST_ACK", "byCount", "info"),
        ("m2c_guild_more_history", "M2C_GUILD_MORE_HISTORY", "wEventNum", "stEventInfo"),
        ("m2c_get_apply_guild_list_ack", "M2C_GET_APPLY_GUILD_LIST_ACK", "wAppCount", "info"),
        ("m2c_guild_get_apply_union_list_ack", "M2C_GUILD_GET_APPLY_UNION_LIST_ACK", "byCount", "info"),
        ("g2c_sync_purchase_aura_record", "G2C_SYNC_PURCHASE_AURA_RECORD", "wCount", "data"),
        ("m2c_guild_get_dungeon_list_ack", "M2C_GUILD_GET_DUNGEON_LIST_ACK", "byDungeonCount", "info"),
        ("m2c_guild_battle_achievement", "M2C_GUILD_BATTLE_ACHIEVEMENT", "byNum", "info"),
        ("m2c_guild_more_chest", "M2C_GUILD_MORE_CHEST", "wEventNum", "stEvent"),
        ("m2c_guild_war_record", "M2C_GUILD_WAR_RECORD", "wCount", "info"),
    ]
    world_parsers = [
        ("message_handler_Combat.cpp", "g2c_focus_info", "G2C_FOCUS_INFO", "byAuraCount", "astAuraData", "AMMOActor* actor"),
        ("message_handler_Item.cpp", "g2c_sell_resource_item", "G2C_SELL_RESOURCE_ITEM", "nSoldCount", "itemslots", "if (msg)"),
        ("message_handler_Scene.cpp", "m2c_query_line_ack", "M2C_QUERY_LINE_ACK", "wCount", "LineSceneData", "MMOMapDataManager &mapDataMgr"),
    ]
    wire = m2c + g2c
    declarations = {}
    for match in re.finditer(r"\bstruct\s+(\w+)\b[^;{]*\{", wire):
        declarations[match[1]] = function(wire[match.start():], match.group()) + ";"
    needed, emitted = [], set()

    def include(name):
        if name in emitted:
            return
        emitted.add(name)
        text = declarations[name]
        for dependency in re.findall(r"\b\w+\b", text):
            if dependency in declarations and dependency != name:
                include(dependency)
        needed.append(text)

    for name in [item[1] for item in parsers] + [item[2] for item in world_parsers] + [
            "M2C_GUILD_INFO_NTF", "M2C_GUILD_DUNGEON_AWARD", "G2C_SYNC_NPC_EX_DATA", "G2C_INTERACT_RESULT",
            "G2C_SYNC_QUEST_LIST", "G2C_SYNC_ROLE_SPIRIT_DATA", "G2C_SYNC_ACHIEVEMENT_LIST",
            "G2C_COMPARE_ACHIEVEMENT", "M2C_ACTIVITY_QUERY_TEAM", "G2C_ACTIVITY_REWARD", "G2C_SYNC_COMBAT_STAT"]:
        include(name)
    parts = ["namespace GuildNpcTests {\nusing INT16 = int16_t;\nusing CHAR = char;"]
    # Use the real constants governing every fixed array in these packets.
    macro_names = []
    for file in ["string_def.h", "guild_def.h", "variable_def.h", "team_def.h"]:
        text = clean(client / "Common/define" / file)
        for macro in re.finditer(r"(?m)^#define\s+(\w+)\s+([^\n]+)", text):
            if macro[1].endswith("_H_"):
                continue
            macro_names.append(macro[1])
            parts.append(macro.group())
    guild_defs = clean(client / "Common/define/guild_def.h")
    parts.extend(function(guild_defs, "enum " + name) + ";"
                 for name in ["GUILD_BUILDING_ENUM", "GUILD_POSITION_TYPE"])
    parts.append(function(clean(client / "Common/define/char_def.h"), "enum ENUM_COMBAT_STAT_ENTRY") + ";")
    parts.extend(["#pragma pack(push, 1)", *needed, "#pragma pack(pop)"])
    database = clean(client / "Common/define/role_db_def_v_3.h")
    parts.append("#pragma pack(push, 1)")
    parts.extend(function(database, "struct " + name) + ";" for name in [
        "ROLE_DB_CIRCLE_QUEST_DATA", "ROLE_DB_REWARD_QUEST_DATA", "ROLE_DB_QUEST_DATA", "ROLE_DB_SPIRIT_DATA",
        "ROLE_DB_ACHIEVEMENT_DATA"])
    parts.append("#pragma pack(pop)")
    for parser, packet, _, _ in parsers:
        body = function(guild, "void CMessageHandler::Parse_" + parser + "(")
        prefix = body[body.index("{") + 1:body.index("MMOGuildManager::")]
        parts.append("bool Validate_" + packet + "(const char* buf, UINT32 size) {" +
                     prefix.replace("return;", "return false;") + "return true; }")
    for file, parser, packet, _, _, end in world_parsers:
        body = function(clean(game / "net" / file), "void CMessageHandler::Parse_" + parser + "(")
        prefix = body[body.index("{") + 1:body.index(end)]
        parts.append("bool Validate_" + packet + "(const char* buf, UINT32 size) {" +
                     prefix.replace("return;", "return false;") + "return true; }")
    for file, parser, name, end in [
        ("message_handler_Quest.cpp", "g2c_sync_quest_list", "Quest", "MMOQuestManager::"),
        ("message_handler_PlayerInfo.cpp", "g2c_sync_role_spirit_data", "Spirit", "if(!msg)"),
        ("message_handler_Achievement.cpp", "g2c_sync_achievement_list", "Achievement", "MMOAchievementManager::"),
        ("message_handler_Achievement.cpp", "g2c_compare_achievement", "Compare", "MMOAchievementManager::"),
        ("message_handler_Scene.cpp", "g2c_sync_combat_stat", "CombatStat", "INT byCount"),
    ]:
        body = function(clean(game / "net" / file), "void CMessageHandler::Parse_" + parser + "(")
        prefix = body[body.index("{") + 1:body.index(end)]
        parts.append("bool Validate" + name + "(const char* buf, UINT32 size) {" +
                     prefix.replace("return;", "return false;") + "return true; }")
    activity = clean(game / "ActivityManager.cpp")
    body = function(activity, "void ActivityManager::UpdateActivityTeamList(")
    prefix = body[body.index("{") + 1:body.index("BOOL bSingle")]
    parts.append("bool ValidateActivityTeams(const char* buf, UINT32 size) {" +
                 prefix.replace("return;", "return false;") + "return true; }")
    for parser, packet, end in [
        ("m2c_guild_info_ntf", "M2C_GUILD_INFO_NTF", "GuildBaseInfo baseInfo"),
        ("m2c_guild_dungeon_award", "M2C_GUILD_DUNGEON_AWARD", "MMOGuildManager::"),
    ]:
        body = function(guild, "void CMessageHandler::Parse_" + parser + "(")
        prefix = body[body.index("{") + 1:body.index(end)]
        parts.append("bool Validate_" + packet + "(const char* buf, UINT32 size) {" +
                     prefix.replace("return;", "return false;") + "return true; }")
    dialog = function(npc, "void CMessageHandler::Parse_g2c_interact_result(")
    prefix = dialog[dialog.index("{") + 1:dialog.index("ParseContext ctx")]
    parts.append("bool ValidateDialog(const char* buf, UINT32 size) {" +
                 prefix.replace("return;", "return false;") + "return true; }")
    parts.append(r'''
enum { E_npct_Npc = 1 };
struct MMOFacade {
    std::vector<UINT32> changed;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    void AddOjbectToSync(UINT32 id) { changed.push_back(id); }
};
struct Cache {
    std::map<UINT32, G2C_SYNC_NPC_EX_DATA*> records;
    ~Cache() { for (const auto& item : records) delete item.second; }
    G2C_SYNC_NPC_EX_DATA* GetCacheData(UINT32 id) {
        auto found = records.find(id);
        return found == records.end() ? nullptr : found->second;
    }
    void AddNewData(UINT32 id, G2C_SYNC_NPC_EX_DATA* value) { records[id] = value; }
};
struct CMessageHandler {
    Cache m_NPCDataCache;
    void Parse_g2c_sync_npc_data(const char*, UINT32);
    void Parse_g2c_sync_npc_ex_data(const char*, UINT32);
    void Parse_g2c_activity_reward(const char*, UINT32);
};
struct ActivityManager {
    INT calls = 0;
    INT32 bits = 0;
    static ActivityManager& getSingleton() { static ActivityManager value; return value; }
    INT32 GetRewardBit() { return bits; }
    void NotifyActivityVariableErvtActivityReward(INT32 value) { ++calls; bits = value; }
};
#define check(value) assert(value)
''')
    parts.extend(function(npc, "void CMessageHandler::Parse_" + parser + "(")
                 for parser in ["g2c_sync_npc_data", "g2c_sync_npc_ex_data"])
    parts.append(function(clean(game / "net/message_handler_Activity.cpp"), "void CMessageHandler::Parse_g2c_activity_reward("))
    parts.append("void Run() {")
    for _, packet, count, array in parsers + [item[1:5] for item in world_parsers]:
        parts.append(r'''
    for (UINT32 count : {0u, 1u, 3u}) {
        const size_t header = sizeof(PACKET);
        const size_t record = sizeof(((PACKET*)nullptr)->ARRAY[0]);
        std::vector<char> bytes(header + count * record, 0);
        auto* message = reinterpret_cast<PACKET*>(bytes.data());
        message->COUNT = (decltype(message->COUNT))count;
        assert(Validate_PACKET(bytes.data(), (UINT32)bytes.size()));
        for (UINT32 length = 0; length < bytes.size(); ++length) {
            std::vector<char> truncated(bytes.begin(), bytes.begin() + length);
            assert(!Validate_PACKET(truncated.data(), length));
        }
        message->COUNT = 255;
        assert(!Validate_PACKET(bytes.data(), (UINT32)bytes.size()));
    }
'''.replace("PACKET", packet).replace("COUNT", count).replace("ARRAY", array))
    parts.append(r'''
    G2C_SYNC_PURCHASE_AURA_RECORD negative = {};
    negative.wCount = -1;
    assert(!Validate_G2C_SYNC_PURCHASE_AURA_RECORD(reinterpret_cast<const char*>(&negative), sizeof(negative)));
    G2C_SELL_RESOURCE_ITEM negativeSale = {};
    negativeSale.nSoldCount = -1;
    assert(!Validate_G2C_SELL_RESOURCE_ITEM(reinterpret_cast<const char*>(&negativeSale), sizeof(negativeSale)));
    for (UINT32 count : {0u, 1u, 3u}) {
        const size_t header = sizeof(G2C_SYNC_QUEST_LIST) + sizeof(ROLE_DB_QUEST_DATA);
        std::vector<char> quest(header + count * sizeof(ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST), 0);
        auto* data = reinterpret_cast<ROLE_DB_QUEST_DATA*>(quest.data() + sizeof(G2C_SYNC_QUEST_LIST));
        data->byAcceptedQuestCount = (UINT8)count;
        assert(ValidateQuest(quest.data(), (UINT32)quest.size()));
        for (UINT32 length = 0; length < quest.size(); ++length) {
            std::vector<char> truncated(quest.begin(), quest.begin() + length);
            assert(!ValidateQuest(truncated.data(), length));
        }
        data->byAcceptedQuestCount = 255;
        assert(!ValidateQuest(quest.data(), (UINT32)quest.size()));
    }
    std::vector<char> spirit(sizeof(G2C_SYNC_ROLE_SPIRIT_DATA) + sizeof(ROLE_DB_SPIRIT_DATA), 0);
    assert(ValidateSpirit(spirit.data(), (UINT32)spirit.size()));
    for (UINT32 length = 0; length < spirit.size(); ++length) {
        std::vector<char> truncated(spirit.begin(), spirit.begin() + length);
        assert(!ValidateSpirit(truncated.data(), length));
    }
    for (auto scenario : {std::make_pair(sizeof(G2C_SYNC_ACHIEVEMENT_LIST), &ValidateAchievement),
                          std::make_pair(sizeof(G2C_COMPARE_ACHIEVEMENT), &ValidateCompare)}) {
        std::vector<char> data(scenario.first + sizeof(ROLE_DB_ACHIEVEMENT_DATA), 0);
        assert(scenario.second(data.data(), (UINT32)data.size()));
        for (UINT32 length = 0; length < data.size(); ++length) {
            std::vector<char> truncated(data.begin(), data.begin() + length);
            assert(!scenario.second(truncated.data(), length));
        }
    }
    M2C_ACTIVITY_QUERY_TEAM activity = {};
    const char* activityStart = reinterpret_cast<const char*>(&activity);
    assert(ValidateActivityTeams(activityStart, sizeof(activity)));
    for (UINT32 length = 0; length < sizeof(activity); ++length) {
        std::vector<char> truncated(activityStart, activityStart + length);
        assert(!ValidateActivityTeams(truncated.data(), length));
    }
    const size_t combatHeader = sizeof(G2C_SYNC_COMBAT_STAT);
    const size_t combatRecord = sizeof(G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA);
    for (UINT32 length = 0; length <= combatHeader + 3 * combatRecord; ++length) {
        std::vector<char> bytes(length, 0);
        assert(ValidateCombatStat(bytes.data(), length) ==
               (length >= combatHeader && (length - combatHeader) % combatRecord == 0));
    }
    CMessageHandler rewards;
    G2C_ACTIVITY_REWARD reward = {};
    auto& state = ActivityManager::getSingleton();
    for (UINT32 index : {0u, 1u, 30u, 31u, 32u, 65535u}) {
        reward.wIndex = (UINT16)index;
        state.bits = 2;
        const INT calls = state.calls;
        rewards.Parse_g2c_activity_reward(reinterpret_cast<const char*>(&reward), sizeof(reward));
        assert(state.calls == calls + (index < 32));
        assert((UINT32)state.bits == (index < 32 ? 2u | (1u << index) : 2u));
    }
    M2C_GUILD_INFO_NTF info = {};
    assert(Validate_M2C_GUILD_INFO_NTF(reinterpret_cast<const char*>(&info), sizeof(info)));
    for (UINT8* count : {&info.stEventInfo.byEventCount, &info.stChestInfo.byCount,
                        &info.byAllianceCount, &info.byEnemyCount, &info.byMemberCount}) {
        *count = 255;
        assert(!Validate_M2C_GUILD_INFO_NTF(reinterpret_cast<const char*>(&info), sizeof(info)));
        *count = 0;
    }
    info.stEventInfo.byEventCount = ARRAY_COUNT(info.stEventInfo.stEventInfo);
    info.stChestInfo.byCount = ARRAY_COUNT(info.stChestInfo.stEvent);
    info.byAllianceCount = ARRAY_COUNT(info.stAlliance);
    info.byEnemyCount = ARRAY_COUNT(info.stEnemy);
    assert(Validate_M2C_GUILD_INFO_NTF(reinterpret_cast<const char*>(&info), sizeof(info)));
    M2C_GUILD_DUNGEON_AWARD award = {};
    for (UINT32 count = 0; count <= 255; ++count) {
        award.byItemCount = (UINT8)count;
        assert(Validate_M2C_GUILD_DUNGEON_AWARD(reinterpret_cast<const char*>(&award), sizeof(award)) ==
               (count <= ARRAY_COUNT(award.arrItem)));
    }
    for (const std::string text : {std::string(), std::string("abc"), std::string("abc\0", 4)}) {
        std::vector<char> bytes(sizeof(G2C_INTERACT_RESULT) + text.size(), 0);
        memcpy(bytes.data() + sizeof(G2C_INTERACT_RESULT), text.data(), text.size());
        assert(ValidateDialog(bytes.data(), (UINT32)bytes.size()) == (text.size() == 4));
    }
    CMessageHandler handler;
    G2C_SYNC_NPC_DATA normal = {};
    normal.dwNpcID = 123;
    normal.nTemplateID = 456;
    normal.byLevel = 7;
    const char* begin = reinterpret_cast<const char*>(&normal);
    for (UINT32 length = 0; length < sizeof(normal); ++length) {
        std::vector<char> truncated(begin, begin + length);
        handler.Parse_g2c_sync_npc_data(truncated.data(), length);
        assert(handler.m_NPCDataCache.records.empty());
    }
    handler.Parse_g2c_sync_npc_data(begin, sizeof(normal));
    auto* cached = handler.m_NPCDataCache.GetCacheData(123);
    assert(cached && cached->byLevel == 7 && cached->nTemplateID == 456);
    assert(cached->byType == E_npct_Npc && !cached->byPetLevel && !cached->dwHostID);
    for (char byte : cached->szHostName) assert(!byte);
    G2C_SYNC_NPC_EX_DATA pet = {};
    *static_cast<G2C_SYNC_NPC_DATA*>(&pet) = normal;
    pet.byType = 2; pet.byPetLevel = 3; pet.dwHostID = 999;
    memset(pet.szHostName, 'N', sizeof(pet.szHostName));
    handler.Parse_g2c_sync_npc_ex_data(reinterpret_cast<const char*>(&pet), sizeof(pet));
    assert(cached->byType == 2 && cached->dwHostID == 999);
    handler.Parse_g2c_sync_npc_data(begin, sizeof(normal));
    assert(cached->byType == E_npct_Npc && !cached->byPetLevel && !cached->dwHostID);
    for (char byte : cached->szHostName) assert(!byte);
    puts("Guild packet and NPC cache regressions passed");
}
#undef check
''')
    parts.extend("#undef " + name for name in macro_names)
    parts.append("}")
    return "\n".join(parts)
