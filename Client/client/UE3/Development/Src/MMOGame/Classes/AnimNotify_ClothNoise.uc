//
// animation notify extension class
//
class AnimNotify_ClothNoise extends AnimNotify_Scripted
	dependson(TypeDefine)
	native;


var() ENoiseActionType ActionType;

var(Preview) SoundCue PreviewSound; 

cpptext
{
	// AnimNotify interface.
	virtual void Notify( class UAnimNodeSequence* NodeSeq );
	//virtual void NotifyEnd( class UAnimNodeSequence* NodeSeq, FLOAT AnimCurrentTime );
}

//event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
//{
//	local MMOActor mmoOwner;
//	mmoOwner = MMOActor(Owner);
//	if(mmoOwner != none)
//	{
//		mmoOwner.PlayClothNoise();
//	}
//}