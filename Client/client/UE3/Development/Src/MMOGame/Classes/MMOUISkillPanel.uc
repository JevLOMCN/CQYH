/********************************************************************
	Created:	2012/06
	Filename: 	MMOUISkillPanel
	file ext:	uc
	Author:		hemu@snda
	Purpose:	MMOUISkillPanel ability UI, contains the following 4 parts
				-Console ability overview
				-Active  active abilities
				  -Rune rune charge, activation
				  -SkillUp ability proficiency upgrade: heart training, soul training
				-Passive passive abilities
				-Avarta  Avarta abilities
*********************************************************************/

class MMOUISkillPanel extends MMOUIBase
	dependson(UIKeyMapManager)
	native(UI)
	config(UIVisibleMask);

enum ESkill_PageType
{
	ESkill_Console,
	ESkill_Active,
	ESkill_Passive,
	ESkill_Avarta,
};

//var MMOPlayer           MainPlayer;
//var MMOPlayerController PC;
//var array<MMOAbility>   AllAbilities;
//var UIKeyMapManager     KeyMapManager;

//var ESkill_PageType CurPageType;

//var int NowActivePageNum;
//var int NowPassivePageNum;
//var int NowAvartaPageNum;
//var int NowAvartaRunePageNum;

//var int TotalActivePageNum;
//var int TotalPassivePageNum;
//var int TotalAvartaPageNum;
//var int TotalAvartaRunePageNum;

//var int MaxActiveModifyCount;
//var int MaxPassiveModifyCount;
//var int MaxAvartaModifyCount;

//var int ActiveSlotCount;
//var int ActiveRuneCount;
//var int PassiveSlotCount;
//var int AvartaSlotCount;
//var int AvartaRuneCount;

////==================ActiveModify()================================ 
//var array<int> ActiveRealSlotList;
//struct native sCurrentActive
//{
//	//var int ItemId; //0~2
//	var int ActiveId;
//	var bool IsChanged;
//};
//var array<sCurrentActive>  RealActives;
//var array<sCurrentActive>  CurrentActives;
//struct native sAllActive
//{
//	var int ActiveId;
//	var bool IsEquiped;
//};
//var array<sAllActive>  AllActives;
//var int CurrentActiveSlot;
//var int CurrentSelectedActiveSlot;
//var int CurrentSelectedActiveId;
//var int CurrentSelectedActiveRuneSlot;

//var int CurrentSelectedAvartaSlot;
//var int CurrentSelectedAvartaId;
//var int CurrentSelectedAvartaRuneSlot;

//var int NowRequestNormalPracticeActiveId;
//var int NowNormalPracticeActiveId;
//var int NowNormalPracticeType;
//var int NowNormalPracticeProgress;

//var int NowRequestFastPracticeActiveId;
//var int NowFastPracticeActiveId;
//var int NowFastPracticeType;
//var int NowFastPracticeProgress;
//var int NowPracticeType;
//var bool IsInFastPractice;

//var string StrTimeLeft;

//enum SkillPracticeType
//{
//	SkillPT_None,
//	SkillPT_Noraml,
//	SkillPT_Fast
//};
//var bool IsRequestCloseNormalSkillPractice;
//var bool IsRequestCloseFastSkillPractice;
//var bool HasAskStopSkillPractice;
////==================PassiveModify()================================ 
//struct native sCurrentPassive
//{
//	//var int ItemId; //0~2
//	var int PassiveId;
//	var bool IsChanged;
//};
//var array<sCurrentPassive>  RealPassives;
//var array<sCurrentPassive>  CurrentPassives;
//struct native sAllPassive
//{
//	var int PassiveId;
//	var bool IsEquiped;
//};
//var array<sAllPassive>  AllPassives;
//var int CurrentPassiveSlot;

//// QTE ability  naming happens to differ from Avatar, so the resource name is kept
////==================AvartaModify()================================ 
//struct native sCurrentAvarta
//{
//	//var int ItemId; //0~2
//	var int AvartaId;
//	var bool IsChanged;
//};
//var array<sCurrentAvarta>  RealAvartas;
//var array<sCurrentAvarta>  CurrentAvartas;
//struct native sAllAvarta
//{
//	var int AvartaId;
//	var bool IsEquiped;
//};
//var array<sAllAvarta>  AllAvartas;
//var int CurrentAvartaSlot;
//var const float UpdateInterval;
//var float ElapsedTime;


//native function     PostAdvance(float DeltaTime);
////Update heart training panel
//native final  function     UpdateNormalPractice( );

//native final  function 	int GetRealActiveRealSlotId(int activeSlotId );
//native final  function 	SendEquipActive(int activeSlotId,int abilityId );
//native final  function 	SendEquipPassive(int passiveSlotId,int abilityId );
//native final  function 	SendEquipAvarta(int avartaSlotId,int abilityId );

