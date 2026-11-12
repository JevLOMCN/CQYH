/*
 * equipment class, functions include clothing, armor, weapons and other visible objects
 * supports four kinds of visible objects: swapped static mesh, skeletal skinning, particle, texture
 * 
 * for non-texture items, models are combined as Components in two ways: one is added to the Actor's Components array, the other is attached to a socket of the Base skeleton
 *
 * for SkeletalMesh there are two kinds of animation: one uses the base animation, the other uses independent animation, which requires configuring the animation tree, animation set, and playback node info.
 * 
 * 
 * 
 */
class EquipAppearence extends Object
	native
	hidecategories(Object);

var() string ConfigDisplayName <DisplayName="ConfigDisplayName">;

// if the body is split into parts, do not use the EET_Default flag; if not split, only use the EET_Default flag and do not use the other parts
var()	EEquipType			EquipType					<DisplayName="EquipType">;
// weapon type, as an equipment subtype; meaningless if not a weapon
var transient editconst bool ShowWeaponType;
var()   EWeaponType         WeaponType					<EditCondition=ShowWeaponType|DisplayName="WeaponType">;

var() EWeaponAppearenceType WeaponAppearenceType;

var(Common) editinline array<EquipMesh>	MeshListTemplate_M	<DisplayName="MeshListTemplate_M">;
var(Common) editinline array<EquipMesh>	RandomMeshListTemplate_M;

var(Common) editinline array<EquipMesh>	MeshListTemplate_F	<DisplayName="MeshListTemplate_F">;
var(Common) editinline array<EquipMesh>	RandomMeshListTemplate_F;

// the new outfit swap mechanism does not need the EquipMesh array, one slot has only one EquipMesh
var(WA) editinline /*array<EquipMesh>*/EquipMesh		MeshList_WA_M	<DisplayName="MeshList_WA_M">;
var(WA) editinline /*array<EquipMesh>*/EquipMesh		MeshList_WA_F	<DisplayName="MeshList_WA_F">;

var(MA) editinline /*array<EquipMesh>*/EquipMesh		MeshList_MA_M	<DisplayName="MeshList_MA_M">;
var(MA) editinline /*array<EquipMesh>*/EquipMesh		MeshList_MA_F	<DisplayName="MeshList_MA_F">;

var(TA) editinline /*array<EquipMesh>*/EquipMesh		MeshList_TA_M	<DisplayName="MeshList_TA_M">;
var(TA)	editinline /*array<EquipMesh>*/EquipMesh		MeshList_TA_F	<DisplayName="MeshList_TA_F">;
var(AG) editinline /*array<EquipMesh>*/EquipMesh		MeshList_AG_M	<DisplayName="MeshList_AG_M">;
var(AG) editinline /*array<EquipMesh>*/EquipMesh		MeshList_AG_F	<DisplayName="MeshList_AG_F">;
var(AR) editinline /*array<EquipMesh>*/EquipMesh		MeshList_AR_M	<DisplayName="MeshList_AR_M">;
var(AR) editinline /*array<EquipMesh>*/EquipMesh		MeshList_AR_F	<DisplayName="MeshList_AR_F">;
var(SP) editinline /*array<EquipMesh>*/EquipMesh		MeshList_SP_M	<DisplayName="MeshList_SP_M">;
var(SP) editinline /*array<EquipMesh>*/EquipMesh		MeshList_SP_F	<DisplayName="MeshList_SP_F">;
var(YS) editinline /*array<EquipMesh>*/EquipMesh		MeshList_YS_M	<DisplayName="MeshList_YS_M">;
var(YS) editinline /*array<EquipMesh>*/EquipMesh		MeshList_YS_F	<DisplayName="MeshList_YS_F">;

// to add two annoying flags to the warrior
var(WA) editinline array<EquipMesh>		AdditionalMeshList_WA_M	<DisplayName="AdditionalMeshes_WA_M">;
var(WA) editinline array<EquipMesh>		AdditionalMeshList_WA_F	<DisplayName="AdditionalMeshes_WA_F">;
var(MA) editinline array<EquipMesh>		AdditionalMeshList_MA_M	<DisplayName="AdditionalMeshes_MA_M">;
var(MA) editinline array<EquipMesh>		AdditionalMeshList_MA_F	<DisplayName="AdditionalMeshes_MA_F">;
var(TA) editinline array<EquipMesh>		AdditionalMeshList_TA_M	<DisplayName="AdditionalMeshes_TA_M">;
var(TA) editinline array<EquipMesh>		AdditionalMeshList_TA_F	<DisplayName="AdditionalMeshes_TA_F">;
var(AG) editinline array<EquipMesh>		AdditionalMeshList_AG_M	<DisplayName="AdditionalMeshes_AG_M">;
var(AG) editinline array<EquipMesh>		AdditionalMeshList_AG_F	<DisplayName="AdditionalMeshes_AG_F">;
var(AR) editinline array<EquipMesh>		AdditionalMeshList_AR_M	<DisplayName="AdditionalMeshes_AR_M">;
var(AR) editinline array<EquipMesh>		AdditionalMeshList_AR_F	<DisplayName="AdditionalMeshes_AR_F">;
var(SP) editinline array<EquipMesh>		AdditionalMeshList_SP_M	<DisplayName="AdditionalMeshes_SP_M">;
var(SP) editinline array<EquipMesh>		AdditionalMeshList_SP_F	<DisplayName="AdditionalMeshes_SP_F">;
var(YS) editinline array<EquipMesh>		AdditionalMeshList_YS_M	<DisplayName="AdditionalMeshes_YS_M">;
var(YS) editinline array<EquipMesh>		AdditionalMeshList_YS_F	<DisplayName="AdditionalMeshes_YS_F">;

DefaultProperties
{
	ShowWeaponType=true
	EquipType=EET_Total_Count
}
