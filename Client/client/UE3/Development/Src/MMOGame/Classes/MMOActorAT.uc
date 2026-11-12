
//
// Control of the game character animation tree, used for all character objects in the game
//

class MMOActorAT extends Object
	dependson(TypeDefine)
	dependson(ActorAttribute)
	dependson(AvatarData)
	native;


//<- new anim 
var transient AnimNodeBlendTwoSequences   IdleLoopBlend;
var transient AnimNodeBlendTwoSequences   MoveLoopBlend;
var transient AnimNodeBlendTwoSequences   ActionBlend;
//add by jimzhang
var transient AnimNodeBlendTwoSequences   MoveTypeBlend;
var transient AnimNodeBlendTwoSequences   MoveLoopBlendEx;

var transient AnimNodeScalePlayRate       ActionAnimRateNode;
var transient AnimNodeScalePlayRate       LoopAnimRateNode;
var transient AnimNodeBlendPerBone        ActionLoopBlend;

var transient AnimNodeSequence            IdleLoop1;
var transient AnimNodeSequence            IdleLoop2;
var transient AnimNodeSequence            MoveLoop1;
var transient AnimNodeSequence            MoveLoop2;
var transient AnimNodeSequence            Action1;
var transient AnimNodeSequence            Action2;
//add by jimzhang
var transient AnimNodeSequence            MoveLoop1Ex;
var transient AnimNodeSequence            MoveLoop2Ex;


var float AnimBlendInTime;
var float AnimBlendOutTime;
var float ActionPlayedLeftTime;
var transient SkeletalMeshComponent	Mesh;
var transient MMOActor actor;

native function InitAnim(SkeletalMeshComponent InMesh,MMOActor InActor);
native function RefreshMoveAnim();

native function SetIdleAnim(name NewIdleLoopAnim);
native function SetMoveAnim(name NewMoveLoopAnim);
native final function name GetIdleAnim();
native final function name GetMoveAnim();

native function SetAnimRateById(int InId, float InRate);

native function SetActionRate(float InRate);
native function SetMoveAnimRate(float InRate);
native function StopAnimation();
native function SetMoveAnimPosition(float InPos);
native function float GetMoveAnimPosition();

native function StopAnimationByName(name InAnim);
native function StopAnimationByKey(int InKey);
native function BlendToIdle();
native function StopLoopAnim(name InAnim);
native function BlendToLoops();

native function SetActionLoopBlendTarget(float BlendTarget, float BlendTime);


native function bool IsPlayingAnimationByName(name InAnim);
native function bool IsPlayingAnimationByKey(int InKey);
native function bool IsPlayingAnyAnimation();
native function bool GetCurrentAnimInfo(out int OutActiveAnimKey, out byte OutAnimLevel, out float OutPercent, out name OutAnimName);



native function float PlayAnimation(name Anim, optional int InKey = 0, optional float Rate = 1.0f, optional bool bLoop = false, optional ERootBoneAxis RootBoneAxisX = RBA_Discard,optional ERootBoneAxis RootBoneAxisY = RBA_Discard,optional ERootBoneAxis RootBoneAxisZ = RBA_Discard, optional EAnimBlendType BlendType = EABT_FullBody, optional float Position = 0.f, optional float InBlendIn=0.1, optional float InBlendOut=0.1, optional EAnimLevel InAnimLevel = EAL_Action);
native function OnAnimEnd(AnimNodeSequence SeqNode, float PlayedTime, float ExcessTime);


defaultproperties
{
	AnimBlendInTime     = 0.15;
	AnimBlendOutTime    = 0.15;
}
