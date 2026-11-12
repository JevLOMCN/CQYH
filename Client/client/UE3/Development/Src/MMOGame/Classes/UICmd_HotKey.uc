//
// 
//
class UICmd_HotKey extends IUICommand
	native(UI);

var int abilityid;
var MMOUIGFXPlayer MoviePlayer;

event Use()
{	
	if( PC.ChatBoxActived )
		return;
	class'MMOUIManager'.static.UpdateMainMenuKeyDown(false,CmdID,SlotID);
	
	`log("HotKey"@ CmdID );

	switch (CmdID)
	{
	case ECK_HotKey_Quest:
		break;
	case ECK_HotKey_QuestReward:
		break;
	case ECK_HotKey_Bag:	
		break;
	case ECK_HotKey_Charactor:		
		break;
	case ECK_HotKey_Team:
		break;
	case ECK_HotKey_Map:
		break;
	case ECK_HotKey_SkillSetting:
		break;
	//case ECK_HotKey_System:	
	//	break;
	//case ECK_HotKey_CtrlLeft:
	//	class'MMOUIManager'.static.MoveByKey(EMCK_Left,true);
	//	break;
	case ECK_HotKey_CtrlRight:
		class'MMOUIManager'.static.MoveByKey(EMCK_Right,true);
		break;
	case ECK_HotKey_CtrlFront:
		class'MMOUIManager'.static.MoveByKey(EMCK_Front,true);
		break;
//	case ECK_HotKey_CtrlBack:
//		class'MMOUIManager'.static.MoveByKey(EMCK_Back,true);
//		break;
	case ECK_HotKey_Stall:
		break;
	case ECK_HotKey_WeaponSkill:
		break;
	case ECK_HotKey_SoulEmbed:
		break;
	case ECK_HotKey_SoulLevelUp:
		break;
	case ECK_HotKey_QTE:
		class'MMOUIManager'.static.ASClickQTE();
		break;
	case ECK_HotKey_Rank:
		break;
	case ECK_HotKey_Mount:
		break;
	case ECK_HotKey_PKMODE:
		class'MMOUIManager'.static.ASPkModeChange();
		break;
	case ECK_HotKey_ArenaTeam:
		break;
	case ECK_HotKey_Hero:
		break;
	case ECK_HotKey_Friend:
		break;
	case ECK_HotKey_Master:
		break;
	case ECK_HotKey_Achievement:
		break;
	case ECK_HotKey_SkillTreeActiveSkill:
		break;
	case ECK_HotKey_GuildView:
		break;
	case ECK_HotKey_GuildMain:
		break;
	case ECK_HotKey_Enhance:
		break;
	case ECK_HotKey_Shift:
		//add
		class'MMOUIManager'.static.OnLeftShift(false);
		break;
	case ECK_HotKey_Activity:
		break;
	case ECK_HotKey_Heaven:
		break;
	case ECK_HotKey_MallStore:
		break;
	case ECK_HotKey_GoldStore:
		break;
	default:
		`log("Null Hotkey" @CmdID );
		break;
	}
}


event OnRelease()
{
	if( PC.ChatBoxActived )		
		return;

	class'MMOUIManager'.static.UpdateMainMenuKeyDown(true,CmdID,SlotID);

	`log("HotKey OnRelease"@ CmdID );

	switch (CmdID)
	{
	case ECK_HotKey_CtrlLeft:
		class'MMOUIManager'.static.MoveByKey(EMCK_Left,false);
		break;
	case ECK_HotKey_CtrlRight:
		class'MMOUIManager'.static.MoveByKey(EMCK_Right,false);
		break;
	case ECK_HotKey_CtrlFront:
		class'MMOUIManager'.static.MoveByKey(EMCK_Front,false);
		break;
	case ECK_HotKey_CtrlBack:
		class'MMOUIManager'.static.MoveByKey(EMCK_Back,false);
		break;
	case ECK_HotKey_Shift:
		//add
		class'MMOUIManager'.static.OnLeftShift(true);
		break;
	}
	
}

DefaultProperties
{
}
