
// stop the Matinee animation currently playing in the scene, condition: the Matinee 'allow interrupt' property is set to TRUE in the tool
// after stopping, continue executing all output nodes

class SeqEvent_PlayerMatineeStop extends SequenceEvent
	native;

cpptext
{
	virtual void Activated();
}

DefaultProperties
{
    MaxTriggerCount = 0;

	ObjName="Player Matinee Stop 'ESC'"
	ObjCategory="Player"
}
