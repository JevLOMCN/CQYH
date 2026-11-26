"""Exercise actual NPC body-size parsing with exact-size string allocations."""
import re


def harness(game, source, function):
    text = re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(game / 'Src/ANPC.cpp'))
    methods = '\n'.join(function(text, signature) for signature in [
        'INT32 ANPC::CalGridBlockRadius(', 'void ANPC:: InitBodySize('])
    parts = ['#include <memory>', '#pragma push_macro("TEXT")', '#undef TEXT']
    for wide in [False, True]:
        parts.extend([
            '#define TEXT(x) ' + ('L##x' if wide else 'x'),
            'namespace NpcBodyBounds' + str(int(wide)) + ' {',
            'using TCHAR = ' + ('wchar_t' if wide else 'char') + ';',
            r'''
struct FString {
    std::unique_ptr<TCHAR[]> data;
    INT length;
    explicit FString(const std::basic_string<TCHAR>& value)
        : data(new TCHAR[value.size() + 1]), length((INT)value.size()) {
        std::copy(value.begin(), value.end(), data.get()); data[length] = 0;
    }
    INT Len() const { return length; }
    const TCHAR* operator*() const { return data.get(); }
    const TCHAR& operator[](INT index) const {
        assert(index >= 0 && index < length); return data[index];
    }
};
struct UNPCTemplate {
    FString BodyType;
    explicit UNPCTemplate(const std::basic_string<TCHAR>& value) : BodyType(value) {}
};
enum { EACT_Boss = 1, EACT_Monster = 2, E_body_1x1 = 1, E_body_3x3 = 3 };
struct ANPC {
    UNPCTemplate* Template = nullptr;
    UBOOL bDead = FALSE;
    INT ActorType = EACT_Monster, CurBlockType = -999, BodySize = E_body_3x3;
    INT32 CalGridBlockRadius();
    void InitBodySize();
};
''', methods, r'''
void Check(const std::basic_string<TCHAR>& input, INT blockType) {
    UNPCTemplate npcTemplate(input);
    for (INT actorType : {INT(EACT_Boss), INT(EACT_Monster)}) {
        ANPC npc; npc.Template = &npcTemplate; npc.ActorType = actorType;
        const INT radius = blockType ? blockType / 2 : (actorType == EACT_Boss ? 0 : -1);
        assert(npc.CalGridBlockRadius() == radius && npc.CurBlockType == blockType);
        npc.InitBodySize(); assert(npc.BodySize == (blockType >= 3 ? E_body_3x3 : E_body_1x1));
        npc.bDead = TRUE; assert(npc.CalGridBlockRadius() == -1);
    }
}
void Run() {
    for (INT length = 0; length < 5; ++length) Check(std::basic_string<TCHAR>(length, TEXT('a')), 1);
    for (TCHAR bad : {TEXT('/'), TEXT(':'), TEXT('x'), TCHAR(0)}) {
        std::basic_string<TCHAR> input = TEXT("body1x1"); input[4] = bad; Check(input, 1);
    }
    for (INT digit = 0; digit <= 9; ++digit) {
        std::basic_string<TCHAR> input = TEXT("body1x1"); input[4] = TCHAR(TEXT('0') + digit);
        Check(input, digit); Check(input.substr(0, 5), digit);
    }
    Check(TEXT("E_body_1x1"), 1);
    ANPC missing;
    assert(missing.CalGridBlockRadius() == 0 && missing.CurBlockType == 1);
    missing.InitBodySize(); assert(missing.BodySize == E_body_1x1);
    puts("NPC body parsing bounds empty/short strings and falls back for invalid digits (ANSI/Unicode)");
}
}
''', '#undef TEXT'])
    parts.append('#pragma pop_macro("TEXT")')
    return '\n'.join(parts)
