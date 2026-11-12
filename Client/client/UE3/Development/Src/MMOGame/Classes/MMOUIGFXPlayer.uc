class MMOUIGFXPlayer extends MMOGFXPlayer
	dependson(MMOUIBase, Quest, TypeDefine)
	config(MMOGame)
	native(UI);
	
enum TargetActor_TYPE
{
	TargetActor_TYPE_NONE,
	TargetActor_TYPE_NPC,
	TargetActor_TYPE_MONSTER,
	TargetActor_TYPE_BOSS,
	TargetActor_TYPE_OhterPlayer
};



var GFxObject  _curHotSkillKey;


var GFxObject       mcPickItemInfo;
var GFxObject       mcPickItemMsg;
var array<int>      PickItemInfoList;

var MMOUISceneHint          SceneHint;
var MMOUITips	                   _Tips;

var MMOPlayerController PlayController;
var MMOUIFriendChatHint FriendChatHintPanel;

var MMOUILevelUp PlayerLevelUpUI;

//var MMOUIStallSettingPanel _StallSetting;
//var MMOUIStallBuyPanel     _StallBuy;


var MaterialInstanceConstant BallMIC;
var MaterialInstanceConstant BallMIC2;


var bool MPVisible;
var bool NeedReInitSceneUI;//need to reinitialize scene-related UI, updated once when the scene starts rendering after loading


var int CurrentSkillBarID; // ability bar number, currently 3
var bool Registered;

var EMoveCtrlKey WSADKey[5];

// oscar@snda custom load/close SwfMovie

struct native MovieInfoProxy
{
	var const native transient pointer pMovie{class FGFxMovie};
};

var transient array<MovieInfoProxy> CustomLoadMovieList;

final native function InitMMOUIWidgets();
final native function CloseMMOUIWidgets();

event OnClose()
{
	CloseMMOUIWidgets();
}
// oscar@snda

native function TickHud( Canvas cvs , float fDeltaTime );
native function MoveByKey( EMoveCtrlKey MoveType,bool MoveFlag);


// function InitMoviePlayer()
// {	
// 	`log("___________MMOUIGFXPlayer InitMoviePlayer");
// 	if( !Registered)
// 		return;
// 
// 	`log("___________MMOUIGFXPlayer InitMoviePlayer OK");
// 	super.InitMoviePlayer();
// 
// 	GetVariableObject("root.mouse_mc").SetVisible( PlayController.bShowSoftMouse );	
// }

//native final function UnInitMoviePlayer();


function RegisterSceneHint( MMOUISceneHint mc , name  inFriendlyName)
{
	`log("RegisterSceneHint:" @ mc @ inFriendlyName);
	SceneHint = mc;
	RegisterUI( mc, inFriendlyName);
}

function registerTips( MMOUITips mc, name inFriendlyName)
{
	`log("registerTips:" @ mc @ inFriendlyName);
	_Tips = mc;
	RegisterUI( mc, inFriendlyName);
}



function RegisterFriendChatHintPanel( MMOUIFriendChatHint mc , name inFriendlyName)
{
	FriendChatHintPanel = mc;
	FriendChatHintPanel.SetVisible( false );
	RegisterUI( mc, inFriendlyName);
}



native function ASSetPkMode(int PkMode);



function native SendEnterMap( int Level);

function native SendMapDiffChange( int Level);


event PostWidgetInit()
{

}

//From AS, click switch ability
function SwitchSkillBar()
{
	//local int i;
	CurrentSkillBarID ++;
	if( CurrentSkillBarID >3 )
	{
		CurrentSkillBarID = 1;
	}
	else
	{
		//RefreshSkillBar();
	}	
	//GetVariableObject("root.Skillbar_mc.SkillBarSwitch_mc").SetString("label",""$CurrentSkillBarID);
}

//-------------
function ActiveChatBox()
{

}



