#include "stdafx.h"
#include "MMOChatManager.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include "FriendManager.h"
#include "LinkManager.h"
#include "CommonGameEvents.h"
#include "UITextParser.h"
#include "MMOMapDataManager.h"
#include "TeamManager.h"
#include "MMOGuildData.h"
#include "MMOVipManager.h"
#include "WebHtmlManager.h"
#include "MMOEquipManager.h"

#define ChatDataFile TEXT("ChatOptionData")
#define HINTPOS 5
#define MSGLIMT 100
#define PRIVATEMSGLIMT 5
extern UBOOL g_bGMBuff;

MMOChatManager::MMOChatManager()
{
	//const FString IconPath = GConfig->GetStr( TEXT("MMOGame.HintButtonIcon") , TEXT("ArenaQueueHintIcon") , GGameIni );
	//_HintIconMap.insert( std::make_pair( EHintButtonType_ArenaQueueHint , IconPath )  );

	//_HintCommandMap.insert( std::make_pair( EHintButtonType_ArenaQueueHint , &Command_ArenaQueue ) );
	Init();
}

MMOChatManager::~MMOChatManager()
{

}

void MMOChatManager::Init()
{
	MMOUI::RegisterEventHandler( this, GameEvent_PlayerBaseInfo);

	GConfig->GetString( TEXT("ChatColor"), TEXT("TotalChannelFontColorString") , TotalChannelFontColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("NearChannelFontColorString") , NearChannelFontColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("PrivateChannelFontColorString") , PrivateChannelFontColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("TeamChannelFontColorString") , TeamChannelFontColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("SystemChannelFontColorString") , SystemChannelFontColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("GuildChannelFontColorString") , GuildChannelFontColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("TrumpetChannelFontColorString") , TrumpetChannelFontColorString , GUIColorIni);
	//GConfig->GetString( TEXT("ChatColor"), TEXT("GMFontColorString"),GMFontColorString , GUIColorIni);
	//chat box color
	GConfig->GetString( TEXT("ChatColor"), TEXT("ChatMyNameColorString") , ChatMyNameColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("ChatOtherPlayerColorString") , ChatOtherPlayerColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("ChatSkillColorString") , ChatSkillColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("ChatAchievementColorString") , ChatAchievementColorString , GUIColorIni);
	//friend chat color
	GConfig->GetString( TEXT("ChatColor"), TEXT("FriendChatSenderColorString") , FriendChatSenderColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("FriendChatMyContentColorString") , FriendChatMyContentColorString , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("FriendChatOtherPlayerContentColorString") , FriendChatOtherPlayerContentColorString , GUIColorIni);

	GConfig->GetString( TEXT("ChatColor"), TEXT("WorldChannelIconPath") , WorldChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("NearChannelIconPath") , NearChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("PrivateChannelIconPath") , PrivateChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("TeamChannelIconPath") , TeamChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("SystemChannelIconPath") , SystemChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("GuildChannelIconPath") , GuildChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("TrumpetChannelIconPath") , TrumpetChannelIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("GMIconPath") , GmIconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("LabaIconPath") , LabaIconPath , GUIColorIni);

	GConfig->GetString( TEXT("ChatColor"), TEXT("VipLevel1IconPath") , VipLevel1IconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("VipLevel2IconPath") , VipLevel2IconPath , GUIColorIni);
	GConfig->GetString( TEXT("ChatColor"), TEXT("VipLevel3IconPath") , VipLevel3IconPath , GUIColorIni);
	

	GConfig->GetString( TEXT("ChatColor"), TEXT("TvShow") , TvShowColor , GUIColorIni);
	GConfig->GetString(TEXT("ChatColor"), TEXT("Lighting"), MarqueeColor, GUIColorIni);

	GConfig->GetInt(TEXT("ChatColor"), TEXT("TvShowTimes"), MarqueeTimes, GUIColorIni);
	GConfig->GetInt(TEXT("ChatColor"), TEXT("TvShowSpeed"), MarqueeSpeed, GUIColorIni);

	GConfig->GetInt( TEXT("Trumpet") , TEXT("TrumpetTime") , TrumpetTime , GDefaultGameIni );
	GConfig->GetInt( TEXT("Trumpet") , TEXT("TrumpetTimes") , TrumpetTimes , GDefaultGameIni );

	GConfig->GetInt( TEXT("Chat") , TEXT("TrumpetID") , TrumpetID , GDefaultGameIni );

	GConfig->GetInt( TEXT("Chat") , TEXT("TrumpetCDTimes") , TrumpetCDTimes , GDefaultGameIni );
	
	GConfig->GetString( TEXT("Chat") , TEXT("TeamSign") , TeamSign , GDefaultGameIni );
	GConfig->GetString( TEXT("Chat") , TEXT("GuildSign") , GuildSign , GDefaultGameIni );
	GConfig->GetString( TEXT("Chat") , TEXT("NearSign") , NearSign , GDefaultGameIni );

	GConfig->GetInt(TEXT("Chat"), TEXT("ChatNearTime") , ChatNearCD, GDefaultGameIni);

	MMOUI::RegisterEventHandler(this, GameEvent_LeaveSelectingRole);

	_IsAddChannelTab = FALSE;
	isShowTrumpet = FALSE;
	bFocus = FALSE;
	bNewChat = FALSE;

	DefaultChatFilterChannelList.Empty();
	AddDefaultChatFilter();
}

