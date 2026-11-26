"""Real gameplay fallback records and shop/Heaven inventory lookup methods."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    generated = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    parts = [r'''
namespace GameplayLookupTests {
struct UAbilityComponent {};
struct USoundCue {};
enum EEventParm { EC_EventParm };
enum ENoInit { E_NoInit };
enum EForceInit { E_ForceInit };
template<class T> struct TArray {
    T* data;
    INT count, capacity;
    TArray() : data(nullptr), count(0), capacity(0) {}
    // Poison the deliberately uninitialized base state so a missing EventParm
    // initialization fails deterministically rather than relying on stack reuse.
    TArray(ENoInit) { memset(this, 0xa5, sizeof(*this)); }
    void Validate() const { assert(count >= 0 && capacity == count); assert((count == 0) == (data == nullptr)); }
    void Clear() {
        Validate();
        for (INT i = 0; i < count; ++i) data[i].~T();
        ::operator delete(data); data = nullptr; count = capacity = 0;
    }
    TArray(const TArray& other) : TArray() { *this = other; }
    TArray& operator=(const TArray& other) {
        Validate(); other.Validate();
        if (this != &other) {
            Clear(); count = capacity = other.count;
            data = count ? static_cast<T*>(::operator new(sizeof(T) * count)) : nullptr;
            for (INT i = 0; i < count; ++i) new(data + i) T(other.data[i]);
        }
        return *this;
    }
    ~TArray() { Clear(); }
    INT Num() const { Validate(); return count; }
    T& operator()(INT index) { Validate(); assert(index >= 0 && index < count); return data[index]; }
    const T& operator()(INT index) const { Validate(); assert(index >= 0 && index < count); return data[index]; }
    void SetOne(T value) { Clear(); data = static_cast<T*>(::operator new(sizeof(T))); new(data) T(value); count = capacity = 1; }
};
''']
    array = clean(source(game.parent / "Core/Inc/Array.h"))
    parts.append("template<class T> " + function(array, "class TArrayNoInit :") + ";")
    for name in ["FDamageEffectPair", "FItemActionSound"]:
        parts.append(function(generated, "struct " + name + "\n") + ";")
    parts.append(r'''
struct MMOFacade {
    FDamageEffectPair* match = nullptr;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    FDamageEffectPair* GetDamgeTemplateData(BYTE, BYTE, BYTE, BYTE) { return match; }
};
struct UGamePlayDataManager {
    TArray<FItemActionSound> ItemActionSounds;
    FDamageEffectPair GetDamageEffect(BYTE, BYTE, BYTE, BYTE);
    FItemActionSound GetItemActionSounds(BYTE);
};
''')
    gameplay = clean(source(game / "Src/UGamePlayDataManager.cpp"))
    for signature in ["struct FDamageEffectPair UGamePlayDataManager::GetDamageEffect(",
                      "FItemActionSound UGamePlayDataManager::GetItemActionSounds("]:
        parts.append(function(gameplay, signature))
    parts.append(r'''
void RecordTests() {
    UGamePlayDataManager manager;
    auto check_empty_sound = [](const FItemActionSound& v) {
        assert(!v.ItemType && !v.MousePickup.Num() && !v.MousePutdown.Num());
        assert(!v.Drop.Num() && !v.Use.Num() && !v.Get.Num());
    };
    for (BYTE key : {BYTE(0), BYTE(255)}) {
        auto damage = manager.GetDamageEffect(key, key, key, key);
        assert(!damage.ResultType && !damage.WeaponHitType && !damage.DirectionType && !damage.ElementaryType);
        assert(!damage.EffectList.Num());
        check_empty_sound(manager.GetItemActionSounds(key));
    }
    UAbilityComponent ability;
    FDamageEffectPair damage(EC_EventParm);
    damage.ResultType = 2; damage.WeaponHitType = 3; damage.DirectionType = 4; damage.ElementaryType = 5;
    damage.EffectList.SetOne(&ability);
    MMOFacade::getSingleton().match = &damage;
    {
        auto found = manager.GetDamageEffect(2, 3, 4, 5);
        assert(found.ResultType == 2 && found.WeaponHitType == 3 && found.DirectionType == 4 && found.ElementaryType == 5);
        assert(found.EffectList.Num() == 1 && found.EffectList(0) == &ability);
        assert(found.EffectList.data != damage.EffectList.data);
    }
    MMOFacade::getSingleton().match = nullptr;
    USoundCue cue;
    FItemActionSound sound(EC_EventParm); sound.ItemType = 7;
    sound.MousePickup.SetOne(&cue); sound.MousePutdown.SetOne(&cue);
    sound.Drop.SetOne(&cue); sound.Use.SetOne(&cue); sound.Get.SetOne(&cue);
    manager.ItemActionSounds.SetOne(sound);
    {
        auto found = manager.GetItemActionSounds(7);
        assert(found.ItemType == 7 && found.MousePickup(0) == &cue && found.MousePutdown(0) == &cue);
        assert(found.Drop(0) == &cue && found.Use(0) == &cue && found.Get(0) == &cue);
        assert(found.Get.data != manager.ItemActionSounds(0).Get.data);
    }
    check_empty_sound(manager.GetItemActionSounds(8));
    puts("Gameplay fallback records initialize every field and no-init array; successful record copies preserve data");
}
}
namespace ShopHeavenLookupTests {
using InventoryLookupTests::TArray;
struct ServerShopItemData { INT marker; };
struct ServerShopData { TArray<ServerShopItemData> _ShowItemList; };
struct ShopManager {
    ServerShopData shop;
    ServerShopData* FindShopData(INT id) const { return id == 7 ? const_cast<ServerShopData*>(&shop) : nullptr; }
    const ServerShopItemData* FindShopItem(INT, INT) const;
};
enum { sasbtEquip, sasbtBag, sasbtStall, ESPType_HeavenGasEquip, ESPType_HeavenGasBag, ESPType_HeavenGasBank };
struct UMMOGFXPlayer {};
struct Slot {
    INT unlocked = 0;
    void SetBool(const char*, UBOOL locked) { assert(!locked); ++unlocked; }
};
struct MMOHeavenManager {
    static MMOHeavenManager& getSingleton() { static MMOHeavenManager value; return value; }
    INT GetEquipContainerSize() const { return 3; }
    INT GetBagContainerSize() const { return 3; }
    INT GetBankContainerSize() const { return 3; }
};
struct UMMOUIHeavenGasMainPanel {
    TArray<Slot*> EquipGasItemSlotList, BagGasItemSlotList, BankGasItemSlotList;
    INT updates = 0, shows = 0, power = 0, last_slot = -1;
    Slot* last_object = nullptr;
    void UpdateContainerSlot(UMMOGFXPlayer*, Slot* slot, INT, INT, INT index) {
        ++updates; last_object = slot; last_slot = index;
    }
    void UpdateEquipSlotStateShow(UMMOGFXPlayer*, INT, INT index) { ++shows; last_slot = index; }
    void UpdateGasEquipPower() { ++power; }
    void UpdateSlotState(UMMOGFXPlayer*, INT, INT);
    void UpdateSlotItem(UMMOGFXPlayer*, INT, INT);
};
''')
    parts.append(function(clean(source(game / "Src/Shop.cpp")), "const ServerShopItemData* ShopManager::FindShopItem("))
    panel = clean(source(game / "Src/MMOUIHeavenGasMainPanel.cpp"))
    for signature in ["void UMMOUIHeavenGasMainPanel::UpdateSlotState(",
                      "void UMMOUIHeavenGasMainPanel::UpdateSlotItem("]:
        parts.append(function(panel, signature))
    parts.append(r'''
void Run() {
    ShopManager shop;
    assert(!shop.FindShopItem(7, 0));
    shop.shop._ShowItemList.values = {{10}, {20}};
    assert(shop.FindShopItem(7, 0)->marker == 10 && shop.FindShopItem(7, 1)->marker == 20);
    for (INT index : {(-2147483647 - 1), -1, 2, 2147483647}) assert(!shop.FindShopItem(7, index));
    assert(!shop.FindShopItem(8, 0));
    UMMOGFXPlayer player;
    UMMOUIHeavenGasMainPanel panel;
    Slot first, last;
    panel.EquipGasItemSlotList.values = {&first, nullptr, &last};
    panel.BagGasItemSlotList.values = {&first, nullptr, &last};
    panel.BankGasItemSlotList.values = {&first, nullptr, &last};
    for (INT container : {int(sasbtEquip), int(sasbtBag), int(sasbtStall), 999}) {
        for (INT index : {(-2147483647 - 1), -1, 3, 2147483647}) {
            panel.UpdateSlotItem(&player, container, index);
            panel.UpdateSlotState(&player, container, index);
        }
        panel.UpdateSlotItem(nullptr, container, 0);
        panel.UpdateSlotState(nullptr, container, 0);
    }
    assert(!panel.updates && !panel.shows && !panel.power && !first.unlocked && !last.unlocked);
    for (INT container : {int(sasbtEquip), int(sasbtBag), int(sasbtStall)}) {
        panel.UpdateSlotItem(&player, container, 0);
        assert(panel.last_object == &first && panel.last_slot == 0);
        panel.UpdateSlotItem(&player, container, 2);
        assert(panel.last_object == &last && panel.last_slot == 2);
        panel.UpdateSlotState(&player, container, 2);
    }
    assert(panel.updates == 6 && panel.shows == 3 && panel.power == 2);
    assert(first.unlocked == 3 && last.unlocked == 3);
    panel.EquipGasItemSlotList.values.clear();
    panel.BagGasItemSlotList.values.clear(); panel.BankGasItemSlotList.values.clear();
    for (INT container : {int(sasbtEquip), int(sasbtBag), int(sasbtStall)}) {
        panel.UpdateSlotItem(&player, container, 0); panel.UpdateSlotState(&player, container, 0);
    }
    assert(panel.updates == 6 && panel.shows == 3 && panel.power == 2);
    puts("Shop and Heaven UI reject negative/exact-end/extreme indices, empty arrays and missing UI; valid slots still update");
}
}
''')
    return "\n".join(parts)
