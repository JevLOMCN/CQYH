/*
 * 装备类,功能包含服装,装甲,武器等可视物件
 * 支持换静态网络模型,骨骼蒙皮,粒子,贴图四种可视对象
 * 
 * 对于非贴图的物品,模型以Component的方式组合在一起, 有两种,一种是添加到Actor的Components数组里去,另一种是绑定到BAse骨架的一个socket上去
 *
 * 对于SkeletalMesh,动画有两种,一种是使用base的动画,另一种是使用独立的动画,后者需要配置动画树,动画集,播放节点信息.
 * 
 * 
 * 
 */
class EquipAppearence extends Object
	native
	hidecategories(Object);

var() string ConfigDisplayName <DisplayName="ConfigDisplayName">;

// 如果身体分块,那么EET_Default这个标记就不要用了;如果不分块,只用EET_Default标记,其它部分不要使用
var()	EEquipType			EquipType					<DisplayName="EquipType">;
// 武器类型,作为装备子类型,如果非武器,这个字段无意义
var transient editconst bool ShowWeaponType;
var()   EWeaponType         WeaponType					<EditCondition=ShowWeaponType|DisplayName="WeaponType">;

var() EWeaponAppearenceType WeaponAppearenceType;

var(Common) editinline array<EquipMesh>	MeshListTemplate_M	<DisplayName="MeshListTemplate_M">;
var(Common) editinline array<EquipMesh>	RandomMeshListTemplate_M;

var(Common) editinline array<EquipMesh>	MeshListTemplate_F	<DisplayName="MeshListTemplate_F">;
var(Common) editinline array<EquipMesh>	RandomMeshListTemplate_F;

// 新换装机制不需要EquipMesh数组 一个部位只会有一个EquipMesh
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

// 为了给战士加两面蛋疼的旗子
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
