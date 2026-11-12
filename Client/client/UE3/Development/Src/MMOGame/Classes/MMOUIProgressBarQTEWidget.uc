class MMOUIProgressBarQTEWidget extends MMOUIBase
	native(UI);

var int m_nActorID;
var int m_nAbilityID;

event Open(int time,string keyName,string imgName,int nActorID,int nAbilityID)
{
	local array<ASValue> args;
	local ASValue asval;
	NativeShow(true);
	m_nActorID = nActorID;
	m_nAbilityID = nAbilityID;
	asval.Type = AS_Number;
	asval.n = time;
	args[0] = asval;
	asval.Type = AS_String;
	asval.s = keyName;
	args[1] = asval;
	asval.Type = AS_String;
	asval.s = imgName;
	args[2] = asval;
	SetPosition( -200,-200 );//image center
	Invoke("ShowQTE",args);
	IsVisible = GetBool("visible");
}

event Close(int closeType)
{
	local array<ASValue> args;
	local ASValue asval;
	asval.Type = AS_Number;
	asval.n = closeType;
	args[0] = asval;
	Invoke("CloseQTE",args);
	IsVisible = GetBool("visible");
}

native function UpdateQTE( Canvas cvs );
native function int GetTargetActorID();
native function int GetTargetAbilityID();

native function bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

native function ShowProgressBarQTE( int time,string keyName,string imgName,int nActorID,int nAbilityID );
native function CloseProgressBarQTE(int closeType);

native function ASClickQTE();
native function PostAdvance(float DeltaTime);

DefaultProperties
{
	m_nActorID=0
	m_nAbilityID=0
	DefaultVisible = false
	CloseOnEsc=false
}
