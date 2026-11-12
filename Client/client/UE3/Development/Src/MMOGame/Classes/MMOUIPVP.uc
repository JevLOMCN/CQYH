class MMOUIPVP extends MMOUIBase
	native(UI);

//Arena
var GFxObject ArenaPanel;
var array<GFxObject> ArenaMode;
var array<GFxObject> ArenaSize;
var const int 	ArenaModeSelCount;
var const int 	ArenaSizeSelCount;
//
event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	//
	//InitArena();
}

function InitArena()
{
	//local int i;
	//ArenaPanel = GetObject( "ArenaPanel_mc" );
	
	//ArenaMode.Add(ArenaModeSelCount);
	//for( i = 0 ; i < ArenaModeSelCount ; i++ )
	//{
	//	ArenaMode[i] = ArenaPanel.GetObject( "ArenaMode"$i );
	//}
	//ArenaMode[1].SetBool("enabled",false);
	//ArenaMode[2].SetBool("enabled",false);

	//ArenaSize.Add(ArenaSizeSelCount);
	//for( i = 0 ; i < ArenaSizeSelCount ; i++ )
	//{
	//	ArenaSize[i] = ArenaPanel.GetObject( "ArenaSize"$i );
	//}

	//ArenaSize[1].GetObject("pvpType_mc").GotoAndStopI(3);
	//ArenaSize[1].SetBool("enabled",false);
	//ArenaSize[2].GetObject("pvpType_mc").GotoAndStopI(5);
	//ArenaSize[2].SetBool("enabled",false);
	//ArenaSize[3].GetObject("pvpType_mc").GotoAndStopI(7);
	//ArenaSize[3].SetBool("enabled",false);


}

event CallSetPanelText(GFxObject data)
{
	ActionScriptVoid("SetPanelText");
}

native function NativeInitOnce();

//
native function ArenaEnter();

//===============================================================================================================
//===============================================================================================================
native function PostAdvance(float DeltaTime);
//===============================================================================================================
//===============================================================================================================

//
defaultproperties
{
	IsDynamicLoad=true
	ArenaModeSelCount=2
	ArenaSizeSelCount=2
}