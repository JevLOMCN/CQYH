"""Actual money visibility setup and notifications before init/with invalid IDs."""
import re


def harness(game, source, function):
    text = re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(game / 'Src/MMOUIDamageNumberLevel.cpp'))
    parts = [r'''
#pragma push_macro("TEXT")
#undef TEXT
#define TEXT(x) x
namespace MoneyVisibilityTests {
enum { E_MONEYTYPE_MAX = 8, MONEYTYPE_MORAL = 6, MONEYTYPE_MASTER_EXP = 7, EGainType_Money = 1 };
struct FString {
    INT index;
    static FString Printf(const char*, INT i) { return {i}; }
};
struct TweenData { UBOOL isVisible; };
struct AMMOPlayer { INT Location = 123; };
struct AMMOHud {
    INT calls = 0, value = 0, type = -1;
    void AddGainInfo(INT gain, INT amount, INT location, INT moneyType) {
        assert(gain == EGainType_Money && location == 123);
        ++calls; value = amount; type = moneyType;
    }
};
struct MMOFacade {
    TweenData data{FALSE};
    AMMOHud hud;
    UBOOL hasHud = TRUE;
    static MMOFacade& getSingleton() { static MMOFacade singleton; return singleton; }
    TweenData* GetTweenData(FString name) { return name.index == 2 ? &data : nullptr; }
    INT GetPlayerCurExp() { return 12; }
    INT GetPlayerMaxExp() { return 30; }
    AMMOHud* GetMyHud() { return hasHud ? &hud : nullptr; }
};
struct UMMOUIDamageNumberLevel {
    ShadowArray<UBOOL> IsMoneyVisible;
    INT LastExp = -1, LastMaxExp = -1;
    AMMOPlayer* Player = nullptr;
    void InitInfo();
    void PlayerMoneyChange(BYTE, INT);
};
''']
    for signature in ['void UMMOUIDamageNumberLevel::InitInfo(',
                      'void UMMOUIDamageNumberLevel::PlayerMoneyChange(']:
        parts.append(function(text, signature))
    parts.append(r'''
void Run() {
    UMMOUIDamageNumberLevel panel;
    AMMOPlayer player; panel.Player = &player;
    auto& facade = MMOFacade::getSingleton();
    for (INT id = 0; id <= 255; ++id) panel.PlayerMoneyChange(BYTE(id), 5000);
    assert(facade.hud.calls == 0);
    panel.InitInfo(); panel.InitInfo();
    assert(panel.IsMoneyVisible.Num() == E_MONEYTYPE_MAX && panel.LastExp == 12 && panel.LastMaxExp == 30);
    for (INT id = 0; id <= 255; ++id) {
        facade.hud.calls = 0;
        panel.PlayerMoneyChange(BYTE(id), 5000);
        const bool visible = id < E_MONEYTYPE_MAX && id != 2;
        assert(facade.hud.calls == (visible ? 1 : 0));
        if (visible) {
            assert(facade.hud.type == id);
            assert(facade.hud.value == (id == MONEYTYPE_MORAL || id == MONEYTYPE_MASTER_EXP ? 5 : 5000));
        }
    }
    facade.hud.calls = 0;
    panel.PlayerMoneyChange(0, 0); panel.PlayerMoneyChange(0, -1);
    panel.Player = nullptr; panel.PlayerMoneyChange(0, 1);
    panel.Player = &player; facade.hasHud = FALSE; panel.PlayerMoneyChange(0, 1);
    assert(facade.hud.calls == 0);
    puts("Money notifications bound every byte-sized currency ID and repeated initialization retains a fully set visibility array");
}
}
#pragma pop_macro("TEXT")
''')
    return '\n'.join(parts)
