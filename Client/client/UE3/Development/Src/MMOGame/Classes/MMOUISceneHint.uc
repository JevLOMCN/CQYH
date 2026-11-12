class MMOUISceneHint extends MMOUIBase
	native(UI);

//var MMOUIGFXPlayer _UIPlayer;

 struct native ActorHint
{
	var int PlayerNo;
	var int TempleteID;
	var GFxObject PlayerHeadText;
	//var GFxObject PlayerTalkBubble;
	var string   PlayerString;
	var bool     ForceUpdate;
	var int      UpdateType;
	var string   IconName;
	var int      ForceUpdateCountDown;
	var EActorClassType ActorType;   
	var bool     IsVisible;
	var float    CurAlpha;
	var int      CurScreenX;
	var int      CurScreenY;
	//var int TalkTime;
	var bool     IsQuestTrace;
};

var int HintID;
var bool bShutDown;
var bool bShowHintBtn;
var bool bRefresh;
var string playerNameColor;
var GFxObject confirmRemoveEnemy;

//cpptext
//{	
	
//	//TMap<INT, FActorHint> mapHint;
//}



//
native function OnShutDown();
native function NPCHpChange( NPC npc , int hp );
native function OtherPlayerHpChange( MMOPlayer pl ,int hp );
native function PlayerHPChange(int NewHP, int MaxHP);
native function OtherPlayerTitleChange( MMOPlayer pl,int TitleID );
native function PlayerTitleChange( int TitleID ); 
native function PlayerPkValueChange( int PkValue );
native function OtherPlayerPkValueChange( MMOPlayer pl,int PkValue );
native function AddNewNpcData( NPC npc );
native function ClearUI();
native function EnterScene();

//new
native function RefleshHintsByTemp(int nTempleteID);
native function RefleshHints(int nServerID,optional ESceneHintUpdate updateType = ESHU_ALL);
native function RefleshAllHints();
native function RefleshAllNPCHints();
native function UpdateHints_Pos(float DeltaTime);
native function UpdateHints_Player(MMOPlayer pl, LocalPlayer lp,bool Force, float maxViewDist);
native function UpdateHints_NPC(NPC pl, LocalPlayer lp,bool Force, float maxViewDist);
native function ShowHintBtn(bool bShow);
native function bool CreateNpcHint(NPC npc);
native function UpdateHp_Player(GFxObject HeadName, MMOPlayer pl);
native function UpdateHp_NPC(GFxObject HeadName, NPC pl);
native function UpdateGreyName_Player(GFxObject HeadName, MMOPlayer pl);


native function ChangeScene();
native function AddActorTalkBubble( int nServerID, string strWords );
native function CloseActorTalkBubble( int nServerId );
native function CloseAllTalkBubble();
native function TestNpcTalk( string strWords );
native function SetIconName( NPC npc, string strName );

final native function UpdatePlayerTitle(bool main_actor);
native function string GetPlayerNameFrame(MMOPlayer pl);
native function string GetPlayerNameColor(MMOPlayer pl);
native function string GetPlayerGuildFrame(string strGuildName);
//native function int GetCroupId(MMOActor actor);

native function ClickRemoveEnemyConfirmOK();
native function ClickRemoveEnemyConfirmCancel();

native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

//-------------------------------------------------------
native function UpdateHintTextByID(int ServerID ,ESceneHintUpdate type=ESHU_ALL);
native function UpdateHintText(MMOActor pl,ESceneHintUpdate type=ESHU_ALL);
native function UpdateHintPos(float DeltaTime);
native function ASRightClickTargetInfo(int x,int y,int serverID);
native function BeforeViewUpdate( float DeltaTime );
native function ASRightClickNpcInfo(int x,int y,int serverID);
//------------------------------------------------------------------------

//-------------------------------------------------------
native function GFxObject CreateHintFromPool();
native function RecycleHintToPool(GFxObject HeadName);
native function InitHintPool();
native function UnintHintPool();
//-------------------------------------------------------

