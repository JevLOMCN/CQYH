/********************************************************************
	Created:	2012
	Filename: 	UIKeyMapManager
	file ext:	uc
	Purpose:	control keyboard input
				physical key->Slot->actual Cmd
				hemu@snda
*********************************************************************/

class UIKeyMapManager extends Object
	dependson(UICommandFactory)
	native(UI);

var UICommandFactory CmdFactory;
var MMOGame CurMMOGame;
var MMOPlayerController PC;

//Cmd and Lock state corresponding to Slot
struct native  SSlotCmd
{
	var bool        IsLocked;
	var IUICommand  Cmd;
};
var array<SSlotCmd> UICmdList; 

enum EKeyCode
{
	// mouse
	KC_MouseLeft,
	KC_MouseRight,
	KC_MouseScrollUp,
	KC_MouseScrollDown,

    //
    KC_Enter,

	KC_Count
};

//Slot
enum ECommandKey
{
	ECK_NormalAtk <DisplayName="NormalAtk">,
	ECK_QTE1 <DisplayName="QTE1">,
	//ECK_QTE2 <DisplayName="QTE2">,
	//ECK_QTE3 <DisplayName="QTE3">,
	ECK_Passive1 <DisplayName="Passive1">,
	ECK_Passive2 <DisplayName="Passive2">,
	ECK_Passive3 <DisplayName="Passive3">,
//	ECK_MouseL <DisplayName="MouseL">,
//	ECK_MouseR <DisplayName="MouseR">,
	ECK_Slot1 <DisplayName="Slot1">,
	ECK_Slot2 <DisplayName="Slot2">,
	ECK_Slot3 <DisplayName="Slot3">,
	ECK_Slot4 <DisplayName="Slot4">,
	ECK_Slot5 <DisplayName="Slot5">,
	ECK_Slot6 <DisplayName="Slot6">,
	ECK_Slot7 <DisplayName="Slot7">,
	ECK_Slot8 <DisplayName="Slot8">,
	ECK_Slot9 <DisplayName="Slot9">,
	ECK_Slot10 <DisplayName="Slot10">,
	ECK_Slot11 <DisplayName="Slot11">,
	ECK_Slot12 <DisplayName="Slot12">,

	ECK_F1 <DisplayName="F1">,
	ECK_F2 <DisplayName="F2">,
	ECK_F3 <DisplayName="F3">,
	ECK_F4 <DisplayName="F4">,
	ECK_F5 <DisplayName="F5">,
	ECK_F6 <DisplayName="F6">,
	ECK_F7 <DisplayName="F7">,

	ECK_HotKey_Quest <DisplayName="Quest">,
	ECK_HotKey_QuestReward <DisplayName="QuestReward">,
	ECK_HotKey_Bag <DisplayName="Bag">,
	ECK_HotKey_Charactor <DisplayName="Charactor">,
	ECK_HotKey_Team <DisplayName="Team ">,
	ECK_HotKey_Map <DisplayName="Map">,
	ECK_HotKey_SkillSetting <DisplayName="Skill">,
	ECK_HotKey_System <DisplayName="System">,
	ECK_HotKey_Contact <DisplayName="Contact">,
	ECK_HotKey_KeyMap <DisplayName="KeyMap">,
	ECK_HotKey_CtrlLeft  <DisplayName="CtrlLeft">,
	ECK_HotKey_CtrlRight <DisplayName="CtrlRight">,
	ECK_HotKey_CtrlFront <DisplayName="CtrlFront">,
	ECK_HotKey_CtrlBack  <DisplayName="CtrlBack">,
	ECK_HotKey_Stall <DisplayName="Stall">,
	ECK_HotKey_WeaponSkill <DisplayName="WeaponSkill">,
	ECK_HotKey_SoulEmbed <DisplayName="SoulEmbed">,
	ECK_HotKey_SoulLevelUp <DisplayName="SoulLevelUp">,
	ECK_HotKey_Friend <DisplayName="Friend">,
	ECK_HotKey_ArenaTeam <DisplayName="ArenaTeam">,
	ECK_HotKey_QTE   <DisplayName="Qte">,
	ECK_HotKey_Rank   <DisplayName="RankList">,
	ECK_HotKey_Mount   <DisplayName="Mount">,
	ECK_HotKey_PKMODE   <DisplayName="PkMode">,
	ECK_HotKey_Master   <DisplayName="Master">,
	ECK_HotKey_Achievement   <DisplayName="Achievement">,
	ECK_HotKey_SkillTreeActiveSkill   <DisplayName="SkillTreeActiveSkill">,
	ECK_HotKey_GuildView   <DisplayName="GuildView">,
	ECK_HotKey_GuildMain   <DisplayName="GuildMain">,
	ECK_HotKey_Hero   <DisplayName="Hero">,
	ECK_HotKey_Enhance   <DisplayName="Enhance">,
	ECK_HotKey_Shift   <DisplayName="Shift">,
	ECK_HotKey_Activity <DisplayName="Activity">,
	ECK_HotKey_Heaven  <DisplayName="Heaven">,
	ECK_HotKey_MallStore  <DisplayName="MallStore">,
	ECK_HotKey_GoldStore  <DisplayName="GoldStore">,
	ECK_Count 
};

function init( )
{
	CmdFactory = new class'UICommandFactory';
	//Slot
	UICmdList.Add( ECK_Count );
}

//initialize once by default
event DefaultKeyMap( MMOPlayerController inPC )
{
	local int j;
	for( j = 0 ; j < ECK_Count ; ++j )
	{
		UnBindingSlot( j, false );
	}	

	PC = inPC;
}


native final function OnSlotKey( int slot , bool bRelease=false );
native final function IniServerSlotBind();
native final function BindingSlot( int slot , UICommandType slottype ,int abilityid );
native final function UnBindingSlot( int slot, bool isLocked = false );
native final function bool IsSlotLocked( int slotId );
native final function int GetCmdIDBySlotID( int slotId );

`if(`notdefined(FINAL_RELEASE))
function TestUICmdList( )
{
	local IUICommand cmd;
	local int i;
	i = -1;
	`log("TestUICmdList()::UICmdList's lehgth is:" @UICmdList.Length );
	while( ++i < UICmdList.Length )
	{
		cmd = UICmdList[i].Cmd;
		`log("Command,No." @i @" is :" @cmd);
	}
	return;
}
`endif

DefaultProperties
{
}
