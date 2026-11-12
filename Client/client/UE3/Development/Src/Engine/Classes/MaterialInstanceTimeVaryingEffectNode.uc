/**
 * Material effect data node
 */
class MaterialInstanceTimeVaryingEffectNode extends Object
	native(Material)
	editinlinenew;


// Effect template ID
var() int EffectTemplateId;

// Effect description
var() string EffectDescribe;

var() array<ScalarParameterValueOverTime>		ScalarParameterValues;
var() array<VectorParameterValueOverTime>		VectorParameterValues;
var() array<LinearColorParameterValueOverTime>  LinearColorParameterValues;
var() array<TextureParameterValueOverTime>	    TextureParameterValues;

cpptext
{
	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
}


defaultproperties
{

}
