class MMOUIDialogNew extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

// !!!This must match the constants in ActionScript3.NPCDialog.NPCDialogData!!!
enum EDialogDataType
{
	EDiagDataType_Dialog,
	EDiagDataType_Quest,
};

enum EOptionType
{
	EOptionType_Default,
	EOptionType_Quest,
};

enum EDialogContextType
{
	EDialogContextType_Dialog,
	EDialogContextType_Quest_Page,
	EDialogContextType_Quest,
};

struct native DialogContent
{
	var native pointer Contex{struct DialogContext};

	var array<string> DialogTextList;
	var string DialogAnswer[3];
	var int DialogAnswerId[3];
	var int QuestId;
    var rotator NPCOldRot;
};
//
var GFxObject ConfirmBox;

var NPC    DialogNpc;
var rotator DialogNpcRot;

var Quest CurQuest;
var LevelStreamingUIShow	ShowLevel;
var NPC	ShowNPC;
var MMOPlayer ShowPlayer;
var int CurrentDialogPage;

var DialogContent CurDialogContent;
var int SelectedAwardIndex;

var private transient bool RTSet;		// if the RT has been set
var private transient int PendingRTCounter;	// set rt x(1) frames later, to avoid green square at the first time - ming

function InitWidget()
{
	ShowLevel = GFXPlayer.GetShowLevel( "HalfShowLevel" );
	if( ShowLevel != none && ShowLevel.RenderTarget != none)
	{
	//	GFXPlayer.SetExternalTexture( "NpcDialogAvatar -nopack", ShowLevel.RenderTarget );
	//	ShowLevel.SetEnabled( TRUE );
	}

	RTSet = false;
	PendingRTCounter = -1;
}

native function CheckPendingRT(float deltaTime);

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	pl.AddPostAdvanceDelegate(CheckPendingRT);
}

event InitOnce()
{
	super.InitOnce();
}

native final function OpenDialog( NPC npc ,  DialogContent DiaCont );
native final function OpenQuestPopupUp( bool accept , int questID , bool share , int shareSrcId);

native final function CloseDialog(optional bool playSound=true);
native final function ClearContex();

native final function ActivationOk(GFxObject mc);
native final function ActivationCancel(GFxObject mc);

event NPC CloneNPC( NPC Base )
{
	if( ShowNPC == none )
	{
		ShowNPC = base.Spawn( class'NPC', GetPC() );
		ShowNPC.InitItems();
	}
	ShowNPC.CloneFrom( base );
	ShowNPC.SetPhysics( PHYS_None );
	return ShowNPC;
}

event MMOPlayer ClonePlayer( MMOPlayer Base )
{
	if( ShowPlayer == none )
	{
		ShowPlayer = base.Spawn( class'MMOPlayer', GetPC() );
		ShowPlayer.InitItems();
	}
	ShowPlayer.CloneFrom( base );
	ShowPlayer.SetPhysics( PHYS_None );
	return ShowPlayer;
}
// add by zhangrui05@snda.com
native final function SetShowActor(MMOActor actor);

native final function SelectDiagOption(int optionID);

native final function UpdateNPCDialog();

//native function TickForTestInteractNPCDistance(float deltaTime);

native final function TrySubmitQuest(int questID);

native final function AcceptAuest(int questID, bool share, int shareSrcId);

event CallUCUpdateDiagData(GFxObject datArr)
{
	ActionScriptVoid("UCUpdateDiagData");
}

event CallUCUpdateDiagPage(int curPage)
{
	ActionScriptVoid("UCUpdateDiagPage");
}

event CallUCPlay(int time)
{
	ActionScriptVoid("UCPlay");
}
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);


native final function OnAcceptTimeLimitQuest(GfxObject mc);
native final function OnRejectTimeLimitQuest(GfxObject mc);

native final function GfxObject CreateDialogContext(INT i,INT questID,BOOL share, INT shareSrcId,EDialogContextType type);
//native function OnShowNative(bool show);

//
native function ConfirmLevelUpCurWeaponAndGo( GFxObject pMc );
native function CancelLevelUpCurWeapon( GFxObject pMc );
//
native function TakeBackCurLeveUpWeaponRtNow( GFxObject pMc );
native function CancelTakeBackCurLeveUpWeaponRtNow( GFxObject pMc );

native final function ASSelectDiagOption(int optionID);
native final function ASNPCDialogPage(int pageIndex);
native final function ASCloseDialog();

native final function ASSelectAward(GFxObject icon, int idx, int itemTemplateID);

native final function ASAcceptQuest(int questID, bool share, int shareSrcId);
native final function ASAbortQuest(int questID);
native final function ASSubmitQuest(int questID);

native final function AbortQuest(int questID);

function AbortQuestOK(GFxObject mc)
{
	local int questID;
	questID = mc.GetInt("iteminfoid");
	AbortQuest(questID);
}

function AbortQuestCanel(GFxObject mc)
{
}

DefaultProperties
{
	IsTargetOutDistanceClose = true
	PendingRTCounter=-1
	RTSet=false
}