void MMOChatManager::ClearAll()
{
	ChatBuffList.Empty();
	tempBuffList.Empty();
	ChatTrumpetList.Empty();
	ChatPrivateList.Empty();

	for ( INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
	{
		ChatFilterChannelList(i).ChatMessageList.Empty();
	}
	
}

//----------------------------------------------------------------------------------------------------------------------------
void MMOChatManager::AddDefaultFilterChannel()
{
	//default channels are General, Private, Party, Nearby, only Tips is missing
	INT ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_World;
	ChannelFilterType |= 0x1<<ECC_Nearby;
	ChannelFilterType |= 0x1<<ECC_Guild;
	ChannelFilterType |= 0x1<<ECC_Team;
	ChannelFilterType |= 0x1<<ECC_Private;
	ChannelFilterType |= 0x1<<ECC_System;
	ChannelFilterType |= 0x1<<ECC_Hint;
	SetFilterChannel( 0 , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.All") ) , ChannelFilterType , TRUE );
	
	/*ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_Nearby;
	ChannelFilterType |= 0x1<<ECC_Private;
	SetFilterChannel( 1 , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Nearby") ), ChannelFilterType , FALSE );
	
	ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_Guild;
	ChannelFilterType |= 0x1<<ECC_Private;
	SetFilterChannel( 2 , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Guild") ), ChannelFilterType , FALSE );
	
	ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_Team;
	ChannelFilterType |= 0x1<<ECC_Private;
	SetFilterChannel( 3 , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Team") ),ChannelFilterType , FALSE );
	
	ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_Private;
	SetFilterChannel( 4 , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Private") )  , ChannelFilterType , FALSE );
	
	ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_Hint;
	SetFilterChannel( 5 , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Hint") )  , ChannelFilterType , FALSE );*/
}

//----------------------------------------------------------------------------------------------------------------------------
void MMOChatManager::SetFilterChannel( INT pos , const FString& ChannelName , INT FilterChannelType , UBOOL defaultChannel )
{
	if( ChatFilterChannelList.Num() > pos )
	{
		//modify
		ChatFilterChannelList(pos).ChannelFilterType = FilterChannelType;
		ChatFilterChannelList(pos).IsDefaultChannel = defaultChannel;
		ChatFilterChannelList(pos).ChannelName = ChannelName;

		//MMOUI::GetUIManager()->delegateSetFilterChannel( pos , ChannelName );
	}
	else
	{
		//newly added
		ChatFilterChannelMessage FilterChannelMsg;
		FilterChannelMsg.ChannelFilterType = FilterChannelType;
		FilterChannelMsg.IsDefaultChannel = defaultChannel;
		FilterChannelMsg.ChannelName = ChannelName;
		ChatFilterChannelList.AddItem( FilterChannelMsg );

		//MMOUI::GetUIManager()->delegateSetFilterChannel( ChatFilterChannelList.Num()-1 , ChannelName );
	}
	
}

void MMOChatManager::AddNewFilterChannelCopy(  )
{
	if( CopyChatFilterChannelList.Num() < UCONST_MaxChatChannelNum )
	{
		ChatFilterChannelMessage FilterChannelMsg;
		FilterChannelMsg.ChannelFilterType = 0 ;
		FilterChannelMsg.IsDefaultChannel = FALSE;
		FilterChannelMsg.ChannelName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NewChannel"));
		CopyChatFilterChannelList.AddItem( FilterChannelMsg );

		
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.OverLim")),MsgInfoType_Table,TEXT("MMOGame.Chat.OverLim") );
		//exceeds limit
	}
}

void MMOChatManager::DelFilterChannelCopy( INT pos )
{
	if( !CopyChatFilterChannelList(pos).IsDefaultChannel )
	{
		CopyChatFilterChannelList.Remove(pos);

	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.DefaultCantChange")) ,MsgInfoType_Table,TEXT("MMOGame.Chat.DefaultCantChange"));
		
	}
}

void MMOChatManager::ReCopyChatFilter()
{
	CopyChatFilterChannelList.Empty();
	for ( INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
	{
		ChatFilterChannelMessage CM;
		CM.ChannelFilterType = ChatFilterChannelList(i).ChannelFilterType;
		CM.IsDefaultChannel = ChatFilterChannelList(i).IsDefaultChannel;
		CM.ChannelName = ChatFilterChannelList(i).ChannelName;
		CopyChatFilterChannelList.AddItem(CM);
	}
}	

void MMOChatManager::ReCopyDefaultChatFilter()
{
	CopyChatFilterChannelList.Empty();
	for ( INT i = 0 ; i < DefaultChatFilterChannelList.Num() ; i++ )
	{
		ChatFilterChannelMessage CM;
		CM.ChannelFilterType = DefaultChatFilterChannelList(i).ChannelFilterType;
		CM.IsDefaultChannel = DefaultChatFilterChannelList(i).IsDefaultChannel;
		CM.ChannelName = DefaultChatFilterChannelList(i).ChannelName;
		CopyChatFilterChannelList.AddItem(CM);
	}
}

void MMOChatManager::ApplyCopyChatFilter()
{
	INT channelListNum = ChatFilterChannelList.Num();
	INT copyChannelListNum = CopyChatFilterChannelList.Num();

	if( channelListNum > copyChannelListNum )
	{
		for(INT i = channelListNum - 1; i >= copyChannelListNum ; i--)
		{
			ChatFilterChannelList.Remove(i);
		}
		
	}
	else if( channelListNum < copyChannelListNum )
	{
		ChatFilterChannelList.AddZeroed( copyChannelListNum - channelListNum );
	}

	for( INT i = 0 ; i < copyChannelListNum ; i++ )
	{
		if( ChatFilterChannelList(i).ChannelFilterType != CopyChatFilterChannelList(i).ChannelFilterType )
		{
			ChatFilterChannelList(i).ChannelFilterType = CopyChatFilterChannelList(i).ChannelFilterType;
			ChatFilterChannelList(i).ChatMessageList.Empty();
		}

		ChatFilterChannelList(i).ChannelName = CopyChatFilterChannelList(i).ChannelName;
	}

	

	//update uichat
	MMOUI::GetUIManager()->delegateSetFilterChannelNum( ChatFilterChannelList.Num() );

	for (INT i = 0 ; i < copyChannelListNum ; i++ )
	{

		MMOUI::GetUIManager()->delegateSetFilterChannel( i , ChatFilterChannelList(i).ChannelName );
	}
	
}

//----------------------------------------------------------------------------------------------------------------------------
void MMOChatManager::ReceiveMessage( INT location , INT ChannelId , INT TargetId , FString MsgContent , FString SenderName, INT SenderID , INT targeId, unsigned bGM )
{
	debugf(TEXT("[CHAT] [%s]: %s"), *SenderName, *MsgContent);


	// add by zhangrui look up other characters' info here just in case (right-click menu operations may need this info)

	INT CharType = GetTypeFromID(SenderID);
	if (CharType == otRole && SenderID > 0)
	{
		MMOFacade::getSingleton().RequestPlayerBaseInfo(SenderID);
	}

	int TypeId = 0;
	
	//check blacklist
	if( FriendManager::getSingleton().InMyBlackList(SenderID) )
	{
		return;
	}

	if( ChannelId == otChannel  )
	{
		if( TargetId ==  cctServer || TargetId == cctGroup )
		{
			TypeId = ECC_World;
		}
		else if(  TargetId ==  cctScreen )
		{
			TypeId = ECC_Nearby;
		}
		/*else if (TargetId == cctGroup )
		{
		TypeId = ECC_Trumpet;
		}*/
	}
	else if( ChannelId == otTeam )
		TypeId = ECC_Team;
	else if( ChannelId == otGuild )
		TypeId = ECC_Guild;
	else if( ChannelId == otRole )
		TypeId = ECC_Private;

	if (SenderID == 0)
	{
		TypeId = ECC_System;
	}

	if( location == cltLighting )
	{
		//scrolling marquee announcement
		ParseContext ctx;
		DialogContext dtx;		
		UITextParser::getSingleton().Parse(ctx, dtx, MsgContent );

		MMOGameEvent_MarqueeNoticeInfo evt( dtx.TempText , MarqueeColor , MarqueeTimes , MarqueeSpeed );
		MMOUI::PostGameEvent( &evt);	

		//add marquee announcement to the system channel
		AddSystemMessage(dtx.TempText);
		return;
		//does normal chat need to be parsed once?
	}
	else if( location == cltTvShow )
	{
		ParseContext ctx;
		DialogContext dtx;		
		UITextParser::getSingleton().Parse( ctx , dtx , MsgContent );		

		MMOGameEvent_RollingNoticeInfo evt( dtx.TempText , TvShowColor );
		MMOUI::PostGameEvent( &evt);	

		//AddSystemMessage(dtx.TempText);
		return;
	}
	else if (cltUrl == location)
	{
		bool bRes = WebHtmlManager::getSingleton().AddNewUrlMessageBox(MsgContent);
		if(bRes)
			return;
	}
	else if(cltTrumpet == location)
	{
		TypeId = ECC_Trumpet;
	}
	
	//Guild mute
	if( ChannelId == otGuild && MMOGuildData::getSingleton().IsGuildFullMemberForbitChat(SenderID) )
	{
		return;
	}

	//handle special fixed strings
	//MsgContent = TEXT("test follows, <S>503902</S>");
	INT constStringPosStart = MsgContent.InStr( TEXT("<S>"), FALSE , FALSE );
	if( constStringPosStart >= 0 ){
		FString constString;
		INT constStringPosEnd = MsgContent.InStr( TEXT("</S>"), FALSE , FALSE );
		FString constStringIdxStr = MsgContent.Mid( constStringPosStart + 3,  constStringPosEnd - constStringPosStart - 3 );
		if( constStringIdxStr.IsNumeric() ){
			INT constStringIdx = appAtoi( *constStringIdxStr );
			constString = MMOUI::ParseLocalizedPropertyPath( constStringIdxStr );
		}
		MsgContent = MsgContent.Left( constStringPosStart ) + constString + MsgContent.Right( MsgContent.Len() - constStringPosEnd - 4 );
	}

	//add tag
	UBOOL isShareMapMarkMsg = ParseShareMapMark( MsgContent, TypeId, TargetId, SenderID, SenderName );
	if( isShareMapMarkMsg ){
		return;
	}

	//ImageFilter(MsgContent);

	//if( player && player->ServerID == targeId && TypeId == ECC_Private )

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if (player && player->ActorName == SenderName &&TypeId == ECC_Private)
	{
		PlayerBaseInfo* targetBaseinfo = NULL;

		if (CharType == otRole && SenderID > 0)
		{
			targetBaseinfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( SenderID);
		}
		 

		if (targetBaseinfo)
			AddChatMessage( TypeId , MsgContent , targetBaseinfo->_RoleName, SenderID , TRUE , targeId ,bGM);
		else if(player->ServerID == SenderID)
			AddChatMessage( TypeId , MsgContent , player->ActorName, SenderID , TRUE , targeId ,bGM);
	}
	else
	{
		AddChatMessage( TypeId , MsgContent , SenderName, SenderID ,FALSE,-1,bGM);
	}

	debugf(TEXT("[CHATAFTER] [%s]: %s"), *SenderName, *MsgContent);
	
}

void MMOChatManager::ImageFilter( FString& messageContent )
{
	TCHAR* filter[] = {TEXT("#aq"),TEXT("#bs"),TEXT("#co"),TEXT("#dk"),TEXT("#dy"),TEXT("#vd"),TEXT("#gg"),TEXT("#hx"),TEXT("#lh"),TEXT("#hd"),TEXT("#jy")
					,TEXT("#ka"),TEXT("#ng"),TEXT("#qq"),TEXT("#sj"),TEXT("#wx"),TEXT("#wq"),TEXT("#yw"),TEXT("#yx"),TEXT("#zj")};

	INT num = sizeof(filter)/sizeof(TCHAR*);
	for(INT i=0;i<num;i++)
	{
		FString resName = FString::Printf(TEXT("<IMG SRC='img://MMO_UIFace.%d'>"), i);
		messageContent = messageContent.Replace(filter[i],*resName);
	}
}


void MMOChatManager::AddChatMessage( INT typeId , const FString& messageContent , const FString& senderName , INT senderID , UBOOL iSay , INT targetId, unsigned bGM )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if (player == NULL)
		return;

    if ( ( typeId != ECC_System || typeId != ECC_Hint ) && MMOUI::GetUIManager()->DeclineStrangeChat && senderID != -1 )
    {
		//if reject-strangers state is on, and the sender is neither self nor a friend, reject receiving
        if ( !FriendManager::getSingleton().IsMyFriend(senderID) && player && player->ServerID != senderID )
            return;
    }

	ChatMessage CM;
	CM.ChatTypeId = typeId;
	CM.ChatFilterChannelMsg = messageContent;
	CM.ChatActorID = senderID;
	CM.TargetId = targetId;
	CM.bGM = bGM;
	

	if (player->ServerID == senderID)
	{
		CM.VipType = player->GetVipType();
		CM.isMe = TRUE;
	}
	else
	{
		PlayerBaseInfo* baseinfo = NULL;

		if (GetTypeFromID(senderID) == otRole && senderID > 0)
		{
			baseinfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(senderID);
		}

		if (baseinfo)
		{
			CM.VipType = baseinfo->_VIPType;
		}
	}
	

	INT senderType = GetTypeFromID(senderID);
	
	if( iSay && targetId >= 0 )
	{
		//feedback for my private chat to someone; here variable senderName is the target player, my name is taken from myself
		FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.ItoSay") );
		CM.ChatPlayer = FString::Printf( *temp , *LinkManager::HtmlLink_ChatPlayer( senderName , senderID ) );

		
	}
	else if( senderType == otRole )
	{
		CM.ChatPlayer = LinkManager::HtmlLink_ChatPlayer( senderName , senderID ); 
	}
	else
	{
		//this sender is not a player
		CM.ChatPlayer = senderName;
		
	}

	if (senderType == otNpc)
	{
		//parse the conversation content
		CM.ChatFilterChannelMsg = ParseNpcDialog(CM.ChatFilterChannelMsg);
	}
	else if (senderType == otRole ||
		senderType == otGuild ||
		senderType == otTeam ||
		senderType == otBattleTeam ||
		senderType == otChannel ||
		senderType == otTrumpet)
	{
		if(!bGM)
		{
			MMOUI::ReplaceCensoredWord(CM.ChatFilterChannelMsg, CensoredWord_ChatRecive);
			//CM.ChatFilterChannelMsg = ParseNormalMsg(CM.ChatFilterChannelMsg);
		}
	}
	else
	{
		MMOUI::FixItemColor(CM.ChatFilterChannelMsg);
	}


	ImageFilter(CM.ChatFilterChannelMsg);

	if (typeId != ECC_Trumpet)
	{

		//whether it is a private chat message sent by someone else
		if (typeId == ECC_Private && senderType == otRole && player->ServerID != targetId)
		{
			//private chat UI flashes
			NoitfyShowPrivateTabEffect(CM);
		}

		if (typeId == ECC_Private && messageContent.Len() <= 0)
			return;

		ChatBuffList.AddItem(CM);
	}
	else
	{
		ChatTrumpetList.AddItem(CM);
		isShowTrumpet = TRUE;
	}
	
}

UBOOL MMOChatManager::CheckHasQuerryInfo( INT roleId )
{
	for ( INT i = 0 ; i < tempBuffList.Num() ; i++ )
	{
		if( tempBuffList(i).TargetId == roleId )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void MMOChatManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	//if( evt->GetEventType() == GameEvent_PlayerBaseInfo )
	//{
		//used before, no use now
		//MMOGameEvent_PlayerBaseInfo* baseInfo = (MMOGameEvent_PlayerBaseInfo*)evt;

		//if(baseInfo)
		//{
		//	for ( INT i = tempBuffList.Num()-1 ; i >= 0  ; i-- )
		//	{
		//		if( baseInfo->_RoldID == tempBuffList(i).TargetId )
		//		{
		//			ChatMessage CM = tempBuffList(i);

		//			PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(baseInfo->_RoldID);
		//			if(playerInfo)
		//			{
		//				FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.ItoSay") );
		//				CM.ChatPlayer = FString::Printf( *temp , *LinkManager::HtmlLink_ChatPlayer( playerInfo->_RoleName , playerInfo->_RoleID ) );
		//				ChatBuffList.AddItem(CM);
		//				
		//			}					
		//			tempBuffList.Remove(i);
		//			
		//		}
		//	}

		//}


	//}

	switch(evt->GetEventType())
	{
	case GameEvent_LeaveSelectingRole:
		{
			LoadRecord();
		}
		break;
	}
	

}

void MMOChatManager::AddSystemMessage(const FString& MessageContent)
{
	AddChatMessage( ECC_System , MessageContent , TEXT("") );
}

void MMOChatManager::AddHintMessage(const FString& MessageContent)
{
	AddChatMessage( ECC_Hint , MessageContent , TEXT("") );
}

void MMOChatManager::AddWorldTrumpetMessage()
{
	if (ChatTrumpetList.Num() <= 0)
		return;

	ChatMessage CM;
	CM.ChatTypeId = ECC_Trumpet_World;
	CM.ChatFilterChannelMsg = ChatTrumpetList(0).ChatFilterChannelMsg;
	CM.ChatPlayer = ChatTrumpetList(0).ChatPlayer;
	CM.bGM = ChatTrumpetList(0).bGM;
	ChatBuffList.AddItem(CM);
}

UGFxObject* MMOChatManager::GreateChatObject(ChatMessage& CM ,UMMOGFXPlayer* GFXPlayer)
{
	FString ColorString;
	FString ChannelIconPath;
	
	switch( CM.ChatTypeId )
	{
	case ECC_World:
		ColorString = MMOChatManager::getSingleton().TotalChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().WorldChannelIconPath;
		break;
	case ECC_Nearby:
		ColorString = MMOChatManager::getSingleton().NearChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().NearChannelIconPath;
		break;
	case ECC_Private:
		ColorString = MMOChatManager::getSingleton().PrivateChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().PrivateChannelIconPath;
		break;
	case ECC_Team:
		ColorString = MMOChatManager::getSingleton().TeamChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().TeamChannelIconPath;
		break;
	case ECC_System:
	case ECC_Hint:
		ColorString = MMOChatManager::getSingleton().SystemChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().SystemChannelIconPath;
		break;
	case ECC_Guild:
		ColorString = MMOChatManager::getSingleton().GuildChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().GuildChannelIconPath;
		break;
	case ECC_Trumpet:
		{
			ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().TrumpetChannelIconPath;
		}
		
		break;
	case ECC_Trumpet_World:
		{
			ColorString = MMOChatManager::getSingleton().TrumpetChannelFontColorString;
			ChannelIconPath = MMOChatManager::getSingleton().LabaIconPath;
		}
		break;
	default:
		ColorString = MMOChatManager::getSingleton().TotalChannelFontColorString;
		ChannelIconPath = MMOChatManager::getSingleton().WorldChannelIconPath;
		break;
	}

	FString vipIconPath = getVipIconPath(CM.VipType);

	FString MsgList = TEXT("<img src='img://");
	MsgList += ChannelIconPath + TEXT("'/>"); 

	if (CM.bGM > 0)
	{
		FString gmStr = TEXT("<img src='img://");
		gmStr += GmIconPath + TEXT("'/>"); 
		MsgList += gmStr; 
		//warnf( NAME_Log, TEXT("=================== Chat GM IconPath :%s"),*gmStr );
	}
	
	//warnf( NAME_Log, TEXT("=================== Chat GM :%d"),CM.bGM );

	if (!vipIconPath.IsEmpty())
	{
		MsgList += vipIconPath;
	}

	if( CM.ChatPlayer.IsEmpty() )
	{
		MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
			CM.ChatFilterChannelMsg  + TEXT("</FONT>");
	}
	else
	{
		if (CM.bGM > 0)
		{
			MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,GM_CHAT_COLOR ) + 
				FString::Printf( TEXT("%s ")   , *CM.ChatFilterChannelMsg ) + TEXT("</FONT>");
		}
		else
		{
			MsgList = MsgList + FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*ColorString ) + 
				FString::Printf( TEXT("%s : %s ") , *CM.ChatPlayer  , *CM.ChatFilterChannelMsg ) + TEXT("</FONT>");
		}
		
	}

	UGFxObject* ChatObj = NULL;
	if(GFXPlayer){
		ChatObj = GFXPlayer->CreateObject(TEXT("Object"));
		if(ChatObj)
		{
			ChatObj->SetString(TEXT("htmlText"),MsgList);
			ChatObj->SetInt(TEXT("channelId"),CM.ChatTypeId);
			ChatObj->SetBool(TEXT("isMe"),CM.isMe);
		}
	}

	//warnf(NAME_Log, TEXT("[ChatMsg]%s"), *MsgList);
	return ChatObj;
}

//
//===================================================================================================================
UBOOL MMOChatManager::ParseShareMapMark( const FString &chatContent, INT channelType, INT chatTarget, INT senderID, FString senderName )
{
	FString emptyString;
	FString markMsgContent;
	UBOOL isMarkAddMsg = chatContent.Split( ADD_MAP_USER_MARK_PROFIX, &emptyString, &markMsgContent );
	if( !isMarkAddMsg ){
		return isMarkAddMsg;
	}

	//
	UBOOL addMarkOK = FALSE;
	switch( channelType )
	{
	case ECC_Team:
		{
			UBOOL isInTeam = TeamManager::getSingleton().HasTeam();
			if( !isInTeam ){
				return isMarkAddMsg;
			}

			//do not add messages sent by self
			UBOOL isTeamLeader = TeamManager::getSingleton().IsTeamLeader();
			if( isTeamLeader ){
				return isMarkAddMsg;
			}

			//
			if( senderID != TeamManager::getSingleton().GetTeamLeaderServeId() ){
				//not sent by the party leader
				warnf( NAME_Error, TEXT("======!!!!======= [Recevied unqualified sender[%d] requesting add map marks]"), senderID );
				return TRUE;
			}

			//
			INT myTeamID = TeamManager::getSingleton().GetMyTeamID();
			if( myTeamID != chatTarget ){
				return isMarkAddMsg;
			}

			addMarkOK = TRUE;
		}
		break;
	case ECC_Guild:
		{
			const GuildBaseInfo& myGuildInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			UBOOL isGuildMember = ( myGuildInfo.Id != 0 );
			if( myGuildInfo.Id != chatTarget ){
				return isMarkAddMsg;
			}

			if( myGuildInfo.LeaderName != senderName ){
				//not sent by the guild leader
				warnf( NAME_Error, TEXT("======!!!!======= [Recevied unqualified sender[%d] requesting add map marks]"), senderID );
				return TRUE;
			}

			//do not add messages sent by self
			UBOOL isGuildLeader = myGuildInfo.LeaderName == GetPlayerRoleName();
			if( isGuildLeader ){
				return isMarkAddMsg;
			}
			//
			addMarkOK = TRUE;
		}
		break;
	default:
		return isMarkAddMsg;
	}

	//
	if( addMarkOK ){
		TArray<FString> markStrData;
		markMsgContent.ParseIntoArray( &markStrData, TEXT(","), FALSE );
		INT cnt = markStrData.Num();
		if( cnt < 7 ){
			return isMarkAddMsg;
		}
		MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
		//
		FVector markPos;
		markPos.X = appAtoi( *markStrData(4) );
		markPos.Y = appAtoi( *markStrData(5) );
		markPos.Z = appAtoi( *markStrData(6) );

		INT markType = appAtoi( *markStrData(1) );
		INT markMapID = appAtoi( *markStrData(2) );
		FString regionName = markStrData(3);
		FMapMark *pNewMapMark = mapDataMgr.AddMapMarkData( markMapID, regionName, markStrData(0), markType, markPos );

		//if the current map is open
		UMMOUIWorldMap *pWorldMapPanel = ( UMMOUIWorldMap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WorldMap );
		if( pWorldMapPanel != NULL ){
			FExportMap &curWorldMapInfo = mapDataMgr.GetCurWorldMap();
			FInMapRegion *pCurReginInMap = mapDataMgr.GetCurWorldMapRegion();
			if( curWorldMapInfo.ServerID == markMapID && pCurReginInMap != NULL && pCurReginInMap->VolumeName == regionName ){
				FUserMarkInMapUI *pMarkUI = mapDataMgr.AddMapMarkToUI( pNewMapMark );
			}
		}
	}

	return isMarkAddMsg;
}

FString MMOChatManager::getVipIconPath(INT vipType)
{
	FString vipIconPath = TEXT("<img src='img://");

	switch(vipType)
	{
	case eVIPTyp_LV1:
		vipIconPath += VipLevel1IconPath + TEXT("'/>");
		break;
	case eVIPTyp_LV2:
		vipIconPath += VipLevel2IconPath + TEXT("'/>");
		break;
	case eVIPTyp_LV3:
		vipIconPath += VipLevel3IconPath + TEXT("'/>");
		break;
	default:
		vipIconPath = "";
		break;
	}
	return vipIconPath;
}

void MMOChatManager::AddScreenMessage()
{
	//FString strScreen= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NoPrivatePlayer"));
	//MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Table,TEXT("MMOGame.Chat.NoPrivatePlayer"));
	//AddSystemMessage(strScreen);
	NotifyShowChatErrorMsg(TEXT("MMOGame.Chat.NoPrivatePlayer"));
}

void MMOChatManager::SaveRecord()
{
	INT cnt = ChatFilterChannelList.Num();

	if(cnt > 0)
	{
		FArchive* pArchive = 0;
		if(MMOFacade::getSingleton().BeginExportAccountData(&pArchive, ChatDataFile))
		{
			pArchive->Serialize(&cnt, sizeof(INT));

			for (INT i = 0; i < cnt; ++i)
			{
				pArchive->Serialize( (void*)( &ChatFilterChannelList(i).ChannelFilterType ) , sizeof(INT) );	

				INT nameLen = ChatFilterChannelList(i).ChannelName.Len()*sizeof(TCHAR);
				pArchive->Serialize( (void*)( &nameLen ) , sizeof(INT) );

				pArchive->Serialize( (void*)( ChatFilterChannelList(i).ChannelName.GetCharArray().GetData() ) , nameLen );

				pArchive->Serialize( (void*)( &ChatFilterChannelList(i).IsDefaultChannel ) , sizeof(UBOOL) );
			
			}

			MMOFacade::getSingleton().EndExportAccountData(pArchive);
		}

	}
	
}
void MMOChatManager::LoadRecord()
{
	FArchive* pArchive = 0;
	MMOFacade::getSingleton().BeginLoadAccountBinary(&pArchive, ChatDataFile);
	if(pArchive)
	{
		INT cnt = 0;
		pArchive->Serialize(&cnt, sizeof(INT));

		for (INT i = 0; i < cnt; ++i)
		{
			ChatFilterChannelMessage chatFilter;
			pArchive->Serialize( &chatFilter.ChannelFilterType , sizeof(INT) );
			INT nameLen = 0;
			pArchive->Serialize( &nameLen, sizeof(nameLen) );

			TCHAR chatChannelName[1024] = {0};
			pArchive->Serialize( &chatChannelName, nameLen );
			chatFilter.ChannelName = chatChannelName;

			pArchive->Serialize( &chatFilter.IsDefaultChannel, sizeof(UBOOL) );

			SetFilterChannel(i,chatFilter.ChannelName,chatFilter.ChannelFilterType,chatFilter.IsDefaultChannel);
		}

		//update uichat
		//MMOUI::GetUIManager()->delegateSetFilterChannelNum( ChatFilterChannelList.Num() );

		//for (INT i = 0 ; i < ChatFilterChannelList.Num() ; i++ )
		//{

			//MMOUI::GetUIManager()->delegateSetFilterChannel( i , ChatFilterChannelList(i).ChannelName );
		//}
	}
	else
	{
		AddDefaultFilterChannel();
	}

	MMOGameEvent_Any evt(GameEvent_Chat_UpdateDropDownList);
	MMOUI::PostGameEvent(&evt);

	MMOFacade::getSingleton().EndLoadAccountBinary(pArchive);
}

void MMOChatManager::NotifyUpdateChannelTabClicked()
{
	if (_IsAddChannelTab)
	{
		MMOGameEvent_Any evt(GameEvent_UpdateChannelTab);
		MMOUI::PostGameEvent(&evt);
	}
	_IsAddChannelTab = FALSE;
}

INT MMOChatManager::GetChatFilterChannelListNum()
{
	return ChatFilterChannelList.Num();
}

BOOL MMOChatManager::CheckTrumpetItem()
{
	if (g_bGMBuff)
	{
		return TRUE;
	}

	BOOL bTrumpetItem = FALSE;
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (pMainPlayer)
	{
		for(INT i = 0; i < pMainPlayer->ItemManager->BagItemList.Num(); i++)
		{
			UItem* item = pMainPlayer->ItemManager->BagItemList(i);
			if (item)
			{
				if (item->ItemId == TrumpetID)
				{
					bTrumpetItem = TRUE;
					break;
				}
			}
		}
	}
	return bTrumpetItem;
}

void MMOChatManager::RemoveTrumpet()
{
	if (ChatTrumpetList.Num())
	{
		ChatTrumpetList.Remove(0);
	}
}

FString MMOChatManager::GetCurrentTrumpet()
{
	if (ChatTrumpetList.Num())
	{
		return ChatTrumpetList(0).ChatFilterChannelMsg;
	}
	else
	{
		return "";
	}
}

void MMOChatManager::AddDefaultChatFilter()
{
	INT ChannelFilterType = 0;
	ChannelFilterType |= 0x1<<ECC_World;
	ChannelFilterType |= 0x1<<ECC_Nearby;
	ChannelFilterType |= 0x1<<ECC_Guild;
	ChannelFilterType |= 0x1<<ECC_Team;
	ChannelFilterType |= 0x1<<ECC_Private;
	ChannelFilterType |= 0x1<<ECC_System;
	ChannelFilterType |= 0x1<<ECC_Hint;

	SetDefaultChatFilter(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.All") ) , ChannelFilterType , TRUE);

	//ChannelFilterType = 0;
	//ChannelFilterType |= 0x1<<ECC_Nearby;
	//SetDefaultChatFilter(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Nearby") ), ChannelFilterType , FALSE);

	//ChannelFilterType = 0;
	//ChannelFilterType |= 0x1<<ECC_Guild;
	//SetDefaultChatFilter(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Guild") ), ChannelFilterType , FALSE );

	//ChannelFilterType = 0;
	//ChannelFilterType |= 0x1<<ECC_Team;
	//SetDefaultChatFilter(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Team") ),ChannelFilterType , FALSE );

	//ChannelFilterType = 0;
	//ChannelFilterType |= 0x1<<ECC_Private;
	//SetDefaultChatFilter(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Private") )  , ChannelFilterType , FALSE );

	//ChannelFilterType = 0;
	//ChannelFilterType |= 0x1<<ECC_Hint;
	//SetDefaultChatFilter(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Hint") )  , ChannelFilterType , FALSE);
}

void MMOChatManager::SetDefaultChatFilter(const FString& ChannelName , INT FilterChannelType , UBOOL defaultChannel)
{
	ChatFilterChannelMessage FilterChannelMsg;
	FilterChannelMsg.ChannelFilterType = FilterChannelType;
	FilterChannelMsg.IsDefaultChannel = defaultChannel;
	FilterChannelMsg.ChannelName = ChannelName;
	DefaultChatFilterChannelList.AddItem( FilterChannelMsg );
}

FString MMOChatManager::ParseNpcDialog(const FString& context)
{
	ParseContext ctx;
	DialogContext dtx;		
	UITextParser::getSingleton().Parse(ctx, dtx, context );
	return dtx.TempText;

}

static const FString g_ChatMsgRemoveWord[] =
{
	TEXT("　"),
	TEXT("\\t"),
	TEXT("\\n"),
	TEXT("\\N"),
	TEXT("\\r"),
	TEXT("\\R"),
	TEXT("<br>"), 
	TEXT("<BR>"),
	TEXT("<Br>"),
	TEXT("<bR>"),
	TEXT("<p>"), 
	TEXT("<P>"),
	TEXT("&nbsp;"),
	TEXT("&lt;br&gt;"),
	TEXT("&lt;BR&gt;"),
	TEXT("&lt;Br&gt;"),
	TEXT("&lt;bR&gt;"),
	TEXT("<br&gt;"),
	TEXT("<BR&gt;"),
	TEXT("<Br&gt;"),
	TEXT("<bR&gt;"),
	TEXT("&lt;br>"),
	TEXT("&lt;BR>"),
	TEXT("&lt;Br>"),
	TEXT("&lt;bR>"),
	TEXT("&lt;p&gt;"),
	TEXT("&lt;P&gt;"),
	TEXT("<p&gt;"),
	TEXT("<P&gt;"),
	TEXT("&lt;p>"),
	TEXT("&lt;P>"),
	TEXT("&#60"),
	TEXT("&#62"),
	TEXT("&#10"),
	TEXT("&#13"),
}; // N entries

FString MMOChatManager::ParseNormalMsg(const FString& msg)
{
	FString parseMsg = msg;

	INT removeNum = sizeof(g_ChatMsgRemoveWord) / sizeof(FString);
	for (INT i = 0; i < removeNum; ++ i)
	{
		parseMsg = parseMsg.Replace(*(g_ChatMsgRemoveWord[i]), TEXT("\0"), TRUE);
	}

//	parseMsg = parseMsg.Replace(TEXT("color"), TEXT("*****"), TRUE);
	MMOUI::ReplaceCensoredWord(parseMsg,CensoredWord_ChatRecive);
//	parseMsg = parseMsg.Replace(TEXT("size"), TEXT("sizes"), TRUE);
//	parseMsg = parseMsg.Replace(TEXT("color"), TEXT("*****"), TRUE);
//	parseMsg = parseMsg.Replace(TEXT("face"), TEXT("****"), TRUE);

	return parseMsg;
}

BOOL MMOChatManager::IsChatFocus()
{
	return bFocus;
	
}

void MMOChatManager::NotifySetChannelByTab()
{
	MMOGameEvent_Any evt(GameEvent_SetChannelByTab);
	MMOUI::PostGameEvent(&evt);
}

BOOL MMOChatManager::ParsePrivateChatMessage(FString Content)
{
	BOOL v = TRUE;

	if( Content.Left(1) == TEXT("/"))
	{
		int Pos = Content.InStr( TEXT(" "));
		if( Pos < 0)
		{
			v =  FALSE;//format error
		}
		else
		{
			FString contentString = Content.RightChop(Pos+1);
			FString targetName = Content.Left(Pos).RightChop(1);
			if(/* contentString.IsEmpty() || */targetName.IsEmpty())
			{
				v  = FALSE;//format error
			}
		}
		
	}
	else //Format error
	{
		v = FALSE;
	}
	
	return v;
}

void MMOChatManager::NoitfyShowPrivateTabEffect(ChatMessage CM)
{
	for(int i = 0; i < ChatFilterChannelList.Num(); i++)
	{
		
		if( ChatFilterChannelList(i).ChannelFilterType ==  (ChatFilterChannelList(i).ChannelFilterType & ~(0x1<<ECC_Private)) )
		{
			MMOGameEvent_Any evt(GameEvent_ShowPrivateTabEffect);
			MMOUI::PostGameEvent(&evt);

			AddPrivateListMsg(CM);
		}
	}
}

BOOL MMOChatManager::IsImgChat(FString content)
{
	FString img = content.Left(3);

	TCHAR* filter[] = {TEXT("#aq"),TEXT("#bs"),TEXT("#co"),TEXT("#dk"),TEXT("#dy"),TEXT("#vd"),TEXT("#gg"),TEXT("#hx"),TEXT("#lh"),TEXT("#hd"),TEXT("#jy")
		,TEXT("#ka"),TEXT("#ng"),TEXT("#qq"),TEXT("#sj"),TEXT("#wx"),TEXT("#wq"),TEXT("#yw"),TEXT("#yx"),TEXT("#zj")};

	INT num = sizeof(filter)/sizeof(TCHAR*);
	for(INT i=0;i<num;i++)
	{
		if (img == filter[i])
			return FALSE;
	}
	return TRUE;
}

void MMOChatManager::CleanChannelChat(INT id)
{
	if(id < ChatFilterChannelList.Num())
	{
		ChatFilterChannelList(id).ChatMessageList.Empty();
	}
}

UBOOL MMOChatManager::ParseSpecialCommand(const FString& cmd)
{
	if (cmd.InStr(TEXT("@sdgo")) != INDEX_NONE)
	{
		INT containerId = E_CONSLOT_EQUIPPED;
		INT slotId = -1;
		MMOFacade::getSingleton().FindFirstItemPosInContainer(ESCI_TeleportRing, containerId, slotId);
		if (slotId < 0)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NotEquippedItem"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOGame.Chat.NotEquippedItem"));
			return FALSE;
		}

		MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();
		FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem(E_CONSLOT_EQUIPPED, slotId);
		if (pItemInfo)
		{
			INT curDurPect = equipMgr.GetEquipCurDurPect(pItemInfo);
			if (curDurPect == 0)
			{
				return FALSE;
			}
		}


		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if(curMapInfo.UseTeleportRingDisabled==TRUE)
		{
			return FALSE;
		}

		INT paramIndex = cmd.InStr(TEXT("@sdgo")) + FString(TEXT("@sdgo")).Len();
		FString paramStr = cmd.Mid(paramIndex);
		TArray<FString> paramArray;
		paramStr.ParseIntoArray(&paramArray, TEXT(" "), TRUE);

		if (paramArray.Num() == 2)
		{
			UINT16 posX = appAtoi(*(paramArray(0)));
			UINT16 posY = appAtoi(*(paramArray(1)));
			CMessageHandler::GetInstance().Send_RoleTeleport(posX, posY);
			return TRUE;
		}
	}
	else if (cmd.InStr(TEXT("@searching")) != INDEX_NONE)
	{
		INT containerId = E_CONSLOT_EQUIPPED;
		INT slotId = -1;
		MMOFacade::getSingleton().FindFirstItemPosInContainer(ESCI_SearchNecklace, containerId, slotId);
		if (slotId < 0)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NotEquippedItem"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOGame.Chat.NotEquippedItem"));
			return FALSE;
		}

		INT paramIndex = cmd.InStr(TEXT("@searching")) + FString(TEXT("@searching")).Len();
		FString paramStr = cmd.Mid(paramIndex);
		TArray<FString> paramArray;
		paramStr.ParseIntoArray(&paramArray, TEXT(" "), TRUE);

		if (paramArray.Num() == 1)
		{
			CMessageHandler::GetInstance().Send_QueryRoleLocation(paramArray(0));
			return TRUE;
		}
	}
	else if (cmd == TEXT("@天地合一"))
	{
		if (!TeamManager::getSingleton().HasTeam())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			return FALSE;
		}

		if (!TeamManager::getSingleton().IsTeamLeader())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotLeader"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotLeader"));
			return FALSE;
		}

		INT containerId = E_CONSLOT_EQUIPPED;
		INT slotId1 = -1;
		INT slotId2 = -1;
		INT slotId3 = -1;
		MMOFacade::getSingleton().FindFirstItemPosInContainer(ESCI_MemNecklace, containerId, slotId1);
		MMOFacade::getSingleton().FindFirstItemPosInContainer(ESCI_MemRing, containerId, slotId2);
		MMOFacade::getSingleton().FindFirstItemPosInContainer(ESCI_MemHand, containerId, slotId3);
		if (slotId1 < 0 || slotId2 < 0 || slotId3 < 0)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.NotEquippedItem"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOGame.Chat.NotEquippedItem"));
			return FALSE;
		}

		CMessageHandler::GetInstance().Send_RecallTeamMember();
		return TRUE;
	}
	else if (cmd == TEXT("@允许天地合一"))
	{
		if (!TeamManager::getSingleton().HasTeam())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			return FALSE;
		}

		CMessageHandler::GetInstance().Send_ChangeRecallState(TRUE);
		return TRUE;
	}
	else if (cmd == TEXT("@取消天地合一"))
	{
		if (!TeamManager::getSingleton().HasTeam())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			return FALSE;
		}

		CMessageHandler::GetInstance().Send_ChangeRecallState(FALSE);
		return TRUE;
	}
	else if (cmd.InStr(TEXT("@召唤")) != INDEX_NONE)
	{
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		/*int VipLevel = mainPlayer->GetVipType();
		if (VipLevel <= 0)
			return FALSE;*/
		INT paramIndex = cmd.InStr(TEXT("@召唤")) + FString(TEXT("@召唤")).Len();
		FString paramStr = cmd.Mid(paramIndex);
		TArray<FString> paramArray;
		paramStr.ParseIntoArray(&paramArray, TEXT(" "), TRUE);

		if (paramArray.Num() == 2)
		{
			UINT16 Bet_type = appAtoi(*(paramArray(0)));
			UINT16 Bet_count = appAtoi(*(paramArray(1)));

			if(Bet_type < 1 || Bet_type > 12)
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errBetType"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errBetType"));
				return FALSE;
			}
			if(Bet_count > 10000)
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errDiceTooMany"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errDiceTooMany"));
				return FALSE;
			}
			if (!MMOFacade::getSingleton().CheckEnoughMoney(E_MONEYTYPE_04,Bet_count * 100))
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Recharge.DiceFailed"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOGame.Recharge.DiceFailed"));
				return FALSE;
			}
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Dice.Buy.Success"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOGame.Dice.Buy.Success"));

			CMessageHandler::GetInstance().Send_Dice_bet(Bet_type, Bet_count);
			return TRUE;
		}
	}
	else if (cmd == TEXT("@传送"))
	{
		if (!TeamManager::getSingleton().HasTeam())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotExist"));
			return FALSE;
		}

		CMessageHandler::GetInstance().Send_Lovers_Teleport();
		return TRUE;
	}
	return FALSE;
}