//Open the ability quick replace box
function OpenSkillHotBar( GFxObject mc )
{
	local int i,j,off;
	local GFxObject slotmc;
	local MMOAbility abi;
	local array<ASValue> args;

	//return;//not used for now
	`log("OpenSkillHotBar");
	off = 0;
	if( GetVariableObject("root.Skillbar_mc.SkillHotBar_mc").GetBool("visible") )
		GetVariableObject("root.Skillbar_mc.SkillHotBar_mc").SetVisible(false);
	else
		GetVariableObject("root.Skillbar_mc.SkillHotBar_mc").SetVisible(true);
	for( i = 0 ; i< 8 ; i++ )
	{
		slotmc = GetVariableObject("root.Skillbar_mc.SkillHotBar_mc.SkillHotBarSlot_"$i$"_mc");
		if( slotmc != none)
		{				
			slotmc.SetVisible(false);
			for ( j = off ;j< MMOPlayer( GetPC().Pawn ).Abilities.Length ; j++)
			{
				abi = MMOPlayer( GetPC().Pawn ).Abilities[j];
				if( abi != none && abi.GetAbilityId() >=0 )
				{
					if( abi.IsPassiveAbility() )
					{
						off = j+1;
						continue;
					}
					else
					{
						CallSetItemData( slotmc, 0, abi.GetAbilityIconName(), abi.GetAbilityId(), ESPType_SkillBar, "skill");
						off = j+1;
						slotmc.SetVisible(true);
						break;
					}
				}
			}
		}
	}
	args.Length = 0;
	_curHotSkillKey = mc;
	GetVariableObject("root.Skillbar_mc.SkillHotBar_mc").Invoke( "UpdateSize",args);

	
}

native function NativeASOnUI( bool bOnUI , string McName);
function ASOnUI( bool bOnUI , string McName)
{
	NativeASOnUI( bOnUI , McName);
	MMOPlayerController( GetPC()).bUIMask = bOnUI;
	MMOPlayerController( GetPC()).MaskWidgetName = McName;
}

function ClickLink(float x,float y,float z)//hyper link
{
	return;//no pathfinding for now

	PlayController.MouseHitWorldLocation.X = x;
	PlayController.MouseHitWorldLocation.y = y;
	PlayController.MouseHitWorldLocation.z = z;

	//PlayController.StartMoveToTargetPosition();

	//`log("clicklink"@x@"and"@y);

	
}

// function ClickFollow( GFxObject mc,bool bSelect)
// {
// 	local int i;
// 	local Quest Quest;
// 
// 	i = mc.GetObject("QuestListButton_mc").GetFloat("itemid");
// 
// 	Quest.bQuestFollow = bSelect;
// }

function HideTips(GfxObject tObj )
{
	//_Tips.HideTips( tObj);

}

function ShowFreeStyleTips(string tipshtml,GfxObject widge)
{
	//_Tips.ShowFreeStyleTips(  ParseLocalizedPropertyPath(tipshtml), widge);
}

/*
event ReturnLogin()
{
	//var MMOGame game;
	//game.NextGameState

	class'MMOUIManager'.static.ActiveGameState(EGameState_Login);
	//Local Vector vec;
	//MMOPlayerController( GetPC()).SwitchUIPlayer(EST_Login);
	//Restore initial state
// 	GetVariableObject("root").GotoAndPlayI( 0 );
// 	GetVariableObject("root.system_mc").SetVisible( false );
// 	GetVariableObject("root").SetVisible( false );
	//Delete MsgBox
	//UnInitMoviePlayer();
	//MMOGame(GetPC().WorldInfo.Game).SceneType = EST_Login;
	//vec.x = 27008.00; vec.y=21152.00; vec.Z=-928;
	//vec = MMOHud(MMOPlayerController( GetPC() ).myHud).SplashMovie.PlayerStartPostion;
	//MMOPlayerController(GetPC()).TeleportToScene( "Arena01_CharacterSelection2" ,vec);
	//MMOHud(MMOPlayerController( GetPC() ).myHud).SplashMovie.ReturnLogin();
	//MMOPlayerController(GetPC()).ReturnLogin();
}*/

// OSCARTODO: this seems to have never been used?
//function ConfirmLogout()
//{
//	MsgBoxMgr.AddModalMessageBox( localize("MsgBox","text15" ,"MMOGame" ), 0, GoLogout, none);
//	//MsgBoxMgr.AddMessageBox( localize("MsgBox","text15" ,"MMOGame" ), 0, GoLogout, none);
//}

native function ModelEquip();
function SysModelEquip()
{
	local MMOPlayer tPlayer;
	local MMOPlayer pl;

	if(class'WorldInfo'.static.IsPlayInEditor())
	{
		return;
	}

	pl = MMOPlayer(GetPC().pawn);

	if( ShowAvatar == none )
	{
		// todo: spawn actor instead of fetching from the level
		foreach GetPC().AllActors(class'MMOPlayer',tPlayer)
		{
			
			if(  tPlayer.ActorName == "male_model"  )
			{
				ShowAvatar = tPlayer;
			}
		}
	}
		if( ShowAvatar != none )
		{
			ShowAvatar.SetMesh( EPlayerClass( pl.GetClassId() ) ,EGenderType( pl.GetSex() ) );
			ModelEquip();
		}


	return;

}


