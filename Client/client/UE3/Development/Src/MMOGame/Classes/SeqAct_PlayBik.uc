
// play the specified video

class SeqAct_PlayBik extends SequenceAction
	native;

cpptext
{
	virtual void Activated();
	virtual UBOOL UpdateOp(FLOAT deltaTime);
}

var(MMO) name BikName;

//shan@snda
//var(MMO) bool bMute;
var(MMO) bool bPlayBGM;

var transient init string RealBinkMovieName;

DefaultProperties
{	
	ObjName="Play Bik"
	ObjCategory="MMO Client"
	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Play")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")
	

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")

	//shan@snda
	//bMute = false
	bPlayBGM = false
}
