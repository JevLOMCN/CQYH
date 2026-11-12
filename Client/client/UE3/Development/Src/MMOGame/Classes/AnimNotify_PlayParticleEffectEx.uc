//
// notify to attach a model at a specific time
//
class AnimNotify_PlayParticleEffectEx extends AnimNotify_ScriptedEx
	;

var(MMO) ParticleSystem PSTemplate;

// attach point (Socket) used to create the particle effect, uses the character's current position if not specified
var(MMO) name SocketName <DynamicList="SocketList">;

// character facing does not affect particle facing
var(MMO) bool KeepZeroRotation;

// position offset when creating the particle (world coordinates)
var(MMO) Vector RelativeLocation;
var(MMO) Rotator RelativeRotation;

/** If this particle system should be attached to the location.**/
var(MMO) bool bAttach;

var EmitterSpawnable EmitterInstance;

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local Vector pos;
	local Rotator rot;
	local Matrix Trans;
	local ParticleSystemComponent psc;

	//`log("+++++++" @Owner);
	if( bAttach )
	{
		psc = new(self) class'ParticleSystemComponent';
		psc.SetTemplate( PSTemplate );
		psc.SetTranslation( RelativeLocation );
		psc.SetRotation(RelativeRotation);
		if( SocketName == '' )
		{
			Owner.AttachComponent( psc );
		}
		else
		{
			Pawn(Owner).Mesh.AttachComponentToSocket( psc, SocketName );
		}
		psc.ActivateSystem( );
	}
	else
	{
		GetWorldPosRot(Owner, SocketName, pos, rot);

		if(KeepZeroRotation)
		{
			rot = MakeRotator(0, 0, 0);
		}
		else
		{
			rot = Owner.Rotation;
		}

		Trans = MakeRotationMatrix(rot);

		pos += TransformVector(Trans, RelativeLocation);
		rot = MatrixGetRotator(MakeRotationMatrix(RelativeRotation)*Trans);

		//if((EmitterInstance == none)
		//	&& (PSTemplate != none))
		//{
		EmitterInstance = Owner.Spawn(class'EmitterSpawnable',,,
			pos, rot, ,true);
		//EmitterInstance.SetBase(Owner);

		//`log("+++++++ Spawn particle system object");
		//`log("+++++++ Active particle system" @PSTemplate);
		EmitterInstance.SetTemplate(PSTemplate);
		//}
	}
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	//`log("-------" @Owner);
	//if(EmitterInstance != none)
	//{
	//	EmitterInstance.ParticleSystemComponent.DeactivateSystem();
	//	EmitterInstance = none;
	//}
}


DefaultProperties
{
	KeepZeroRotation=true
}