event ShowMsgSceneHint( string bigInfo )
{
	local GFxObject mc ;
	`log("ShowMsgSceneHint");
	mc = GetVariableObject("root.MsgSceneHint_mc");
	mc.SetVisible( true );
	mc.SetBool("visible",true);
	mc.GotoAndPlayI( 0 );
	GetVariableObject("root.MsgSceneHint_mc.textField").SetText( bigInfo );
}

//native function MouseRightClickItem (GFxObject mc);

event OpenStallBuy( MMOPlayer Player  )
{
	//_StallBuy.Open( player );
}

//itemslot throw out
native function ASThrowSlot( GFxObject mc );

function OnThrowItemOk(GFxObject mc)
{
	if( mc != none )
	{
		DeleteItem( mc.GetInt("iteminfoid") , mc.GetInt("itemdataid") );
	}
}



native function DeleteItem( int ConId , int SlotId );

function ChangeEquip(GFxObject SrcMc ,GFxObject DesMc)
{

// 	local int SrcSlotID;
// 	local int DesSlotID;
	local int SrcType;
	local int DesType;
// 	local int SrcEquipID;
// 	local int DesEquipID;
	//local Item SrcItem;
	//local MMOGFXPlayer_EquipEnhance equipEnhcPlyer;
	//local MMOUIEquipEnhance equipEnhc;

	if(DesMc==none)
		return;
	`log("change:"@SrcMc@DesMc);

	SrcType = SrcMc.GetInt("SlotType" );
	DesType = DesMc.GetInt("SlotType" );
// 	SrcSlotID = SrcMc.GetInt("itemid");
// 	DesSlotID = DesMc.GetInt("itemid");
// 	SrcEquipID = SrcMc.GetInt("EquipID");
// 	DesEquipID = DesMc.GetInt("EquipID");

	`log("change:"@SrcType@DesType);
// 	`log("change:"@SrcSlotID@DesSlotID);
// 	`log("change:"@SrcEquipID@DesEquipID);


	//This UI message is received when moving equipment items to change
	if( SrcType == ESPType_Bag )//source is bag
	{
		switch (  DesType )
		{
		case ESPType_SkillBar:
			if( DesMc.GetObject( "_ItemData" ).GetString( "itemtype" ) == "skill" )
			{
				
			}
			break;		
		}
	}
	
	return;
}



function ReturnLoginOk(GFxObject mc)
{
	//ReturnLogin();
}

function ShowWarningTrap(bool isShow ,optional Texture tex, optional int x,optional int y,optional float para1 )
{
		local array<ASValue> args;
	local ASValue arg;
	arg.n = para1*1000;
	arg.Type = AS_Number;
	args[0] = arg;
	
	if(isShow)
	{
			GetVariableObject("root.WarningTrap_mc").SetVisible(true);
			GetVariableObject("root.WarningTrap_mc").Invoke("SetStart",args);		
	}
	else
	{
		GetVariableObject("root.WarningTrap_mc").SetVisible(false);
	}
}
function QTEResult( bool success)
{
		GetVariableObject("root.Hint_mc").SetBool("hasGetQteResult",true);
		GetVariableObject("root.Hint_mc").SetBool("success",success);
}

