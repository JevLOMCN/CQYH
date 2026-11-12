class MMOGFXPlayer_HudInfo extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUITargetInfo	        TargetInfo;
var MMOUIMyInfo	        PlayerInfo;
var MMOUIHeroInfo	        HeroInfo;
var MMOUITeamMate	        TeamMate;
var MMOUITeamHint           TeamHint;

function RegisterTargetInfo( MMOUITargetInfo mc )
{	
	TargetInfo = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_HudInfo_TargetInfo );

	//self.OnPostAdvance = TargetInfo.TickForTestInteractNPCDistance;
}

function RegisterPlayerInfo( MMOUIMyInfo mc )
{	
	PlayerInfo = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_HudInfo_MyInfo );
}

function RegisterHeroInfo( MMOUIHeroInfo mc )
{	
	HeroInfo = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_HudInfo_HeroInfo );
}


function RegisterTeamMate( MMOUITeamMate mc )
{
	TeamMate = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_HudInfo_TeamMate );
}

function RegisterTeamHint( MMOUITeamHint mc )
{
	TeamHint = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_HudInfo_TeamHint );
}

native function ASMyInfoRightClick( int x , int y );//moved

native function ASRightClickTargetInfo( int x , int y , String RoleName );//moved

native function ASSetPkMode(int PkMode);//moved

native function ASPkModeChange();//moved

native function ASTeamHintLeave();//moved

native function ASTeamExpand(bool v);//moved

final native function ASTeamMemberRightClick( int x , int y , int index );//moved

//
final native function ASVIPRightClick();//moved
final native function ASVIPShowTips( GFxObject hintMc );//moved
final native function ASVIPHideTips();//moved
//
final native function ASShowTeammateHint( GFXObject mc, int index );//moved
final native function ASHideTeammateHint();//moved

event HeroUpdateData()
{
	//`log("------------------------------HeroUpdateData");
	HeroInfo.HeroUpdateData();
}

//moved
event AddFunctionBuffPanel(int buffSlotID,string buffIcon,int buffMaxTime,int buffCurTime, int stackCount, int ownerID,int buffID,int showTimeUp,bool debuff)
{
	//`log("AddFunctionBuffPanel+"$buffIcon);
	PlayerInfo.AddBuff( buffSlotID, buffIcon,  buffMaxTime, buffCurTime,stackCount,ownerID,buffID,showTimeUp, debuff);
}
//moved
event DeleteFunctionBuffPanel(int buffSlotID)
{
	PlayerInfo.DeleteBuff( buffSlotID );
}
//moved
event AddTargetBuffPanel(int buffSlotID,string buffIcon,int buffMaxTime,int buffCurTime, int stackCount, int ownerID,int buffID,int showTimeUp,bool debuff)
{
	//`log("AddTargetBuffPanel+"$buffIcon);
	TargetInfo.AddBuff( buffSlotID, buffIcon,  buffMaxTime, buffCurTime,stackCount,ownerID,buffID,showTimeUp, debuff);
}
//moved
event DeleteTargetBuffPanel(int buffSlotID)
{
	TargetInfo.DeleteBuff( buffSlotID );
}

//Unused
function SetHp( int ComboCount)
{
	ActionScriptVoid("root.SetCombo");
}

native function ASHeroUncall();//moved
native function ASHeroBag();//moved
native function FunMode(int Mode);//moved

native function PostAdvance(float DeltaTime );

event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	local bool res;
	if(strHoverControllerName=="")
	{
		`log("----------------strHoverControllerName==null");
		return false;
	}

	res = PlayerInfo.SkipCaptureInputKey( key,  eIE, strHoverControllerName, ClickPosInMovieSpace);
	if(res==false)return false;

	res = HeroInfo.SkipCaptureInputKey( key,  eIE, strHoverControllerName, ClickPosInMovieSpace);
	if(res==false)return false;

	res = TargetInfo.SkipCaptureInputKey( key,  eIE, strHoverControllerName, ClickPosInMovieSpace);
	if(res==false)return false;

	res = TeamHint.SkipCaptureInputKey( key,  eIE, strHoverControllerName, ClickPosInMovieSpace);
	if(res==false)return false;

	return super.SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

DefaultProperties
{
	MovieInfoStr="UI_HudInfoWidget.HudInfoWidget"
	bAllowFocus=FALSE
	Priority=4
}
