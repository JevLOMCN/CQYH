class AnimNodeBlendTwoSequences extends AnimNodeBlend
	native;

var int     ActiveChildIdx;

// fangruyi to uniquely identify an animation (the animation name alone cannot tell whether it is the same animation)
var int     ActiveAnimKey;

var byte    AnimLevel;

native function float PlayAnimation( name animName, optional int Key = 0,  optional float rate = 1.f, 
	optional float blendInTime = 0.25f, optional float blendOutTime = 0.25f, 
	optional bool loop = false, 
	optional ERootBoneAxis rba_x = RBA_Default,
	optional ERootBoneAxis rba_y = RBA_Default,
	optional ERootBoneAxis rba_z = RBA_Default,
	optional bool noAnimEnd = false,
	optional float Position = 0.f,
	optional byte InAnimLevel = 1
	);

// returns TRUE when the given animation is actually playing and is stopped
native function bool StopAnimationByName(name InAnim);
native function bool StopAnimationByKey(int InKey);
native function ReplayAnim();
native function bool IsPlayingAnimationByName(name InAnim);
native function bool IsPlayingAnimationByKey(int InKey);
native function bool IsPlayAnyAnimation();
native function SetAnimRate(int InKey, float InRate);
native function bool GetCurrentAnimInfo(out int OutActiveAnimKey, out byte OutAnimLevel, out float OutPercent, out name OutAnimName);


function StopAllAnimation()
{
	local int i;
	local AnimNodeSequence child;
	for( i = 0; i < 2; ++i )
	{
		child = AnimNodeSequence( Children[i].Anim );
		if( child != none )
		{
			child.StopAnim();
		}
	}
	ActiveChildIdx = -1;
	ActiveAnimKey = 0;
}

function ShowPlayStatus()
{
	local int i;
	local AnimNodeSequence child;
	if( !IsPlayAnyAnimation() )
	{
		`log( "Not Playing Animation" );
		return;
	}
	for( i = 0; i < 2; ++i )
	{
		child = AnimNodeSequence( Children[i].Anim );
		if( child != none )
		{
			if( child.bPlaying )
				`log( "ActionBlend::Children["@i @"]:" @child.AnimSeqName @"Is Playing");
			else
				`log( "ActionBlend::Children["@i @"]:" @child.AnimSeqName @"not Playing");
		}
	}		
}

event Dump()
{
	local int i;
	local AnimNodeSequence child;
	for(i = 0; i < 2; ++i)
	{
		child = AnimNodeSequence( Children[i].Anim );
		if(child != none)
		{
			`log("Index="$i$"  Anim="$child.AnimSeqName$"  bPlaying="$child.bPlaying);
		}
		else
		{
			`log("Index="$i$"  NoAnim");
		}
	}

	`log("Active Idx = "$ActiveChildIdx);
}

/** Get notification that this node has become relevant for the final blend. ie TotalWeight is now > 0 */
event OnBecomeRelevant()
{
	super.OnBecomeRelevant();

}
/** Get notification that this node is no longer relevant for the final blend. ie TotalWeight is now == 0 */
event OnCeaseRelevant()
{
	local int i;
	local AnimNodeSequence child;
	super.OnCeaseRelevant();
	for(i = 0; i < 2; ++i)
	{
		child = AnimNodeSequence( Children[i].Anim );
		if(child != none && child.bPlaying)
		{
			child.StopAnim();
		}
	}

}

DefaultProperties
{
	bFixNumChildren=true
	ActiveChildIdx=-1
}