//native function         SkillSlotChange();
//native final  function 	RefreshAll( );
//native final  function 	SendEquipActiveRune( int activeSlotId, int runeSlotId);
//native final  function 	SendEquipAvartaRune( int avartaSlotId, int runeSlotId);
//native final  function 	SendActivateRune( int inSpellId, int runeSlotId);
//native final  function 	RuneActivate( int wSpellID, int byRuneSlot, int byDurabilityType, int nDurability );
//native final  function 	RuneDurSync( int wSpellID, int byRuneSlot, int byDurabilityType, int nDurability );
//native final  function 	RuneEquip( int bySlot, int wSpellID, int byRuneSlot );
			
////Related checks to prevent Crash
//native final  function bool Validate();

////Reset state after logging in again
//native function EnterLogin();

//event RefreshData()
//{
//	RefreshCurrentSpell();
//	//RefreshActiveModify();
//}

////Perform initialization
//event InitLoad(MMOGFXPlayer pl)
//{
//	super.InitLoad(pl);
//	//_UIPlayer = pl;	
//	//@test
//	`log("MMOUISkillPanel : InitLoad()");
//	IniData();

//	ActiveRealSlotList.Add( ActiveSlotCount );
//	ActiveRealSlotList[ 0 ] = ECK_MouseL;
//	ActiveRealSlotList[ 1 ] = ECK_MouseR;
//	//Too many changes, hardcode it for now s
//	ActiveRealSlotList[ 2 ] = ECK_Slot1;
//	ActiveRealSlotList[ 3 ] = ECK_Slot2;
//	ActiveRealSlotList[ 4 ] = ECK_Slot3;
//	ActiveRealSlotList[ 5 ] = ECK_Slot4;

//	CurrentActives.Add( ActiveSlotCount );
//	RealActives.Add( ActiveSlotCount );

//	CurrentPassives.Add( PassiveSlotCount );
//	RealPassives.Add( PassiveSlotCount );

//	CurrentAvartas.Add( AvartaSlotCount );
//	RealAvartas.Add( AvartaSlotCount );

//	class'MMOUIManager'.static.SetReplaceHotSkillDelegate( ReplaceHotSkill );
//}

////Tab key directly switches left/right click abilities
//native final  function ReplaceHotSkill();

//event IniData()
//{
	
//	`log("==============================MMOUISkillPanel::IniData() Opended=================================================");
//	PC = MMOPlayerController( GetPC() );
//	MainPlayer = MMOPlayer( PC.Pawn );
//	//ThisWorld = GetPC().WorldInfo;
//	//`log("worldinfo:"$ThisWorld.GetMapInfo() );
//	KeyMapManager = MMOGame(PC.WorldInfo.Game).KeyMapManager;
//	PageJump( ESkill_Console );
//	GetObject("SpellConsole").SetVisible( true );
//	GetObject("ActiveModify").SetVisible( false );
//	GetObject("PassiveModify").SetVisible( false );
//	GetObject("AvartaModify").SetVisible( false );

//}

//native function NativeInitOnce();

//native final  function SetLockedState( int slotId, GFxObject tObj, int uiSlot );
////Refresh the whole panel
//native final  function RefreshCurrentSpell();

//event SetItem( GFxObject mc,int quantity,string IconName,int ItemIndex,optional string Type="", optional int itemColor = 0  )
//{
//	GFXPlayer.CallSetItemData(mc,quantity,IconName,ItemIndex, ESPType_SkillPanel, Type,itemColor );
//}

////Bag change response
//native function BagSlotChange( INT BagId,INT SlotId );
////Refresh current page
//native final  function RefreshCurrentPage( );
////Refresh page
//native final  function RefreshPage( ESkill_PageType tSPT );
////Perform related variable initialization
//native final  function PageJump( ESkill_PageType tSPT );
////PlayerMoneyChange
//native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int Money, int OldMoney);

////==================================================RefreshActiveModify()====================================================== 

////Set ActiveBtn
//native final  function SetSpellBtnByActiveId(GFxObject tObj,int SlotId,int abilityId,bool isSubCtrl = false );
////Select current CurrentActiveSlot
//native final  function SetCurrentActive( int activeSlotId );
////RefreshActiveModify
//native final  function RefreshActiveModify();
////Select CurrentActiveSlot switched to the ActiveId corresponding to AllActives( abilityId )
//native final  function SelectActive( int tSlotId,int abilityId );

////Refresh display of runes, ability points, etc.
//native final  function OnSelecteActiveChanged( int abilityId );
////Determine whether the current rune can be activated
//native final  function bool CanActiveRune( MMOAbility tRune );
////Set RuneBtn
//native final  function SetRuneBtnByAbility(MMOAbility tRune,int iRuneSlot );
////Click equip rune
//native final  function EquipRune( int runeSlotId);
////Click activate rune
//native final  function ActivateRune( int runeSlotId);
////Cancel rune equip
//native final  function CleanRune();
///*
////Rune icon hover
//native final  function OnRuneBtnHover( int runeSlotId );*/