void MMOChatManager::AddHintItem(ChatMessage CM)
{
	ChatFilterChannelList(HINTPOS).ChatMessageList.AddItem( CM );
	if( ChatFilterChannelList(HINTPOS).ChatMessageList.Num() > MSGLIMT)
	{
		ChatFilterChannelList(HINTPOS).ChatMessageList.Remove( 0 );
	}
}

void MMOChatManager::AddChatErrorMsg(FString msg)
{
	//FString error = MMOUI::ParseLocalizedPropertyPath(msg);
	//AddSystemMessage(error);
	NotifyShowChatErrorMsg(msg);
}

void MMOChatManager::AddPrivateListMsg(ChatMessage CM)
{
	ChatPrivateList.AddItem(CM);
	if (ChatPrivateList.Num() > PRIVATEMSGLIMT)
	{
		ChatPrivateList.Remove(0);
	}
}

void MMOChatManager::AddPrivateMsgToBufList()
{
	for(INT i = 0; i< ChatPrivateList.Num();i++)
	{
		ChatBuffList.AddItem(ChatPrivateList(i));
	}
	ChatPrivateList.Empty();
}


void MMOChatManager::NotifyShowChatErrorMsg(FString errMsg)
{
	MMOGameEvent_Chat_ShowChatErrorMsg evt(errMsg);
	MMOUI::PostGameEvent(&evt);
}
