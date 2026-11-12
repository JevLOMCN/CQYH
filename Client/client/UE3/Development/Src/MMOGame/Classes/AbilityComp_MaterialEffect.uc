/*
 * MMOACTOR type character, material color modifier, translucency modifier component
 * 
 * */
class AbilityComp_MaterialEffect extends AbilityComponent
	dependson(TypeDefine)
	editinlinenew
	native;

var(Common) ESourceType SourceType;
var(Common) EMaterialEffectType EffectType;
var(ChangeMaterialProperty) int EffectTemplateId;
var(ChangeMaterialEffect) MaterialInstance EffectMaterialInstance;
var(ChangeMaterial) bool CanBeDeleteWhenSetNewMaterial;
var(ChangeMaterial) int Priority;



var transient float LeftTime;

cpptext
{
	virtual FString GetResName();
	virtual struct BaseComponent* CreateCplusplusNode();
}



DefaultProperties
{
	CanBeDeleteWhenSetNewMaterial=true
	Priority=0
}