////Whether Active is already equipped
//native final  function int IsActiveAlreadyEquiped( int abilityId );
////Confirm submit Modify
//native final  function ConfirmActive( );
//native final  function ActiveLastPage();
//native final  function ActiveNextPage();

////Ability upgrade
////Click pop up panel
//native final  function ASClickNormalPractice( bool IsToggle = true );
//native final  function ASClickFastPractice( bool IsToggle = true );
////Click start upgrading ability
//native final  function ASStartNormalPractice(  );
//native final  function ASStartFastPractice(  );
////Select heart training type
//native final  function ASChooseNormalParcticeType( int NPType );
////Select soul training type
//native final  function ASChooseFastPracticeType( int FPType );
////Soul training use medicine
//native final  function ASUseFastPracticeItem( int FPItem );
////Abort ability upgrade
//native final  function ASStopPractice( int inActiveId );
////Abort and close the ability upgrade panel
//native final  function ASStopPracticeAndClose( int inActiveId,SkillPracticeType spt );
////Abort ability upgrade confirm
//native final  function StopSkillPracticeOk(GFxObject mc);
////Abort ability upgrade cancel
//native final  function StopSkillPracticeCancel(GFxObject mc);

////Start ability upgrade
//native final  function NetStartSkillPractice(int inActiveId,int byPracticeType,int	nInterval,int nEndTime );
////End ability upgrade
//native final  function NetStopSkillPractice(int inActiveId);
////Ability proficiency reminder
//native final  function NetSkillProgressChange( int inActiveId,int nProgress );
////Ability upgrade hint
//native final  function NetSkillLevelUp( int inActiveId,int byLevel );
////Ability upgrade hint
//native final  function NetSkillPointChangeNotify( );
////Ability upgrade function enabled
//native final  function EnableSkillUp( bool isEnable = true );

////==================================================RefreshPassiveModify()====================================================== 

////Set PassiveBtn
//native final  function SetSpellBtnByPassiveId(GFxObject tObj,int SlotId,int abilityId,bool isSubCtrl = false );
////Select current CurrentPassiveSlot
//native final  function SetCurrentPassive( int passiveSlotId );
////RefreshPassiveModify
//native final  function RefreshPassiveModify();
////Select CurrentPassiveSlot switched to the PassiveId corresponding to AllPassives( abilityId )
//native final  function SelectPassive( int tSlotId,int abilityId );
////Whether Passive is already equipped
//native final  function int IsPassiveAlreadyEquiped( int abilityId );
////Confirm submit Modify
//native final  function ConfirmPassive( );

////==================================================RefreshAvartaModify()====================================================== 

////Set AvartaBtn
//native final  function SetSpellBtnByAvartaId(GFxObject tObj,int SlotId,int abilityId,bool isSubCtrl = false );
////Set AvartaBtn
//native final  function SetSpellBtnByAvarta(GFxObject tObj,int SlotId,MMOAbility tAvarta,bool isSubCtrl = false,bool isRuneCtrl = false );
////Select current CurrentAvartaSlot
//native final  function SetCurrentAvarta( int avartaSlotId );
////RefreshAvartaModify
//native final  function RefreshAvartaModify();
////Select CurrentAvartaSlot switched to the AvartaId corresponding to AllAvartas( abilityId )
//native final  function SelectAvarta( int tSlotId,int abilityId );
////Refresh runes once
//native final  function OnSelecteAvartaChanged( int  abilityId );
////Ability abilityId has equipped the tSlotId-th rune
//native final  function SelectAvartaRune( int tSlotId,int abilityId );
////Whether Avarta is already equipped
//native final  function int IsAvartaAlreadyEquiped( int abilityId );
////Confirm submit Modify
//native final  function ConfirmAvarta( );
////Turn page
//native final  function AvartaLastPage();
//native final  function AvartaNextPage();
//native final  function AvartaRuneLastPage();
//native final  function AvartaRuneNextPage();

//DefaultProperties
//{
//	IsDynamicLoad=true
//	MaxActiveModifyCount=10
//	MaxPassiveModifyCount=20
//	MaxAvartaModifyCount=5
//	ActiveSlotCount=6
//	ActiveRuneCount=5
//	AvartaRuneCount=10
//	PassiveSlotCount=3
//	AvartaSlotCount=3
//	IsRequestCloseNormalSkillPractice=false
//	IsRequestCloseFastSkillPractice=false
//	HasAskStopSkillPractice=false
//	UpdateInterval=0.06 //Second
//	ElapsedTime=0
//}
