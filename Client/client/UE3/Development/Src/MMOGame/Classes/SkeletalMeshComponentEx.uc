//
// reuse the waste
//
class SkeletalMeshComponentEx extends SkeletalMeshComponent
	native
	editinlinenew;
	//hidecategories(SkeletalMeshComponent)
	//dependson(SkeletalMeshComponent)
	//native;




////<- new anim 
//var transient AnimNodeBlendTwoSequences   IdleLoopBlend;
//var transient AnimNodeBlendTwoSequences   MoveLoopBlend;
//var transient AnimNodeBlendTwoSequences   ActionBlend;
////add by jimzhang
//var transient AnimNodeBlendTwoSequences   MoveTypeBlend;
//var transient AnimNodeBlendTwoSequences   MoveLoopBlendEx;

//var transient AnimNodeScalePlayRate       ActionAnimRateNode;
//var transient AnimNodeScalePlayRate       LoopAnimRateNode;
//var transient AnimNodeBlendPerBone        ActionLoopBlend;

//var transient AnimNodeSequence            IdleLoop1;
//var transient AnimNodeSequence            IdleLoop2;
//var transient AnimNodeSequence            MoveLoop1;
//var transient AnimNodeSequence            MoveLoop2;
//var transient AnimNodeSequence            Action1;
//var transient AnimNodeSequence            Action2;
////add by jimzhang
//var transient AnimNodeSequence            MoveLoop1Ex;
//var transient AnimNodeSequence            MoveLoop2Ex;


//event InitAnimNodes(MMOActor actor)
//{

//	IdleLoopBlend = AnimNodeBlendTwoSequences(FindAnimNode('IdleLoopBlend'));
//	MoveLoopBlend = AnimNodeBlendTwoSequences(FindAnimNode('MoveLoopBlend'));
//	ActionBlend = AnimNodeBlendTwoSequences(FindAnimNode('ActionBlend'));

//	MoveTypeBlend       = AnimNodeBlendTwoSequences(FindAnimNode('MoveTypeBlend'));
//	MoveLoopBlendEx = AnimNodeBlendTwoSequences(FindAnimNode('MoveLoopBlendEx'));

//	ActionAnimRateNode = AnimNodeScalePlayRate(FindAnimNode('ActionAnimRateNode'));
//	LoopAnimRateNode = AnimNodeScalePlayRate(FindAnimNode('LoopAnimRateNode'));
//	ActionLoopBlend = AnimNodeBlendPerBone(FindAnimNode('ActionLoopBlend'));
//	IdleLoop1 = AnimNodeSequence(FindAnimNode('IdleLoop1'));
//	IdleLoop2 = AnimNodeSequence(FindAnimNode('IdleLoop2'));
//	MoveLoop1 = AnimNodeSequence(FindAnimNode('MoveLoop1'));
//	MoveLoop2 = AnimNodeSequence(FindAnimNode('MoveLoop2'));
//	Action1 = AnimNodeSequence(FindAnimNode('Action1'));
//	Action2 = AnimNodeSequence(FindAnimNode('Action2'));

//	MoveLoop1Ex  = AnimNodeSequence(FindAnimNode('MoveLoop1Ex'));
//	MoveLoop2Ex  = AnimNodeSequence(FindAnimNode('MoveLoop2Ex'));



//	// todo fix
//	RefreshAnim(actor);
//}

//native function SetIdleAnim(name NewIdleLoopAnim);
//native function SetMoveAnim(name NewMoveLoopAnim);
//native function RefreshAnim(MMOActor actor);









////struct native MaterialNode
////{
////	var MaterialInterface   OriginalMaterial;
////	var MaterialInterface   TransparentOriginalMaterial;
////	var MaterialInstanceTimeVarying OpacityTimeVaringEffectInstance;
////	var MaterialInstanceTimeVarying TransparentTimeVaryingEffectInstance;
////};

////var array<MaterialNode> MeshMaterials;

////var int EffectId;

////// to check weather need to rebuild material list
////var SkeletalMesh LastSkeletalMesh;


////// init color
////var name InitVectorName;
////var Vector InitVectorValue;

////// opacity
////var name  OpactiyName;
////var float OpacityValue;


////native final function RebulidMeshMaterialList();
////native final function SwitchToTransparentOriginalMaterial(bool bTo);
////native final function SetVectorValue(name InVectorName, Vector InVector);
////native final function SetScaleValue(name InScaleName, float InScale);

////native final function SetEffectData(int InEffectId, MaterialInstanceTimeVaryingEffectNode InData);
////native final function CloseEffect(int InEffectId);

////native final function SetInitColor(name InColorName, Vector InColor);
////native final function SetOpacity(name InOpacityName, float InOpacity);

////native final function CopyMaterialInstanceParameters(Material DestMat, MaterialInstance DestMatInst, MaterialInstance SrcMatInst);


DefaultProperties
{
	bAcceptsDynamicDecals=false
	//EffectId=-1
}
