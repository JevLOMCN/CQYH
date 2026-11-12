//
// particle system binding info
//
class ParticleSystemAttachInfo extends Object
	editinlinenew
	hidecategories(Object);

// name used for identification
var() Name AttachName <DisplayName="AttachName">;

// used to create the particle effect and attach it
var() Name AttachSocket <DisplayName="AttachSocket"|DynamicList="SocketList">;

// particle effect template
var() ParticleSystem EmitterTemplate <DisplayName="EmitterTemplate">;

var() bool BindToWeapon;
var() EWeaponType WeaponType<EditCondition=BindToWeapon>;
var() float EffectScale;
var() name VertSurfaceActorParameterName;

// variable for temporarily storing objects
var ParticleSystemComponent EmitterInstance;

DefaultProperties
{
	BindToWeapon=FALSE
	EffectScale=1
	VertSurfaceActorParameterName=VertSurfaceActor
}
