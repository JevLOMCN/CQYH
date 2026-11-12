class MMOUIProgressBarWidget extends MMOUIBase
	native(UI);

delegate DelegateTimeUp();
//var delegate<OnCountDownOver> OnCountDownOver;

event Open(int countDownTime,string strTitle, int type, optional delegate<DelegateTimeUp> OnOver = none)
{
	local array<ASValue> args;
	local ASValue asval;

	DelegateTimeUp = OnOver;
	NativeShow(true);
	asval.Type = AS_Number;
	asval.n = countDownTime;
	args[0] = asval;
	asval.Type = AS_String;
	asval.s = strTitle;
	args[1] = asval;
	asval.Type = AS_Number;
	asval.n = type;
	args[2] = asval;
	Invoke("ShowProcesses",args);
	`log("ProgressBar showed strTitle:"$strTitle$",DelegateTimeUp"$DelegateTimeUp);
}

event Close(int closeType)
{
	local array<ASValue> args;
	local ASValue asval;
	asval.Type = AS_Number;
	asval.n = closeType;
	args[0] = asval;
	Invoke("CloseProcesses",args);
}

native function bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

//
function ProgressBarTimeUpCallback()
{
	`log("ProgressBar TimeUp Callback called"$DelegateTimeUp);
	if( DelegateTimeUp == none ){
		return;
	}
	DelegateTimeUp();
	DelegateTimeUp = none;
}

native function ShowProgressBar( int countDownTime,string strTitle, int type );
native function CloseProgressBar(int closeType);
//DelegateTimeUp needs to be separated
//function FBSummaryProgressBarTimeUpCallback()
//{
//	`log("FBSummaryProgressBar TimeUp Callback called"$DelegateTimeUp);
//	if( DelegateTimeUp == none ){
//		return;
//	}
//	DelegateTimeUp();
//	DelegateTimeUp = none;
//}

DefaultProperties
{
	DefaultVisible = false
	CloseOnEsc=false
}
