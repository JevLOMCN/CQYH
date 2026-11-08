//
// 1.05 backport: "equip mesh exchange" ability component (used by
// ItemBuff/ItemAvatar.upk, couture transform buffs 44003 NeZha and 44007
// HuangQuan: temporarily swaps the character's equip appearance).
//
// RECOVERED from 1.05 archetypes (exports
// "44003_NeZha_BianShen/44007_HuangQuan_BianShen.AbilityEquipMeshExchangeComponent_0"):
//     EquipMeshExchangeInfoArr : ArrayProperty of struct, each element is a
//         tagged struct with two StrProperty fields:
//             ExchangeMaterialPath  (empty in all data)
//             ExchangeEquipAppPath  e.g. "EquipAppearence'Couture_Set.Couture_body_02_battle'"
//     StopType = ACBT_StopWhenAbilityEnd, Duration = 0.2 (inherited)
// The struct name is NOT stored in the packages (elements of a tagged array
// carry no struct name); "EquipMeshExchangeInfo" is a GUESS. The struct is
// non-native (elements are tagged-serialized in the data).
//
// GUESSED behaviour: while active, the actor's equip appearances are
// replaced by the EquipAppearence assets listed, optionally with a material.
// This class is script-only and has NO C++ node (CreateCplusplusNode returns
// 0 in the base class, so the ability processor ignores it): data only.
// To implement the effect, add a C++ BaseComponent (OnAction(bOn) applying /
// restoring the listed EquipAppearence on the base actor) and override
// CreateCplusplusNode.
//
class AbilityEquipMeshExchangeComponent extends AbilityComponent
	editinlinenew
	hidecategories(Object);

struct EquipMeshExchangeInfo
{
	// optional material object path to apply ("" in all 1.05 data)
	var() string ExchangeMaterialPath;
	// full object path of the replacement EquipAppearence
	var() string ExchangeEquipAppPath;
};

var() array<EquipMeshExchangeInfo> EquipMeshExchangeInfoArr;

DefaultProperties
{
	LineColor=(R=1.0,G=0.5,B=1.0,A=1.0)
	TitleString="EquipMeshExchange"
	bHaveTrail=true
}