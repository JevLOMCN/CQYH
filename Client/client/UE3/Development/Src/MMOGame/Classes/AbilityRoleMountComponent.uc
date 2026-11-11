//
// 1.05 backport: "role mount" ability component (used by CQ_Skill_Spear_1.upk,
// the Lancer dragon-horse buffs 12120/12121/12122).
//
// RECOVERED from 1.05 archetypes (tagged properties of the three exports
// "12120_Horse_0/12121_Horse_1/12122_Horse_2.AbilityRoleMountComponent_0"):
//     MountId  : IntProperty  = 100 / 101 / 102
//     StopType : ByteProperty = ACBT_StopWhenAbilityEnd   (inherited)
// No other property is serialized, so the class has exactly one own variable.
//
// GUESSED: MountId is a mount template id that the owning actor should
// ride while the component is active (MMOActor.EquipMount(int MountID) /
// UnEquipMount() already exist natively). This class is script-only and
// has NO C++ node: AbilityComponent::CreateCplusplusNode() returns 0, so the
// ability processor silently skips it. It only exists so that the 1.05
// packages load without a missing-class error. To make it ride, add a
// BaseComponent (OnAction(bOn) -> BaseActor->EquipMount(MountId) /
// UnEquipMount()) and a CreateCplusplusNode override in C++.
//
class AbilityRoleMountComponent extends AbilityComponent
	editinlinenew
	hidecategories(Object);

// mount template id (100..102 in 1.05 data)
var() int MountId;

DefaultProperties
{
	LineColor=(R=0.5,G=1.0,B=0.5,A=1.0)
	TitleString="RoleMount"
	bHaveTrail=true
}