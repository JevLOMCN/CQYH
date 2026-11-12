/**
 * Material effect list data class
 */
class MaterialInstanceTimeVaryingEffect extends Object
	native(Material);

var() editinline array<MaterialInstanceTimeVaryingEffectNode> TimeVaryingEffectList;

var transient MaterialInstanceTimeVaryingEffectNode BackupData;

var transient MaterialInstanceTimeVarying SourceInstance;

var transient int CurrentActiveEffectIndex;

cpptext
{
	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

		/** Copies the parameter array values back to the source instance. */
	void CopyToSourceInstance();

	/** 
	 * Sets the source instance for this object and regenerates arrays. 
	 *
	 * @param MaterialInterface		Instance to use as the source for this material editor instance.
	 */
	void SetSourceInstance(UMaterialInstanceTimeVarying* MaterialInterface);

	void RefreshSourceInstance();
}


defaultproperties
{

}