function SetHint( bool isShow ,optional EHint_Type HintType ,optional Texture tex, optional int x,optional int y,optional float para1 )
{
	if( isShow )
	{	
		switch(HintType)
		{
		case EHT_QteSpace:
			GetVariableObject("root.Hint_mc").GotoAndStop("QteSpace");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);
			GetVariableObject("root.Hint_mc").SetBool("hasGetQteResult",false);
			break;
		case EHT_HoldStone:
			GetVariableObject("root.Hint_mc").GotoAndStop("HoldStone");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;
		case EHT_FinAttack:
			GetVariableObject("root.Hint_mc").GotoAndStop("FinAttack");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;
		case EHT_FrequenceSpace:
			GetVariableObject("root.Hint_mc").GotoAndStop("FreSpace");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;
		case EHT_OpenTrap:
			GetVariableObject("root.Hint_mc").GotoAndStop("OpenTrap");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;
		case EHT_TurnTrap:
			GetVariableObject("root.Hint_mc").GotoAndStop("TurnTrap");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;
		case	EHT_Slip:
			GetVariableObject("root.Hint_mc").GotoAndStop("slip");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;
		case	EHT_ClimbWall:
			GetVariableObject("root.Hint_mc").GotoAndStop("ClimbWall");
			GetVariableObject("root.Hint_mc").GetObject("HintLabel_mc").GotoAndPlayI(1);			
			break;

		case EHT_Texture:
			GetVariableObject("root.Hint_mc").GotoAndStop("Texture");
					GetVariableObject("root.Hint_mc").SetFloat("width",tex.GetSurfaceWidth());
			GetVariableObject("root.Hint_mc").SetFloat("height",tex.GetSurfaceHeight() );
			SetExternalTexture( "keySpace",tex );
			break;
		}
		
		GetVariableObject("root.Hint_mc").SetVisible(true);	
	
		GetVariableObject("root.Hint_mc").SetPosition(x,y);
	}
	else
	{
	
		GetVariableObject("root.Hint_mc").SetVisible(false);	
	}
}

function ASRollOutItemSlot( GFxObject mc)
{
	SetMouseState( EMS_Normal );
}

function ReleaseOutItem(GFxObject mc)
{

}


//function Vector2D GetMouseXY()
//{
//	local Vector2D mouse;
//	mouse.X = GetVariableNumber("root._xmouse");
//	mouse.Y = GetVariableNumber("root._ymouse");
//	return mouse;
//}



//function SetShowPlayer( MMOPlayer pl)
//{

//}





native function int CheckInputCharSize( string TextContent );



function ASCheckInputCharSize( GFxObject mc , string TextContent)
{
	local int size;
	size = CheckInputCharSize( TextContent );
	mc.SetInt( "ActualCharNum" , size );
}

function SetController(PlayerController ctrl)
{
	PlayController = MMOPlayerController(GetPC());//MMOPlayerController(ctrl);
}


function Init(optional LocalPlayer player)
{
	super.Init(player);
	
	//CreateNPCDialogManager();

	InitMMOUIWidgets();

	`log("HUDInit");
}






// event SetUIPlayerVisible(bool bVisible)
// {
// 	//not used anymore
// 	return;
// 
// 
// 	//m_SkillBar.SetVisible(bVisible);	
// 	if( bVisible   )
// 	{
// 		if( !MPVisible )
// 		{
// 			OnChangeScene();//clean up some map-related data
// 			InitMoviePlayer();
// 			
// 		}
// 		else
// 			OnChangeScene();
// 	}
// 	else
// 		UnInitMoviePlayer();
// 
// 	MPVisible = bVisible;
// 	GetVariableObject("root").SetVisible(bVisible);
// }

event OnChangeScene()
{
	local int i;

	for(i = 0;i< UIObjectList.Length;i++)
	{

		//UIObjectList[i].OnChangeScene(); // FIX:
	}
	NeedReInitSceneUI = true;

	WSADKey[0] = EMCK_Empty;
	WSADKey[1] = EMCK_Empty;
	WSADKey[2] = EMCK_Empty;
	WSADKey[3] = EMCK_Empty;
}



//Add pickup item hint
native function AddPickItemInfo( int ItemId );
//Play pickup item hint
native function PlayGetItemInfo( int ItemId );
//Pickup item hint playback finished notification
native function ASGetItemEnded();

DefaultProperties
{
	//MovieInfoStr="MMO_UI.Gamehud.Gamehud"
	bEnableGammaCorrection=false
	_curHotSkillKey=none

	BallMIC=MaterialInstanceConstant'FX_UI_Material.UI_MIC_B'
	BallMIC2=MaterialInstanceConstant'FX_UI_Material.UI_MIC_A'

	//m_DropIcon=none
	SoundThemes(0)=(ThemeName=default, Theme=UISoundTheme'MMO_UISoundTheme.Default')
	//NPCDialogManagerTemplet=MMO_NPCDialogManager'MMO_Dialog.MMO_Dialog.TalkList'
	
	MPVisible=false
	bAllowInput=true
	bAllowFocus=true
	bCaptureInput=false
	CurrentSkillBarID=1
	
	Registered = false

	WSADKey[0] = EMCK_Empty
	WSADKey[1] = EMCK_Empty
	WSADKey[2] = EMCK_Empty
	WSADKey[3] = EMCK_Empty
}
