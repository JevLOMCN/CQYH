"""Real event overload defaults and map-tile list boundary regressions."""
import re


def harness(game, source, function):
    header = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / "CommonGameEvents.h"))
    parts = [r'''
namespace EventTileSafetyTests {
using UBOOL = UINT; // Match Core/Inc/UnVcWin32.h so the integer and boolean overloads differ.
using FString = std::string;
struct AMMOPlayer {};
''']
    for name in ["MMOGameEvent_MatineeSubtitle", "MMOGameEvent_MainPlayerDataChange"]:
        parts.append(function(header, "struct " + name + " :") + ";")
    parts.append(r'''
void EventTests() {
    using Subtitle = MMOGameEvent_MatineeSubtitle;
    using Change = MMOGameEvent_MainPlayerDataChange;
    AMMOPlayer player;
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(Subtitle) unsigned char subtitle_bytes[sizeof(Subtitle)];
        memset(subtitle_bytes, pattern, sizeof(subtitle_bytes));
        auto* subtitle = new(subtitle_bytes) Subtitle(FString("text"), Subtitle::EMSET_UpdateSubtitle);
        assert(subtitle->SubtitleEventType == Subtitle::EMSET_UpdateSubtitle && !subtitle->ShowSubtitle && subtitle->Subtitle == "text");
        assert(subtitle->GetEventType() == GameEvent_MatineeSubtitle);
        subtitle->~Subtitle();
        for (bool show : {false, true}) {
            memset(subtitle_bytes, pattern, sizeof(subtitle_bytes));
            subtitle = new(subtitle_bytes) Subtitle(show, Subtitle::EMSET_Show);
            assert(subtitle->SubtitleEventType == Subtitle::EMSET_Show && subtitle->ShowSubtitle == show && subtitle->Subtitle.empty());
            subtitle->~Subtitle();
        }
        alignas(Change) unsigned char change_bytes[sizeof(Change)];
        memset(change_bytes, pattern, sizeof(change_bytes));
        auto* change = new(change_bytes) Change(Change::EMSET_HP, &player, 17, 43);
        assert(change->DataChangeEventType == Change::EMSET_HP && change->MainPlayer == &player);
        assert(change->CurrentIntValue == 17 && change->MaxIntValue == 43);
        assert(!change->CurrentFloatValue && change->MaxFloatValue == 1.0f && !change->BoolValue);
        assert(change->GetEventType() == GameEvent_MainPlayerDataChange);
        change->~Change();
        memset(change_bytes, pattern, sizeof(change_bytes));
        change = new(change_bytes) Change(Change::EMSET_MP, &player, 0.25f, 0.75f);
        assert(change->DataChangeEventType == Change::EMSET_MP && change->MainPlayer == &player);
        assert(!change->CurrentIntValue && change->MaxIntValue == 1);
        assert(change->CurrentFloatValue == 0.25f && change->MaxFloatValue == 0.75f && !change->BoolValue);
        change->~Change();
        for (UBOOL flag : {UBOOL(FALSE), UBOOL(TRUE)}) {
            memset(change_bytes, pattern, sizeof(change_bytes));
            change = new(change_bytes) Change(Change::EMSET_HP, &player, flag);
            assert(change->DataChangeEventType == Change::EMSET_HP && change->MainPlayer == &player);
            assert(!change->CurrentIntValue && change->MaxIntValue == 1);
            assert(!change->CurrentFloatValue && change->MaxFloatValue == 1.0f && change->BoolValue == flag);
            change->~Change();
        }
    }
    puts("Subtitle and player-data event overload defaults passed (two memory patterns)");
}
void TileTests() {
    ::MMOMapImgTiles tiles(2, 3);
    tiles._texLoaderName = "previous";
    ::TArray<UGFxObject*> panels;
    for (INT count : {0, 1, 2, 3, 4, 5, 7, 32}) {
        panels.assign(count, nullptr);
        assert(!tiles.LoadUiTile(panels, "rejected"));
        assert(tiles._texLoaderName.value == "previous");
        for (INT i = 0; i < tiles._TileUiRowMax; ++i)
            for (INT j = 0; j < tiles._TileUiColMax; ++j)
                assert(!tiles._ShowTile[i][j]._UiObj && !tiles._ShowTile[i][j]._ImgLoderIdx && !tiles._ShowTile[i][j]._IsChanged);
    }
    panels.assign(6, nullptr);
    INT marker;
    panels[5] = reinterpret_cast<UGFxObject*>(&marker);
    assert(tiles.LoadUiTile(panels, "accepted"));
    assert(tiles._texLoaderName.value == "accepted");
    for (INT i = 0; i < tiles._TileUiRowMax; ++i) {
        for (INT j = 0; j < tiles._TileUiColMax; ++j) {
            INT index = i * tiles._TileUiColMax + j;
            assert(tiles._ShowTile[i][j]._UiObj == panels[index]);
            assert(tiles._ShowTile[i][j]._ImgLoderIdx == index + 1 && tiles._ShowTile[i][j]._IsChanged);
        }
    }
    auto rows = tiles._TileUiRowMax, columns = tiles._TileUiColMax;
    // Validate oversized products without allocating a huge tile grid.
    tiles._TileUiRowMax = tiles._TileUiColMax = 65535;
    assert(!tiles.LoadUiTile(panels, "oversized"));
    tiles._TileUiRowMax = 0;
    assert(!tiles.LoadUiTile({}, "empty"));
    tiles._TileUiRowMax = rows; tiles._TileUiColMax = columns;
    assert(tiles._texLoaderName.value == "accepted");
    puts("Map tile lists rejected before indexing, exact-size mapping and large dimensions passed");
}
}
''')
    return "\n".join(parts)