native function ASClickStallInfo(string playerName, string stallName);

event SceneHintRemoveChild( GFxObject tChild )
{
	ActionScriptVoid("removeChild");
}

event SetSelectButton(GFxObject target, bool visible)
{
	target.ActionScriptVoid("SetSelectButton");
}

event SetPlayerTxt_Name(GFxObject target, string Frame, string PlayerName,string nameColor)
{
	target.ActionScriptVoid("SetPlayerTxt_Name");
}

event SetPlayerTxt_Guild(GFxObject target, string Frame, string GuildName)
{
	target.ActionScriptVoid("SetPlayerTxt_Guild");
}

event SetPlayerTxt_Title(GFxObject target, string Frame, string TitleName, string TitleIcon)
{
	target.ActionScriptVoid("SetPlayerTxt_Title");
}

event SetPlayerTxt_Status(GFxObject target, string Frame, string StatusName)
{
	target.ActionScriptVoid("SetPlayerTxt_Status");
}

event SetPlayerTxt_Group(GFxObject target, int PkValue,   int GroupType)
{
	target.ActionScriptVoid("SetPlayerTxt_Group");
}

event UpdateDisplay(GFxObject target)
{
	target.ActionScriptVoid("UpdateDisplay");
}



event SetPlayerTxt(GFxObject target, int PkValue,   int GroupType, string strName,  string Guild, string Title ,string TitleIcon ,string Master)
{
	target.ActionScriptVoid("SetPlayerTxt");
}

event SetNpcTxt(GFxObject target, int NpcType, bool Attackeble, string strName,  string Status, string FrameExName,string nameColor)
{
	target.ActionScriptVoid("SetNpcTxt");
}

event SetMonsterTxt(GFxObject target, int MonsterType, string strName, string Status, string icon, string FrameExName,string nameColor)
{
	target.ActionScriptVoid("SetMonsterTxt");
}

event SetBossTxt(GFxObject target,  string strName,  string Status )
{
	target.ActionScriptVoid("SetBossTxt");
}

event SetStallTxtInfo( GFxObject target, string playerName, string stallName, bool attention )
{
	target.ActionScriptVoid("SetStallTxtInfo");
}

event SetIconInfo( GFxObject target, string IconName1, string IconName2 )
{
	target.ActionScriptVoid("SetIconInfo");
}

event SetSabacState( GFxObject target, bool win )
{
	target.ActionScriptVoid("setShabake");
}

event RefreshIconInfo( GFxObject target)
{
	target.ActionScriptVoid("RefreshIconInfo");
}

event SetHpInfo( GFxObject target, int Hp, int HpMax, int nState, int VisibleType ) //VisibleType, 0 hidden, 1 shown, 2 show only the HP bar without numbers
{
	target.ActionScriptVoid("SetHpInfo");
}

event SetBubbleText( GFxObject target, string bubbleText)
{
	target.ActionScriptVoid("SetBubbleText");
}

event SetBubbleTextEx(GFxObject target, string bubbleText)
{
	target.ActionScriptVoid("SetBubbleTextEx");
}

event CloseBubbleText(GFxObject target)
{
	target.ActionScriptVoid("CloseBubbleText");
}

event SetVipState(GFxObject target,int vipType)
{
	target.ActionScriptVoid("setVipStatus");
}

event setEnemy(GFxObject target,bool bEnemy)
{
	target.ActionScriptVoid("setEnemy");
}

event SetSabacStatueName(GFxObject target, string playerName, string guildName, bool visible)
{
	target.ActionScriptVoid("SetGuildLeader");
}

event UCSetAllVisible(GFxObject target, bool visible)
{
	target.ActionScriptVoid("UCSetAllVisible");
}

DefaultProperties
{
	DefaultVisible =true
	HintID=0
	//UpdateSelf=true
	//PlayerID=0
	CloseOnEsc=false
	bShutDown = false;
	bShowHintBtn = false;
}
