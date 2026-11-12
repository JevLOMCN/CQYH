/*
 * 
 * 
 * */
class AbilityAnimationComponent extends AbilityComponent
	hidecategories(Object)
	editinlinenew
	native;


var()   array<name> Anims;
var()   float Rate;
var()   bool bLoop;
var()   float   LoopTime;
var()   EAnimLevel  AnimLevel;
var()   bool bForcePlay;
// start playing frame ( >= 0)
var()   int StartFrame;

var()   float BlendInTime;
// to define
var     float BlendOutTime;

var()   ERootBoneAxis RootBoneAxisX;
var()   ERootBoneAxis RootBoneAxisY;
var()   ERootBoneAxis RootBoneAxisZ;

var()   bool    bHiddenWeapon;

cpptext
{
	virtual FString GetResName();
	virtual FLOAT GetResLength(USkeletalMeshComponent* InSM);
	virtual void SetResLength(USkeletalMeshComponent* InSM, FLOAT InTime);
	virtual struct BaseComponent* CreateCplusplusNode();
}



DefaultProperties
{
	Rate=1.0
	bLoop=false
	AnimLevel=EAL_Action
	bForcePlay=true
	LoopTime=1.f
	LineColor=(R=0.196,G=0.196,B=1.0,A=1.0)
	TitleString="Anim"
	BlendInTime=0.1
	BlendOutTime=0.1
	bHiddenWeapon=false
}
