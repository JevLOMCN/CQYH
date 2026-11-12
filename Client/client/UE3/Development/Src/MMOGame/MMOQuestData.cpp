#include "stdafx.h"
#include "EngineAudioDeviceClasses.h"
#include "MMOFacade.h"
#include "MMOQuestData.h"
#include "UITextParser.h"
#include "MasterStudentManager.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"		
#include "CommonGameEvents.h"
#include "MMOAchievementManager.h"
#include "MMOGuideManager.h"
#include "MMOChatManager.h"
#include "MMOGameClasses.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuildManager.h"
#include "DoodadManager.h"
#include "MMODropManager.h"
#include "MMOQuestManager.h"
#include "FriendManager.h"
#include "ActivityManager.h"
#include "MMOVipManager.h"
#include "MMOSkillManager.h"
#include "MMOClientVariableManager.h"
#include "client_option_def.h"

AcceptedQuestData& AcceptedQuestData::operator =(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& src)
{
	_Assign(src);
	return *this;
}

void AcceptedQuestData::_Assign(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST &src)
{
	Id = src.nQuestID;
	AcceptTime = src.dwAcceptTime;
	appMemcpy(&KillNpcCount, &src.byKillNpcCount, sizeof(UINT8) * MAX_CONDITION_PER_QUEST);
	appMemcpy(&KillDoodadCount, &src.byOpenDoodadCount, sizeof(UINT8) * MAX_CONDITION_PER_QUEST);
	appMemcpy(&KillPlayerCount, &src.nKillPlayerCount, sizeof(UINT8) * CHECK_KILL_PLAYER_CONDITION);
	appMemcpy(&QuestVariable, &src.nVariable, sizeof(INT32) * MAX_VARIABLE_PER_QUEST);

	appMemset(&RoleVariable, 0, sizeof(INT32) * MAX_ROLE_VARIABLE_COUNT);
	appMemset(&ClientVariable, 0, sizeof(INT32) * MAX_CLIENT_VARIABLE_COUNT);
}

AcceptedQuestData::AcceptedQuestData(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& src)
{
	_Init();
	_Assign(src);
}

AcceptedQuestData::AcceptedQuestData()
{
	_Init();
}

void AcceptedQuestData::_Init()
{
	IsFailed = false;
	IsTraced = true;
	IsExpanded = true;
	IsNewQuest = false;
	Id = INVALID_QUEST_ID;
	AcceptTime = 0;
	LeftTime = 0;
	Level = 0;
	appMemzero(&KillNpcCount, sizeof(UINT8) * MAX_CONDITION_PER_QUEST);
	appMemzero(&KillDoodadCount, sizeof(UINT8) * MAX_CONDITION_PER_QUEST);
	appMemzero(&KillPlayerCount, sizeof(UINT8) * CHECK_KILL_PLAYER_CONDITION);
	appMemzero(&RoleVariable, sizeof(INT32) * MAX_ROLE_VARIABLE_COUNT);
	appMemzero(&QuestVariable, sizeof(INT32) * MAX_VARIABLE_PER_QUEST);
	appMemzero(&ClientVariable, sizeof(INT32) * MAX_CLIENT_VARIABLE_COUNT);
}

//////////////////////////////////////////////////////////////////////////
UQuest* QuestDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	UQuest* quest = Cast<UQuest>(GEngine->StaticConstructObject(UQuest::StaticClass()));
	quest->AddToRoot();
	return quest;
}

void QuestDataProcessor::DestroyDataObject(UQuest* obj)
{
	if (obj)
	{
		obj->RemoveFromRoot();
	}
}

void QuestDataProcessor::ParseData(UQuest* obj, class CTabFile* dataRow, int rowIdx)
{
	if (!obj)
	{
		return;
	}

	if (!obj->IsValid())
	{
		return;
	}

	obj->QuestTempID = TReadDataValue_Int<QuestDataManager>(TEXT("QuestTempID"), dataRow, rowIdx);
	obj->QuestName = TReadDataValue_FString<QuestDataManager>(TEXT("QuestName"), dataRow, rowIdx);
	obj->QuestType = TReadDataValue_Int<QuestDataManager>(TEXT("QuestType"), dataRow, rowIdx);
	obj->QuestChapter = TReadDataValue_Int<QuestDataManager>(TEXT("QuestChapter"), dataRow, rowIdx);
	obj->QuestStage = TReadDataValue_Int<QuestDataManager>(TEXT("QuestStage"), dataRow, rowIdx);
	obj->QuestLevel = TReadDataValue_Int<QuestDataManager>(TEXT("QuestLevel"), dataRow, rowIdx);
	obj->CircleID = TReadDataValue_Int<QuestDataManager>(TEXT("CircleID"), dataRow, rowIdx);
	obj->StarLevel = TReadDataValue_Int<QuestDataManager>(TEXT("StarLevel"), dataRow, rowIdx);
	obj->ResetType = TReadDataValue_Int<QuestDataManager>(TEXT("ResetType"), dataRow, rowIdx);
	obj->nResetTime = TReadDataValue_Int<QuestDataManager>(TEXT("nResetTime"), dataRow, rowIdx);	                                                        		                                                        
	obj->CanAbandon = TReadDataValue_Int<QuestDataManager>(TEXT("CanAbandon"), dataRow, rowIdx);				                                                        
	obj->CanShare = TReadDataValue_Int<QuestDataManager>(TEXT("CanShare"), dataRow, rowIdx);					                                                        
	obj->CanPublish = TReadDataValue_Int<QuestDataManager>(TEXT("CanPublish"), dataRow, rowIdx);				                                                        
	obj->DurationLimit = TReadDataValue_Int<QuestDataManager>(TEXT("DurationLimit"), dataRow, rowIdx);			                                                        

	obj->AcceptQuestDesc = TReadDataValue_FString<QuestDataManager>(TEXT("AcceptQuestDesc"), dataRow, rowIdx);                                         
	obj->QuestDescript = TReadDataValue_FString<QuestDataManager>(TEXT("QuestDescript"), dataRow, rowIdx);			                                    
	obj->QuestCompletedDescript = TReadDataValue_FString<QuestDataManager>(TEXT("QuestCompletedDescript"), dataRow, rowIdx);			                        
	obj->AcceptQuestDialog = TReadDataValue_FString<QuestDataManager>(TEXT("AcceptQuestDialog"), dataRow, rowIdx);		                                    
	obj->PreSubmitQuestDialog = TReadDataValue_FString<QuestDataManager>(TEXT("PreSubmitQuestDialog"), dataRow, rowIdx);	                                    
	obj->CompleteQuestDialog = TReadDataValue_FString<QuestDataManager>(TEXT("CompleteQuestDialog"), dataRow, rowIdx);	                                    
           
	obj->QuestSoundAccept = NULL;
	obj->QuestSoundAcceptPath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundAccept"), dataRow, rowIdx);
	
	for (int i = 0; i < obj->QuestPageSoundAccept.Num(); ++ i)
	{
		obj->QuestPageSoundAccept(i).PageSound = NULL;
		FString pageSoundName = FString::Printf(TEXT("QuestPageSoundAccept.PageSound%d"), i + 1);
		obj->QuestPageSoundAccept(i).PageSoundPath = TReadDataValue_FString<QuestDataManager>(*pageSoundName, dataRow, rowIdx);
	}

	obj->QuestSoundAcceptedIcon = NULL;    
	obj->QuestSoundAcceptedIconPath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundAcceptedIcon"), dataRow, rowIdx);

	obj->QuestSoundAccepted = NULL;                                                
	obj->QuestSoundAcceptedPath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundAccepted"), dataRow, rowIdx);

	obj->QuestSoundUnderway = NULL;                                            
	obj->QuestSoundUnderwayPath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundUnderway"), dataRow, rowIdx);

	obj->QuestSoundSubmit = NULL;                                                  
	obj->QuestSoundSubmitPath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundSubmit"), dataRow, rowIdx);
 
	for (int i = 0; i < obj->QuestPageSoundSubmit.Num(); ++ i)
	{
		obj->QuestPageSoundSubmit(i).PageSound = NULL;
		FString pageSoundName = FString::Printf(TEXT("QuestPageSoundSubmit.PageSound%d"), i + 1);
		obj->QuestPageSoundSubmit(i).PageSoundPath = TReadDataValue_FString<QuestDataManager>(*pageSoundName, dataRow, rowIdx);
	}

	obj->QuestSoundCompleteIcon = NULL;                                     
	obj->QuestSoundCompleteIconPath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundCompleteIcon"), dataRow, rowIdx);

	obj->QuestSoundComplete = NULL;                                              
	obj->QuestSoundCompletePath = TReadDataValue_FString<QuestDataManager>(TEXT("QuestSoundComplete"), dataRow, rowIdx);

	obj->EmergencyRate = TReadDataValue_Int<QuestDataManager>(TEXT("EmergencyRate"), dataRow, rowIdx);                                                        
	obj->EmergencyCD = TReadDataValue_Int<QuestDataManager>(TEXT("EmergencyCD"), dataRow, rowIdx);                                                              
	obj->EmergencyTipsType = TReadDataValue_Int<QuestDataManager>(TEXT("EmergencyTipsType"), dataRow, rowIdx);		                                    
                                                        
	obj->StartType = TReadDataValue_Int<QuestDataManager>(TEXT("StartType"), dataRow, rowIdx);
	obj->StartNPCNew = NULL;                                            
	obj->StartNPCNewId = TReadDataValue_Int<QuestDataManager>(TEXT("StartNPCNew"), dataRow, rowIdx);
	obj->StartNPCMap = TReadDataValue_Int<QuestDataManager>(TEXT("StartNPCMap"), dataRow, rowIdx);
	obj->StartNpcFly = TReadDataValue_Int<QuestDataManager>(TEXT("StartNpcFly"), dataRow, rowIdx);			                                            
	obj->StartItem = NULL;	                                            
	obj->StartItemID = TReadDataValue_Int<QuestDataManager>(TEXT("StartItem"), dataRow, rowIdx);  				                                            
	obj->StartWithDialog = TReadDataValue_Int<QuestDataManager>(TEXT("StartWithDialog"), dataRow, rowIdx);                                                  
	obj->StartOverWithDialog = TReadDataValue_Int<QuestDataManager>(TEXT("StartOverWithDialog"), dataRow, rowIdx);                                                 
	obj->AutoStartDialogHeadIcon = NULL;		                                
	obj->AutoStartDialogHeadIconId = TReadDataValue_Int<QuestDataManager>(TEXT("AutoStartDialogHeadIcon"), dataRow, rowIdx);

	obj->FinishType = TReadDataValue_Int<QuestDataManager>(TEXT("FinishType"), dataRow, rowIdx);
	obj->FinishNpcNew = NULL;		                                            
	obj->FinishNpcNewId = TReadDataValue_Int<QuestDataManager>(TEXT("FinishNpcNew"), dataRow, rowIdx);
	obj->FinishItem = NULL;                                        
	obj->FinishItemID = TReadDataValue_Int<QuestDataManager>(TEXT("FinishItem"), dataRow, rowIdx);                                                        
	obj->FinishWithDialog = TReadDataValue_Int<QuestDataManager>(TEXT("FinishWithDialog"), dataRow, rowIdx);                                                    
	obj->FinishOverWithDialog = TReadDataValue_Int<QuestDataManager>(TEXT("FinishOverWithDialog"), dataRow, rowIdx);                                                
	obj->AutoFinishDialogHeadIcon = NULL;                                     
	obj->AutoFinishDialogHeadIconId = TReadDataValue_Int<QuestDataManager>(TEXT("AutoFinishDialogHeadIcon"), dataRow, rowIdx);;
	obj->AutoStartNextID = TReadDataValue_Int<QuestDataManager>(TEXT("AutoStartNextID"), dataRow, rowIdx);                                                      

	obj->MaxCompleteCount = TReadDataValue_Int<QuestDataManager>(TEXT("MaxCompleteCount"), dataRow, rowIdx);				                                    
	obj->ClassLimit	 = TReadDataValue_Int<QuestDataManager>(TEXT("ClassLimit"), dataRow, rowIdx);		                                    
	obj->SexLimit = TReadDataValue_Int<QuestDataManager>(TEXT("SexLimit"), dataRow, rowIdx);				                                    
	obj->RelationLimit = TReadDataValue_Int<QuestDataManager>(TEXT("RelationLimit"), dataRow, rowIdx);   

	UQuest* PreQuestTempID1 = NULL;		                                            
	obj->PreQuestId1 = TReadDataValue_Int<QuestDataManager>(TEXT("PreQuestTempID1"), dataRow, rowIdx);
	UQuest* PreQuestTempID2 = NULL;		                                            
	obj->PreQuestId2 = TReadDataValue_Int<QuestDataManager>(TEXT("PreQuestTempID2"), dataRow, rowIdx);
	UBuff* BuffLimit = NULL;
	obj->BuffLimitId = TReadDataValue_Int<QuestDataManager>(TEXT("BuffLimit"), dataRow, rowIdx);

	obj->BuffLevelLimit = TReadDataValue_Int<QuestDataManager>(TEXT("BuffLevelLimit"), dataRow, rowIdx);			                                            
	obj->MaxLevelLimit = TReadDataValue_Int<QuestDataManager>(TEXT("MaxLevelLimit"), dataRow, rowIdx);			                                            
	obj->MinLevelLimit = TReadDataValue_Int<QuestDataManager>(TEXT("MinLevelLimit"), dataRow, rowIdx);			                                            
	obj->ReputationForceID = TReadDataValue_Int<QuestDataManager>(TEXT("ReputationForceID"), dataRow, rowIdx);		                                            
	obj->ReputationMaxLimit = TReadDataValue_Int<QuestDataManager>(TEXT("ReputationMaxLimit"), dataRow, rowIdx);		                                            
	obj->ReputationMinLimit = TReadDataValue_Int<QuestDataManager>(TEXT("ReputationMinLimit"), dataRow, rowIdx);	
	          
	obj->MoneyTypeLimit = TReadDataValue_Int<QuestDataManager>(TEXT("MoneyTypeLimit"), dataRow, rowIdx);                                                                     
	obj->MoneyAmountLimit = TReadDataValue_Int<QuestDataManager>(TEXT("MoneyAmountLimit"), dataRow, rowIdx);		                                            
	obj->YearLimit = TReadDataValue_Int<QuestDataManager>(TEXT("YearLimit"), dataRow, rowIdx);				                                            
	obj->MonthLimit = TReadDataValue_Int<QuestDataManager>(TEXT("MonthLimit"), dataRow, rowIdx);	                                                     
	obj->StartWeekday = TReadDataValue_Int<QuestDataManager>(TEXT("StartWeekday"), dataRow, rowIdx);			                                            
	obj->WeekdayDuration = TReadDataValue_Int<QuestDataManager>(TEXT("WeekdayDuration"), dataRow, rowIdx);	                                            
	obj->StartMonthday = TReadDataValue_Int<QuestDataManager>(TEXT("StartMonthday"), dataRow, rowIdx);			                                            
	obj->MonthDayDuration = TReadDataValue_Int<QuestDataManager>(TEXT("MonthDayDuration"), dataRow, rowIdx);
                                        
	obj->nAcceptTimeStart = TReadDataValue_Int<QuestDataManager>(TEXT("nAcceptTimeStart"), dataRow, rowIdx);                                                                   
	obj->nAcceptTimeEnd = TReadDataValue_Int<QuestDataManager>(TEXT("nAcceptTimeEnd"), dataRow, rowIdx);                                                                      
	obj->AcceptStartHour = TReadDataValue_Int<QuestDataManager>(TEXT("AcceptStartHour"), dataRow, rowIdx);			                                        
	obj->AcceptStartMinute = TReadDataValue_Int<QuestDataManager>(TEXT("AcceptStartMinute"), dataRow, rowIdx);			                                        
	obj->AcceptEndHour = TReadDataValue_Int<QuestDataManager>(TEXT("AcceptEndHour"), dataRow, rowIdx);				                                        
	obj->AcceptEndMinute = TReadDataValue_Int<QuestDataManager>(TEXT("AcceptEndMinute"), dataRow, rowIdx);
	
	for (int i = 0; i < obj->ItemLimit.Num(); ++ i)
	{
		obj->ItemLimit(i).CheckItem = NULL;
		FString text = FString::Printf(TEXT("ItemLimit.CheckItemID%d"), i + 1);
		obj->ItemLimit(i).CheckItemID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("ItemLimit.CheckItemCnt%d"), i + 1);
		obj->ItemLimit(i).CheckItemCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("ItemLimit.Description%d"), i + 1);
		obj->ItemLimit(i).Description = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
	}
	 
	for (int i = 0; i < obj->VarLimit.Num(); ++ i)
	{
		FString text = FString::Printf(TEXT("VarLimit.CheckVarType%d"), i + 1);
		obj->VarLimit(i).CheckVarType = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("VarLimit.CheckVarID%d"), i + 1);
		obj->VarLimit(i).CheckVarID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("VarLimit.CheckVarCnt%d"), i + 1);
		obj->VarLimit(i).CheckVarCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("VarLimit.Description%d"), i + 1);
		obj->VarLimit(i).Description = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
	}

	obj->OfferCurrencyType = TReadDataValue_Int<QuestDataManager>(TEXT("OfferCurrencyType"), dataRow, rowIdx);
	obj->OfferCurrencyAmount = TReadDataValue_Int<QuestDataManager>(TEXT("OfferCurrencyAmount"), dataRow, rowIdx);						                        
	obj->OfferBuff = TReadDataValue_Int<QuestDataManager>(TEXT("OfferBuff"), dataRow, rowIdx);								                            
	obj->OfferBuffLevel = TReadDataValue_Int<QuestDataManager>(TEXT("OfferBuffLevel"), dataRow, rowIdx);							                        
	
	for (int i = 0; i < 1; ++ i)
	{
		obj->OfferItem(i).CheckItem = NULL;
		FString text = FString::Printf(TEXT("OfferItem.CheckItemID%d"), i + 1);
		obj->OfferItem(i).CheckItemID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("OfferItem.CheckItemCnt%d"), i + 1);
		obj->OfferItem(i).CheckItemCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("OfferItem.Description%d"), i + 1);
		obj->OfferItem(i).Description = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
	}

	obj->CheckBuff = TReadDataValue_Int<QuestDataManager>(TEXT("CheckBuff"), dataRow, rowIdx);										                
	obj->CheckBuffLevel = TReadDataValue_Int<QuestDataManager>(TEXT("CheckBuffLevel"), dataRow, rowIdx);	

	for (int i = 0; i < obj->CheckKillNpc.Num(); ++ i)
	{
		obj->CheckKillNpc(i).CheckKillNpcNew = NULL;

		FString text = FString::Printf(TEXT("CheckKillNpc.CheckKillNpcNew%d"), i + 1);
		obj->CheckKillNpc(i).CheckKillNpcNewId = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillNpc.CheckKillNpcGroupId%d"), i + 1);
		obj->CheckKillNpc(i).CheckKillNpcGroupId = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillNpc.CheckKillNpcCnt%d"), i + 1);
		obj->CheckKillNpc(i).CheckKillNpcCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillNpc.Description%d"), i + 1);
		obj->CheckKillNpc(i).Description = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillNpc.CheckKillNpcMap%d"), i + 1);
		obj->CheckKillNpc(i).CheckKillNpcMap = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillNpc.CheckKillNpcNav%d"), i + 1);
		obj->CheckKillNpc(i).CheckKillNpcNav = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillNpc.CheckKillNpcFly%d"), i + 1);
		obj->CheckKillNpc(i).CheckKillNpcFly = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
	}

	for (int i = 0; i < obj->CheckItem.Num(); ++ i)
	{
		obj->CheckItem(i).CheckItem = NULL;

		FString text = FString::Printf(TEXT("CheckItem.CheckItemID%d"), i + 1);
		obj->CheckItem(i).CheckItemID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckItem.CheckItemCnt%d"), i + 1);
		obj->CheckItem(i).CheckItemCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckItem.Description%d"), i + 1);
		obj->CheckItem(i).Description = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckItem.CheckItemMap%d"), i + 1);
		obj->CheckItem(i).CheckItemMap = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckItem.CheckItemNav%d"), i + 1);
		obj->CheckItem(i).CheckItemNav = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckItem.CheckItemFly%d"), i + 1);
		obj->CheckItem(i).CheckItemFly = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckItem.ItemDropSource%d"), i + 1);
		obj->CheckItem(i).ItemDropSource = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
	}

	obj->TalkNpcID = TReadDataValue_Int<QuestDataManager>(TEXT("TalkNpcID"), dataRow, rowIdx);										                
	obj->TalkNpcMap = TReadDataValue_Int<QuestDataManager>(TEXT("TalkNpcMap"), dataRow, rowIdx);										                
	obj->TalkNpcFly = TReadDataValue_Int<QuestDataManager>(TEXT("TalkNpcFly"), dataRow, rowIdx);				                                        
	obj->TalkVarID = TReadDataValue_Int<QuestDataManager>(TEXT("TalkVarID"), dataRow, rowIdx);										                
	obj->ExplorVarID = TReadDataValue_Int<QuestDataManager>(TEXT("ExplorVarID"), dataRow, rowIdx);										                
	obj->CheckLevel = TReadDataValue_Int<QuestDataManager>(TEXT("CheckLevel"), dataRow, rowIdx);

	for (int i = 0; i < obj->CheckVar.Num(); ++ i)
	{
		FQuestCheckVar checkVar;
		FString text = FString::Printf(TEXT("CheckVar.CheckVarType%d"), i + 1);
		obj->CheckVar(i).CheckVarType = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckVar.CheckVarID%d"), i + 1);
		obj->CheckVar(i).CheckVarID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckVar.CheckVarCnt%d"), i + 1);
		obj->CheckVar(i).CheckVarCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckVar.Description%d"), i + 1);
		obj->CheckVar(i).Description = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
	}

	for (int i = 0; i < obj->CheckClassVar.Num(); ++ i)
	{
		FQuestCheckClassVar checkClassVar;
		FString text = FString::Printf(TEXT("CheckClassVar.CheckClassVarCnt%d"), i + 1);
		obj->CheckClassVar(i).CheckClassVarCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckClassVar.CheckClassVarDesc%d"), i + 1);
		obj->CheckClassVar(i).CheckClassVarDesc = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
	}

	obj->CheckReputationForceID = TReadDataValue_Int<QuestDataManager>(TEXT("CheckReputationForceID"), dataRow, rowIdx);		                                    
	obj->CheckReputation = TReadDataValue_Int<QuestDataManager>(TEXT("CheckReputation"), dataRow, rowIdx);

	for (int i = 0; i < obj->CheckKillDoodad.Num(); ++ i)
	{
		FString text = FString::Printf(TEXT("CheckKillDoodad.CheckKillDoodadID%d"), i + 1);
		obj->CheckKillDoodad(i).CheckKillDoodadID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillDoodad.CheckKillDoodadCnt%d"), i + 1);
		obj->CheckKillDoodad(i).CheckKillDoodadCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillDoodad.CheckKillDoodadDesc%d"), i + 1);
		obj->CheckKillDoodad(i).CheckKillDoodadDesc = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillDoodad.CheckKillDoodadMap%d"), i + 1);
		obj->CheckKillDoodad(i).CheckKillDoodadMap = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillDoodad.CheckKillDoodadNav%d"), i + 1);
		obj->CheckKillDoodad(i).CheckKillDoodadNav = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillDoodad.CheckKillDoodadFly%d"), i + 1);
		obj->CheckKillDoodad(i).CheckKillDoodadFly = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
	}

	for (int i = 0; i < obj->CheckKillPlayer.Num(); ++ i)
	{
		FString text = FString::Printf(TEXT("CheckKillPlayer.CheckKillPlayerLevelStart%d"), i + 1);
		obj->CheckKillPlayer(i).CheckKillPlayerLevelStart = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillPlayer.CheckKillPlayerLevelEnd%d"), i + 1);
		obj->CheckKillPlayer(i).CheckKillPlayerLevelEnd = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillPlayer.CheckKillPlayerBuffId%d"), i + 1);
		obj->CheckKillPlayer(i).CheckKillPlayerBuffId = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillPlayer.CheckKillPlayerCnt%d"), i + 1);
		obj->CheckKillPlayer(i).CheckKillPlayerCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("CheckKillPlayer.CheckKillPlayerDesc%d"), i + 1);
		obj->CheckKillPlayer(i).CheckKillPlayerDesc = TReadDataValue_FString<QuestDataManager>(*text, dataRow, rowIdx);
	}
									    
	obj->AwardCurrencyType = TReadDataValue_Int<QuestDataManager>(TEXT("AwardCurrencyType"), dataRow, rowIdx);                                                             
	obj->AwardCurrencyAmount = TReadDataValue_Int<QuestDataManager>(TEXT("AwardCurrencyAmount"), dataRow, rowIdx); 
										
	obj->AwardCurrencyTypeEx = TReadDataValue_Int<QuestDataManager>(TEXT("AwardCurrencyTypeEx"), dataRow, rowIdx);                                                                  
	obj->AwardCurrencyAmountEx = TReadDataValue_Int<QuestDataManager>(TEXT("AwardCurrencyAmountEx"), dataRow, rowIdx); 	

	obj->AwardExp = TReadDataValue_Int<QuestDataManager>(TEXT("AwardExp"), dataRow, rowIdx);														    
	obj->AwardBuff = TReadDataValue_Int<QuestDataManager>(TEXT("AwardBuff"), dataRow, rowIdx);														    
	obj->AwardBuffLevel = TReadDataValue_Int<QuestDataManager>(TEXT("AwardBuffLevel"), dataRow, rowIdx);													    
	obj->AwardReputationType = TReadDataValue_Int<QuestDataManager>(TEXT("AwardReputationType"), dataRow, rowIdx);												    
	obj->AwardReputation = TReadDataValue_Int<QuestDataManager>(TEXT("AwardReputation"), dataRow, rowIdx);													    
	obj->AwardTitle = TReadDataValue_Int<QuestDataManager>(TEXT("AwardTitle"), dataRow, rowIdx);	    												    
	obj->AwardActivity = TReadDataValue_Int<QuestDataManager>(TEXT("AwardActivity"), dataRow, rowIdx);  

	for (int i = 0; i < obj->AwardItem.Num(); ++ i)
	{
		obj->AwardItem(i).AwardItem = NULL;

		FString text = FString::Printf(TEXT("AwardItem.AwardItemID%d"), i + 1);
		obj->AwardItem(i).AwardItemID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("AwardItem.AwardItemCnt%d"), i + 1);
		obj->AwardItem(i).AwardItemCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
	}

	for (int i = 0; i < obj->SelectableAwardItem.Num(); ++ i)
	{
		obj->SelectableAwardItem(i).SelectableAwardItem = NULL;

		FString text = FString::Printf(TEXT("SelectableAwardItem.SelectableAwardItemID%d"), i + 1);
		obj->SelectableAwardItem(i).SelectableAwardItemID = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("SelectableAwardItem.SelectableAwardItemCnt%d"), i + 1);
		obj->SelectableAwardItem(i).SelectableAwardItemCnt = TReadDataValue_Int<QuestDataManager>(*text, dataRow, rowIdx);
	}
}

void QuestDataProcessor::PostInitData()
{
	QuestDataManager& questDataManager = QuestDataManager::getSingleton();
	QuestDataManager::DataTypeMap::const_iterator iter = questDataManager._dataMap.begin();
	for (; iter != questDataManager._dataMap.end(); ++ iter)
	{
		MMOQuestData::getSingleton().AddQuestData(iter->second);
	}
}

//////////////////////////////////////////////////////////////////////////
QuestRecycleData* QuestRecycleDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void QuestRecycleDataProcessor::DestroyDataObject(QuestRecycleData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void QuestRecycleDataProcessor::ParseData(QuestRecycleData* obj, class CTabFile* dataRow, int rowIdx)
{
	QuestRecycleData data;
	data.ItemLevel = TReadDataValue_Int<QuestRecycleDataManager>(TEXT("ItemLevel"), dataRow, rowIdx);
	data.ItemColor = MMOFacade::getSingleton().FindClientEnumValue(TReadDataValue_FString<QuestRecycleDataManager>(TEXT("ItemColor"), dataRow, rowIdx),
		TEXT("EITEM_COLOR"),1);
	data.ItemSubType = TReadDataValue_FString<QuestRecycleDataManager>(TEXT("ItemSubType"), dataRow, rowIdx);
	data.VarIndex = TReadDataValue_Int<QuestRecycleDataManager>(TEXT("VarIndex"), dataRow, rowIdx);
	MMOQuestData::getSingleton().AddQuestRecycle(data);
}

void QuestRecycleDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
CircleQuestData* CircleQuestDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void CircleQuestDataProcessor::DestroyDataObject(CircleQuestData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void CircleQuestDataProcessor::ParseData(CircleQuestData* obj, class CTabFile* dataRow, int rowIdx)
{
	CircleQuestData data;
	data.CircleId = TReadDataValue_Int<CircleQuestDataManager>(TEXT("CircleID"), dataRow, rowIdx);
	data.CircleGroup = TReadDataValue_Int<CircleQuestDataManager>(TEXT("GroupID"), dataRow, rowIdx);
	data.CircleRoundMax = TReadDataValue_Int<CircleQuestDataManager>(TEXT("MaxCirclePerDay"), dataRow, rowIdx);
	data.CircleSize = TReadDataValue_Int<CircleQuestDataManager>(TEXT("MaxQuestPerNum"), dataRow, rowIdx);
	data.ResetTime = TReadDataValue_Int<CircleQuestDataManager>(TEXT("ResetTime"), dataRow, rowIdx);
	data.Level = TReadDataValue_Int<CircleQuestDataManager>(TEXT("Level"), dataRow, rowIdx);
	data.CircleRound = TReadDataValue_Int<CircleQuestDataManager>(TEXT("CircleNum"), dataRow, rowIdx);

	for (UINT8 j = 0; j < MAX_QUEST_PER_CIRCLE; ++ j)
	{
		FString text = FString::Printf(TEXT("RewardGroupID%d"), j + 1);
		data.RewardId[j] = TReadDataValue_Int<CircleQuestDataManager>(*text, dataRow, rowIdx);
	}

	MMOQuestData::getSingleton().AddCircleQuestData(data);
}

void CircleQuestDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
CircleQuestRewardData* CircleQuestRewardDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void CircleQuestRewardDataProcessor::DestroyDataObject(CircleQuestRewardData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void CircleQuestRewardDataProcessor::ParseData(CircleQuestRewardData* obj, class CTabFile* dataRow, int rowIdx)
{
	CircleQuestRewardData data;

	data.RewardId = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("GroupID"), dataRow, rowIdx);
	data.MoneyType = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardMoneyType"), dataRow, rowIdx);
	data.Money = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardMoneyAmount"), dataRow, rowIdx);
	data.Exp = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardExp"), dataRow, rowIdx);
	data.BuffId = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardBuffID"), dataRow, rowIdx);
	data.BuffLevel = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardBuffLevel"), dataRow, rowIdx);
	data.Soul = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardPtsCount"), dataRow, rowIdx);
	data.Title = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardTitleID"), dataRow, rowIdx);
	data.ReputationForceId = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardReputationForceID"), dataRow, rowIdx);
	data.Reputation = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardReputation"), dataRow, rowIdx);

	for (INT j = 0; j < MAX_CIRCLE_AWARD_ITEM_COUNT; ++ j)
	{
		FString text = FString::Printf(TEXT("AwardItemID%d"), j + 1);
		data.AwardItemId[j] = TReadDataValue_Int<CircleQuestRewardDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("AwardItemCnt%d"), j + 1);
		data.AwardItemCount[j] = TReadDataValue_Int<CircleQuestRewardDataManager>(*text, dataRow, rowIdx);
	}

	data.NeedSelectAwardItem = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("NeedSelectAward"), dataRow, rowIdx);

	for (INT j = 0; j < MAX_SELECTABLE_REWARD_COUNT; ++ j)
	{
		FString text = FString::Printf(TEXT("SelectableAwardItemID%d"), j + 1);
		data.SelectableAwardItemId[j] = TReadDataValue_Int<CircleQuestRewardDataManager>(*text, dataRow, rowIdx);
		text = FString::Printf(TEXT("SelectableAwardItemCnt%d"), j + 1);
		data.SelectableAwardItemCount[j] = TReadDataValue_Int<CircleQuestRewardDataManager>(*text, dataRow, rowIdx);
	}

	data.GuildContribute = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardGuildContribute"), dataRow, rowIdx);
	data.MoneyTypeEx = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardMoneyTypeEx"), dataRow, rowIdx);
	data.MoneyEx = TReadDataValue_Int<CircleQuestRewardDataManager>(TEXT("AwardMoneyAmountEx"), dataRow, rowIdx);

	MMOQuestData::getSingleton().AddCircleQuestRewardData(data);
}

void CircleQuestRewardDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
StoryQuestData* StoryQuestDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void StoryQuestDataProcessor::DestroyDataObject(StoryQuestData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void StoryQuestDataProcessor::ParseData(StoryQuestData* obj, class CTabFile* dataRow, int rowIdx)
{
	StoryQuestData data;

	data.Chapter = TReadDataValue_Int<StoryQuestDataManager>(TEXT("Chapter"), dataRow, rowIdx);
	data.ChapterName =TReadDataValue_FString<StoryQuestDataManager>(TEXT("ChapterName"), dataRow, rowIdx);
	data.ChapterDesc = TReadDataValue_FString<StoryQuestDataManager>(TEXT("ChapterDesc"), dataRow, rowIdx);
	data.ChapterIcon = TReadDataValue_FString<StoryQuestDataManager>(TEXT("ChapterIcon"), dataRow, rowIdx);

	data.Stage = TReadDataValue_Int<StoryQuestDataManager>(TEXT("Stage"), dataRow, rowIdx);
	data.StageName = TReadDataValue_FString<StoryQuestDataManager>(TEXT("StageName"), dataRow, rowIdx);
	data.StageDesc = TReadDataValue_FString<StoryQuestDataManager>(TEXT("StageDesc"), dataRow, rowIdx);
	
	MMOQuestData::getSingleton().AddStoryQuestData(data);
}

void StoryQuestDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
RewardQuestCostData* RewardQuestCostDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void RewardQuestCostDataProcessor::DestroyDataObject(RewardQuestCostData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void RewardQuestCostDataProcessor::ParseData(RewardQuestCostData* obj, class CTabFile* dataRow, int rowIdx)
{
	RewardQuestCostData data;
	data.Time = TReadDataValue_Int<RewardQuestCostDataManager>(TEXT("Time"), dataRow, rowIdx);
	data.Cost = TReadDataValue_Int<RewardQuestCostDataManager>(TEXT("MoneyCost"), dataRow, rowIdx);

	MMOQuestData::getSingleton().AddRewardQuestCostData(data);
}

void RewardQuestCostDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
QuestDoodadData* QuestDoodadDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void QuestDoodadDataProcessor::DestroyDataObject(QuestDoodadData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void QuestDoodadDataProcessor::ParseData(QuestDoodadData* obj, class CTabFile* dataRow, int rowIdx)
{
	QuestDoodadData data;
	data.QuestId = TReadDataValue_Int<QuestDoodadDataManager>(TEXT("QuestId"), dataRow, rowIdx);

	for (int i = 0; i < MAX_QUEST_DOODAD_COUNT; ++ i)
	{
		FString headerName = FString::Printf(TEXT("Doodad%d"), i);
		data.DoodadDrops.insert(TReadDataValue_Int<QuestDoodadDataManager>(headerName, dataRow, rowIdx));
	}

	MMOQuestData::getSingleton().AddQuestDoodadData(data);
}

void QuestDoodadDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
MMOQuestData::MMOQuestData()
{
	//initialize quest data
	appMemset(_NormalQuest, 0, sizeof(UINT8) * MAX_NORMAL_QUEST_COUNT / 8);
	appMemset(_RepeatQuest, 0, sizeof(UINT8) * MAX_REPEAT_QUEST_COUNT);
	_RepeatQuestScanTime = 0;
	_RewardQuestFreeCount = MAX_REWARD_QUEST_COUNT_PER_ROLE;
	_RewardQuestCostCount = 0;
	_EmergencyQuestCompleteCount = 0;
}

MMOQuestData::~MMOQuestData()
{
}

void MMOQuestData::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOQuestData::Initialize"), 0);

	_LoadQuestData(TEXT("quest.txt"));
	_LoadCircleQuestData(TEXT("circle_quest.txt"));
	_LoadCircleQuestRewardData(TEXT("quest_reward.txt"));
	_LoadStoryQuestData(TEXT("quest_story.txt"));
	_LoadQuestRecyle(TEXT("quest_recycle.txt"));
	_LoadRewardQuestCostData(TEXT("reward_quest_money_cost.txt"));

	// if not a release build, check quest requirement item source errors, generate quest_doodad.txt
#if !SHIPPING_PC_GAME
	_CheckQuestData();
	GenerateQuestDoodadDropData(TEXT("quest_doodad.txt"));
#endif

	_LoadQuestDoodadData(TEXT("quest_doodad.txt"));
}

void MMOQuestData::Shutdown()
{
	_UnloadCircleQuestData();
	_UnloadCircleQuestRewardData();
	_UnloadStoryQuestData();
	_UnloadQuestRecyle();
	_UnloadRewardQuestCostData();
	_UnloadQuestDoodadData();
}

void MMOQuestData::Clear()
{
	_AcceptedQuestDataMap.clear();

	_AcceptedQuestTraceList.clear();
	_AvailableQuestTraceList.clear();

	appMemzero(_NormalQuest, sizeof(UINT8) * (MAX_NORMAL_QUEST_COUNT / 8));
	appMemzero(_RepeatQuest, sizeof(UINT8) * MAX_REPEAT_QUEST_COUNT);
	_RepeatQuestScanTime = 0;

	_CircleQuestInfoMap.clear();

	_RewardQuestList.clear();
	_RewardQuestFreeCount = MAX_REWARD_QUEST_COUNT_PER_ROLE;
	_RewardQuestCostCount = 0;

	//_StoryQuestChapterStageMap.clear();
	_StoryQuestPastChapterList.clear();

	_EmergencyQuestCoolDownMap.clear();

	_MonsterAcceptedNormalMap.clear();
	_MonsterAcceptedEmergencyMap.clear();

	_EmergencyQuestCompleteCount = 0;

	_ReAcceptQuestList.clear();
}

void MMOQuestData::_InitQuestUtilData()
{
	_QuestMapDataMap.clear();
	_QuestNpcInfoMap.clear();
	_LevelTriggerQuestList.clear();
	for (auto iter = _QuestDataMap.begin(); iter != _QuestDataMap.end(); ++iter)
	{
		UQuest* quest = iter->second;
		if (!quest)
		{
			continue;
		}

		if (quest->StartType == EAQT_Npc)
		{
			QuestNpcInfo* info = _GetOrAddQuestNpcInfo(quest->StartNPCNewId);
			if (info)
			{
				info->StartQuestList.insert(std::make_pair(quest->QuestTempID, quest));
			}

			if (quest->StartNPCMap > 0)
			{
				QuestMapDataMap::iterator iterMap = _QuestMapDataMap.find(quest->StartNPCMap);
				if (iterMap == _QuestMapDataMap.end())
				{
					QuestDataList questList;
					questList.clear();
					questList.push_back(quest);
					_QuestMapDataMap.insert(std::make_pair(quest->StartNPCMap, questList));
				}
				else
				{
					QuestDataList& questList = iterMap->second;
					questList.push_back(quest);
				}
			}
		}
		else if (quest->StartType == EAQT_Level)
		{
			_LevelTriggerQuestList.push_back(quest);

			if (quest->StartNPCMap > 0)
			{
				QuestMapDataMap::iterator iterMap = _QuestMapDataMap.find(quest->StartNPCMap);
				if (iterMap == _QuestMapDataMap.end())
				{
					QuestDataList questList;
					questList.clear();
					questList.push_back(quest);
					_QuestMapDataMap.insert(std::make_pair(quest->StartNPCMap, questList));
				}
				else
				{
					QuestDataList& questList = iterMap->second;
					questList.push_back(quest);
				}
			}
		}

		if (quest->FinishType == ESQT_Npc)
		{
			QuestNpcInfo* info = _GetOrAddQuestNpcInfo(quest->FinishNpcNewId);
			if (info)
			{
				info->EndQuestList.insert(std::make_pair(quest->QuestTempID, quest));
			}
		}
	}
}

void MMOQuestData::LoadQuestDataFromUpk()
{
	TArray<FString> DirArray;
	GConfig->GetArray( TEXT("QuestTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );

	MMO::FormatTempleteDataLoadingDir(DirArray);

	for (INT k = 0; k < DirArray.Num(); ++k)
	{
		const FString& cfgDir = DirArray(k);

		debugIDf(TEXT("TemplateData"), TEXT("===== Loading Quest template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++ i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if ( !Package )
			{
				M_AddDataError(TEXT("quest.txt"),
					FString::Printf(TEXT("Cannot load quest data package: %s"), *pkgs(i)));
				continue;;
			}

			if (!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}

			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UQuest* quest = Cast<UQuest>(Package->GetNetObjectAtIndex(j));
				if (quest)
				{
					quest->AddToRoot();
					INT questId = quest->QuestTempID;

					QuestDataMap::const_iterator iter = _QuestDataMap.find(questId);
					if (iter == _QuestDataMap.end())
					{
						_QuestDataMap.insert(std::make_pair(questId, quest));
					}
				}
			}
		}
	}
}

void MMOQuestData::AddQuestData(UQuest* data)
{
	if (!data)
	{
		return;
	}

	INT questId = data->QuestTempID;
	QuestDataMap::const_iterator iter = _QuestDataMap.find(questId);
	if (iter == _QuestDataMap.end())
	{
		_QuestDataMap.insert(std::make_pair(questId, data));
	}
}

UBOOL MMOQuestData::_LoadQuestData(const TCHAR* filePath)
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOQuestData::Init"), 0);

	_QuestDataMap.clear();
	QuestDataManager::getSingleton().InitFromFile(filePath, true);

	_InitQuestUtilData();
	return TRUE;
}

void MMOQuestData::_UnloadQuestData()
{
	_QuestMapDataMap.clear();
	_QuestNpcInfoMap.clear();
	_LevelTriggerQuestList.clear();
	_QuestDataMap.clear();
}

void MMOQuestData::AddCircleQuestData(const CircleQuestData& data)
{
	_CircleQuestDataMap.insert(std::make_pair(MAKE_CIRCLE_QUEST_KEY(data.CircleId, 0, data.Level), data));
}

UBOOL MMOQuestData::_LoadCircleQuestData(const TCHAR* filePath)
{
	_CircleQuestDataMap.clear();
	CircleQuestDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOQuestData::_UnloadCircleQuestData()
{
	_CircleQuestDataMap.clear();
}

void MMOQuestData::AddCircleQuestRewardData(const CircleQuestRewardData& data)
{
	_CircleQuestRewardDataMap.insert(std::make_pair(data.RewardId, data));
}

UBOOL MMOQuestData::_LoadCircleQuestRewardData(const TCHAR* filePath)
{
	_CircleQuestRewardDataMap.clear();
	CircleQuestRewardDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOQuestData::_UnloadCircleQuestRewardData()
{
	_CircleQuestRewardDataMap.clear();
}

void MMOQuestData::AddStoryQuestData(const StoryQuestData& data)
{
	_StoryQuestDataMap.insert(std::make_pair(MAKE_STORY_QUEST_KEY(data.Chapter, data.Stage), data));
	_AddStoryQuestChapterStage(data.Chapter, data.Stage);
}

UBOOL MMOQuestData::_LoadStoryQuestData(const TCHAR* filePath)
{
	_StoryQuestDataMap.clear();
	_StoryQuestChapterStageMap.clear();
	StoryQuestDataManager::getSingleton().InitFromFile(filePath, true);
	
	// Requirement: #15142, temporarily revert to showing the quest name
	/*
	// unified handling of quest names
	for (auto iter = _QuestDataMap.begin(); iter != _QuestDataMap.end(); ++ iter)
	{
		UQuest* quest = iter->second;
		if (quest)
		{
			if (quest->QuestType == E_qt_Trunk)
			{
				StoryQuestData data;
				if (GetStoryQuestData(quest->QuestChapter, quest->QuestStage, data))
				{
					quest->QuestName = data.StageName;
				}
			}
		}
	}
	*/
	return TRUE;
}

void MMOQuestData::_UnloadStoryQuestData()
{
	_StoryQuestDataMap.clear();
}

UBOOL MMOQuestData::_CheckQuestData()
{
	UBOOL hasError = FALSE;
	QuestDataMap::iterator iterQuest = _QuestDataMap.begin();
	for (; iterQuest != _QuestDataMap.end(); ++ iterQuest)
	{
		const UQuest* quest = iterQuest->second;
		for (INT i = 0; i < quest->CheckItem.Num(); ++ i)
		{
			const FQuestCheckItem& checkItem = quest->CheckItem(i);
			if (checkItem.CheckItemID != 0)
			{
				if (checkItem.ItemDropSource == EIDS_None)
				{
					// only check whether it is in the item table
					if (!MMOFacade::getSingleton().FindItemTemplate(checkItem.CheckItemID))
					{
						hasError = TRUE;
						M_AddDataError(TEXT("quest.txt"), FString::Printf(TEXT("Quest [%d][%s] item condition [%d]: cannot find item [%d] in item.txt/item_armor.txt"),
							quest->GetTemplateID(), *quest->QuestName, i, checkItem.CheckItemID));
					}
				}
			}
		}
	}

	return !hasError;
}

#if !SHIPPING_PC_GAME
void MMOQuestData::GenerateQuestDoodadDropData(const TCHAR* filePath)
{
	typedef std::map<INT, DoodadDropSet> ItemDoodadDropMap;
	ItemDoodadDropMap itemDoodadDropMap;

	const MMODoodadDropDataManager::DataTypeMap& doodadDropMap = MMODoodadDropDataManager::getSingleton().GetDataMap();
	MMODoodadDropDataManager::DataTypeMap::const_iterator iterDoodadDrop = doodadDropMap.begin();
	for (; iterDoodadDrop != doodadDropMap.end(); ++iterDoodadDrop)
	{
		const FDoodadDropData* doodadDropData = iterDoodadDrop->second;
		if (doodadDropData)
		{
			for (INT i = 0; i < doodadDropData->GetDropSetCount(); ++ i)
			{
				const FDoodadDropSet& dropSet = doodadDropData->GetDropSet(i);
				if (dropSet.DropGroupID > 0)
				{
					const FDropGroup* dropGroup = dropSet.GetDropGroup();
					if (dropGroup)
					{
						for (INT j = 0; j < dropGroup->DropItemList.Num(); ++ j)
						{
							INT itemId = dropGroup->DropItemList(j).ItemTemplateID;
							if (itemId > 0)
							{
								ItemDoodadDropMap::iterator iterItem = itemDoodadDropMap.find(itemId);
								if (iterItem == itemDoodadDropMap.end())
								{
									iterItem = itemDoodadDropMap.insert(std::make_pair(itemId, DoodadDropSet())).first;
								}
								// what is recorded here is CombineId
								iterItem->second.insert(doodadDropData->GetTemplateID());
							}
						}
					}
				}
			}
		}
	}

	typedef std::map<INT, DoodadDropSet> QuestDoodadMap;
	QuestDoodadMap questDoodadMap;
	questDoodadMap.clear();

	QuestDataMap questDataMap = MMOQuestData::getSingleton().GetQuestDataMap();
	QuestDataMap::iterator iterQuest = questDataMap.begin();
	for (; iterQuest != questDataMap.end(); ++ iterQuest)
	{
		const UQuest* quest = iterQuest->second;
		if (!quest)
		{
			continue;
		}

		DoodadDropSet doodadDrops;
		doodadDrops.clear();

		for (int i = 0; i < quest->CheckItem.Num(); ++ i)
		{
			const FQuestCheckItem& checkItem = quest->CheckItem(i);
			if (checkItem.CheckItemID != 0 && checkItem.ItemDropSource == EIDS_Doodad)
			{
				ItemDoodadDropMap::iterator iterItemDoodad = itemDoodadDropMap.find(checkItem.CheckItemID);
				if (iterItemDoodad != itemDoodadDropMap.end())
				{
					doodadDrops.insert(iterItemDoodad->second.begin(), iterItemDoodad->second.end());
				}
				else
				{
					const UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(checkItem.CheckItemID);
					if (itemTemp)
					{
						M_AddDataError(TEXT("quest.txt"), 
							FString::Printf(TEXT("Quest [%d][%s] item condition [%d] [expected from Doodad drops]: cannot find item [%d][%s] in the DropGroup configured in doodad_drop_data.txt"),
							quest->GetTemplateID(), *quest->QuestName, i, checkItem.CheckItemID, *itemTemp->ItemName));
					}
					else
					{
						M_AddDataError(TEXT("quest.txt"), 
							FString::Printf(TEXT("Quest [%d][%s] item condition [%d] [expected from Doodad drops]: cannot find item [%d][item not in item table] in the DropGroup configured in doodad_drop_data.txt"),
							quest->GetTemplateID(), *quest->QuestName, i, checkItem.CheckItemID));
					}

					// if not found in the DropGroups already configured in the doodad_drop_data table, look in all DropGroups (DropGroup is not necessarily all configured in doodad_drop_data)
					bool foundInDropGroup = MMODropGroupDataManager_IsItemExists(checkItem.CheckItemID);
					if (!foundInDropGroup)
					{
						const UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(checkItem.CheckItemID);
						if (itemTemp)
						{
							M_AddDataError(TEXT("quest.txt"),
								FString::Printf(TEXT("Quest [%d][%s] item condition [%d]: cannot find item [%d][%s] in drop_group.txt"),
								quest->GetTemplateID(), *quest->QuestName, i, checkItem.CheckItemID, *itemTemp->ItemName));
						}
						else
						{
							M_AddDataError(TEXT("quest.txt"),
								FString::Printf(TEXT("Quest [%d][%s] item condition [%d]: cannot find item [%d][item not in item table] in drop_group.txt"),
								quest->GetTemplateID(), *quest->QuestName, i, checkItem.CheckItemID));
						}
					}
				}
			}
		}

		if (!doodadDrops.empty())
		{
			questDoodadMap.insert(std::make_pair(quest->GetTemplateID(), doodadDrops));
		}
	}

	// output
	FString printText;
	// table header
	printText += TEXT("QuestId");
	for (int i = 0; i < MAX_QUEST_DOODAD_COUNT; ++ i)
	{
		printText += TEXT("\t");
		printText += FString::Printf(TEXT("Doodad%d"), i);
	}
	printText += TEXT("\r\n");
	// second row
	printText += TEXT("Client");
	for (int i = 0; i < MAX_QUEST_DOODAD_COUNT; ++ i)
	{
		printText += TEXT("\tClient");
	}
	printText += TEXT("\r\n");
	// third row
	printText += TEXT("0");
	for (int i = 0; i < MAX_QUEST_DOODAD_COUNT; ++ i)
	{
		printText += TEXT("\t0");
	}
	printText += TEXT("\r\n");
	// data
	QuestDoodadMap::iterator iterQuestDoodad = questDoodadMap.begin();
	for (; iterQuestDoodad != questDoodadMap.end() ; iterQuestDoodad ++)
	{
		printText += FString::Printf(TEXT("%d"), iterQuestDoodad->first);

		DoodadDropSet::iterator iterDoodadDrop = iterQuestDoodad->second.begin();
		int index = 0;
		for (; iterDoodadDrop != iterQuestDoodad->second.end(); ++ iterDoodadDrop)
		{
			printText += TEXT("\t");
			printText += FString::Printf(TEXT("%d"), *iterDoodadDrop);
			index ++;
		}

		// pad the rest with 0
		for (int i = 0; i < MAX_QUEST_DOODAD_COUNT - index; ++ i)
		{
			printText += TEXT("\t0");
		}

		printText += TEXT("\r\n");
	}

	FString writeFileName = appGameConfigDir();
	writeFileName += FString(filePath);
	FArchive* ar = GFileManager->CreateFileWriter(*writeFileName);
	
	if (ar)
	{
		UNICHAR BOM = UNICODE_BOM;
		ar->Serialize(&BOM, sizeof(BOM));
		ar->Serialize(const_cast<TCHAR*>(*printText), printText.Len() * sizeof(TCHAR));
		ar->Flush();
		ar->Close();
		delete ar;
	}
}
#endif

void MMOQuestData::AddQuestDoodadData(const QuestDoodadData& data)
{
	_QuestDoodadDataMap.insert(std::make_pair(data.QuestId, data));
}

UBOOL MMOQuestData::IsAcceptedQuestDoodad(INT doodadDropId)
{
	if (doodadDropId <= 0)
	{
		return FALSE;
	}

	AcceptedQuestDataMap::const_iterator iterQuest = _AcceptedQuestDataMap.begin();
	for (; iterQuest != _AcceptedQuestDataMap.end(); ++ iterQuest)
	{
		const AcceptedQuestData& acceptedQuest = iterQuest->second;
		QuestDoodadDataMap::iterator iterQuestDoodad = _QuestDoodadDataMap.find(acceptedQuest.Id);
		if (iterQuestDoodad != _QuestDoodadDataMap.end())
		{
			QuestDoodadData questDoodad = iterQuestDoodad->second;
			DoodadDropSet::iterator iterDoodadDrop = questDoodad.DoodadDrops.find(doodadDropId);
			if (iterDoodadDrop != questDoodad.DoodadDrops.end())
			{
				const UQuest* quest = GetQuestData(acceptedQuest.Id);
				if (quest)
				{
					for (INT j = 0; j < quest->CheckItem.Num(); ++ j)
					{
						INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(quest->CheckItem(j).CheckItemID, E_CONSLOT_DEFAULT);
						if (itemCount < quest->CheckItem(j).CheckItemCnt)
						{
							return TRUE;
						}
					}
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOQuestData::_LoadQuestDoodadData(const TCHAR* filePath)
{
	_QuestDoodadDataMap.clear();
	QuestDoodadDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOQuestData::_UnloadQuestDoodadData()
{
	_QuestDoodadDataMap.clear();
}

const QuestDataMap& MMOQuestData::GetQuestDataMap() const
{
	return _QuestDataMap;
}

const class UQuest* MMOQuestData::GetQuestData(INT questId) const
{
	QuestDataMap::const_iterator iter = _QuestDataMap.find(questId);
	if (iter != _QuestDataMap.end())
	{
		return iter->second;
	}

	if (questId != INVALID_QUEST_ID)
	{
		warnf(TEXT("[MMOQuestData::FindQuestTemplate const] Quest template not found:%d"), questId);
	}

	return 0;
}

void MMOQuestData::UpdateQuestList(ROLE_DB_QUEST_DATA* questData)
{
	if (!questData)
	{
		return;
	}

	appMemcpy(_NormalQuest, questData->byNormalQuest, sizeof(UINT8) * MAX_NORMAL_QUEST_COUNT / 8);
	appMemcpy(_RepeatQuest, questData->byRepeatQuest, sizeof(UINT8) * MAX_REPEAT_QUEST_COUNT);
	_RepeatQuestScanTime = questData->dwRepeatQuestScanTime;

	for (UINT8 i = 0; i < questData->byAcceptedQuestCount; ++ i)
	{
		INT32 questId = questData->AcceptQuestList[i].nQuestID;
		const UQuest* quest = GetQuestData(questId);
		if (quest && IsQuestAccepted(questId) == FALSE)
		{
			_AddAcceptedQuest(questData->AcceptQuestList[i], false);
		}
	}

	// bounty quest
	_RewardQuestFreeCount = questData->stDailyQuestData.nRewardQuestRemainCompleteTime;
	_RewardQuestCostCount = questData->stDailyQuestData.dwRewardBoughtTime;

	__time32_t endTime = questData->stDailyQuestData.dwRewardQuestBrushCDTime;
	_localtime32(&endTime);

	if (endTime > 0)
	{
		__time32_t curTime;
		_time32(&curTime);
		_localtime32(&curTime);

		MMOQuestManager::getSingleton().SetRewardQuestRefreshTime(endTime - curTime);
	}

	// circle quest
	_CircleQuestInfoMap.clear();
	for (UINT8 i = 0; i < MAX_CIRCLE_COUNT; ++ i)
	{
		CircleQuestInfo info;
		info.CircleId = i + 1;
		info.QuestId = questData->stCircleQuestData.nQuestID[i];
		info.CircleIndex = questData->stCircleQuestData.nCircleNumber[i];
		info.CircleRound = questData->stCircleQuestData.nCompleteCount[i];

		_CircleQuestInfoMap.insert(std::make_pair(info.CircleId, info));
	}
	
	// completed story quests
	//_StoryQuestChapterStageMap.clear();
	_StoryQuestPastChapterList.clear();
	for (auto iter = _QuestDataMap.begin(); iter != _QuestDataMap.end(); ++ iter)
	{
		const UINT32 questId = iter->first;
		UQuest* quest = iter->second;
		if (quest && (IsQuestCompleted(questId) || IsQuestAccepted(questId)))
		{
			//_AddStoryQuestChapterStage(quest->QuestChapter, quest->QuestStage);
			_AddStoryQuestPastChapter(quest->QuestChapter);
		}
	}

	// emergency quest
	_EmergencyQuestCompleteCount = questData->byCompleteEmergencyQuestCount;

	_UpdateAcceptedQuestVariable();

	// Quest #25538: for main quests that failed to be accepted for some reason (e.g. auto-accept happened while in trial mode and failed), guarantee acceptance on login
	for (auto iter = _QuestDataMap.begin(); iter != _QuestDataMap.end(); ++iter)
	{
		const UINT32 questId = iter->first;
		UQuest* quest = iter->second;
		if (quest && 
			quest->QuestType == E_qt_Trunk && 
			quest->StartNPCNewId == 0 &&
			IsQuestAvailable(questId))
		{
			_ReAcceptQuestList.push_back(questId);
		}
	}
}

void MMOQuestData::_AddMonsterAcceptedQuest(INT monsterId, INT questId)
{
	if (monsterId == 0)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	if (quest->QuestType == E_qt_Emegency)
	{
		MonsterAcceptedQuestsMap::iterator iterMonster = _MonsterAcceptedEmergencyMap.find(monsterId);
		if (iterMonster != _MonsterAcceptedEmergencyMap.end())
		{
			QuestTraceList& questList = iterMonster->second;
			questList.push_back(questId);
		}
		else
		{
			QuestTraceList questList;
			questList.clear();
			questList.push_back(questId);
			_MonsterAcceptedEmergencyMap.insert(std::make_pair(monsterId, questList));
		}
	}
	else
	{
		MonsterAcceptedQuestsMap::iterator iterMonster = _MonsterAcceptedNormalMap.find(monsterId);
		if (iterMonster != _MonsterAcceptedNormalMap.end())
		{
			QuestTraceList& questList = iterMonster->second;
			questList.push_back(questId);
		}
		else
		{
			QuestTraceList questList;
			questList.clear();
			questList.push_back(questId);
			_MonsterAcceptedNormalMap.insert(std::make_pair(monsterId, questList));
		}
	}
}

void MMOQuestData::_RemoveMonsterAcceptedQuest(INT monsterId, INT questId)
{
	if (monsterId == 0)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	if (quest->QuestType == E_qt_Emegency)
	{
		MonsterAcceptedQuestsMap::iterator iterMonster = _MonsterAcceptedEmergencyMap.find(monsterId);
		if (iterMonster != _MonsterAcceptedEmergencyMap.end())
		{
			QuestTraceList& questList = iterMonster->second;
			QuestTraceList::iterator iterQuest = questList.begin();
			for (; iterQuest != questList.end(); ++iterQuest)
			{
				if (*iterQuest == questId)
				{
					questList.erase(iterQuest);
					break;
				}
			}

			if (questList.empty())
			{
				_MonsterAcceptedEmergencyMap.erase(iterMonster);
			}
		}
	}
	else
	{
		MonsterAcceptedQuestsMap::iterator iterMonster = _MonsterAcceptedNormalMap.find(monsterId);
		if (iterMonster != _MonsterAcceptedNormalMap.end())
		{
			QuestTraceList& questList = iterMonster->second;
			QuestTraceList::iterator iterQuest = questList.begin();
			for (; iterQuest != questList.end(); ++iterQuest)
			{
				if (*iterQuest == questId)
				{
					questList.erase(iterQuest);
					break;
				}
			}

			if (questList.empty())
			{
				_MonsterAcceptedNormalMap.erase(iterMonster);
			}
		}
	}
}

UBOOL MMOQuestData::GetNormalQuestListByMonsterBeTraced(INT monsterId, QuestTraceList& questList)
{
	MonsterAcceptedQuestsMap::iterator iterMonster = _MonsterAcceptedNormalMap.find(monsterId);
	if (iterMonster != _MonsterAcceptedNormalMap.end())
	{
		questList = iterMonster->second;
		return (questList.size() > 0);
	}

	return FALSE;
}

UBOOL MMOQuestData::GetEmergencyQuestListByMonsterBeTraced(INT monsterId, QuestTraceList& questList)
{
	MonsterAcceptedQuestsMap::iterator iterMonster = _MonsterAcceptedEmergencyMap.find(monsterId);
	if (iterMonster != _MonsterAcceptedEmergencyMap.end())
	{
		questList = iterMonster->second;
		return (questList.size() > 0);
	}

	return FALSE;
}

void MMOQuestData::_AddAcceptedQuest(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& questData, bool isNewQuest)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questData.nQuestID);
	if (iter == _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData data(questData);
		data.IsNewQuest = isNewQuest;

		const UQuest* quest = GetQuestData(questData.nQuestID);
		if (quest && quest->CheckLevel > 0)
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				UActorAttribute* attri = mainPlayer->ActorAttributes;
				if (attri)
				{
					data.Level = attri->LevelAttributes.Level;
				}
			}
		}

		// timed quest
		if (quest && quest->IsTimeLimit())
		{
			__time32_t localTime;
			_time32(&localTime);
			_localtime32(&localTime);

			if (data.AcceptTime + quest->DurationLimit > (UINT32)localTime)
			{
				data.LeftTime = data.AcceptTime + quest->DurationLimit - (UINT32)localTime;
				data.IsFailed = false;
			}
			else
			{
				// timed out
				data.LeftTime = 0;
				data.IsFailed = true;
			}
		}

		_AcceptedQuestDataMap.insert(std::make_pair(data.Id, data));

		if (quest)
		{
			for (INT i = 0; i < quest->CheckKillNpc.Num(); ++ i)
			{
				const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(i);
				if (killNpc)
				{
					if (killNpc->CheckKillNpcNewId > 0)
					{
						if (i < MAX_CONDITION_PER_QUEST && data.KillNpcCount[i] < killNpc->CheckKillNpcCnt)
						{
							_AddMonsterAcceptedQuest(killNpc->CheckKillNpcNewId, quest->GetTemplateID());
						}
					}
					else if (killNpc->CheckKillNpcGroupId > 0)
					{
						if (i < MAX_CONDITION_PER_QUEST && data.KillNpcCount[i] < killNpc->CheckKillNpcCnt)
						{
							MonsterList monsterList;
							monsterList.clear();
							if (GetGroupMonsterList(killNpc->CheckKillNpcGroupId, monsterList))
							{
								MonsterList::iterator iterMonster = monsterList.begin();
								for (; iterMonster != monsterList.end(); ++iterMonster)
								{
									_AddMonsterAcceptedQuest(*iterMonster, quest->GetTemplateID());
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("[MMOQuestData::AddPlayerQuest]Quest already exist:%d"), questData.nQuestID);
	}	
}

UBOOL MMOQuestData::IsQuestAccepted(INT32 questId) const
{
	AcceptedQuestDataMap::const_iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		return true;
	}

	return false;
}

UBOOL MMOQuestData::IsQuestCompleted(INT32 questId) const
{
	return GetCompletedQuestCount(questId) > 0;
}

UINT32 MMOQuestData::GetCompletedQuestCount(INT32 questId) const
{
	if (questId > 0 && questId < MAX_NORMAL_QUEST_COUNT)
	{
		return (_NormalQuest[questId / 8] & (1 << (questId % 8)));
	}
	else if (questId >= REPEAT_QUEST_START_ID && questId < REPEAT_QUEST_START_ID + MAX_REPEAT_QUEST_COUNT)
	{
		return _RepeatQuest[questId - REPEAT_QUEST_START_ID];
	}

	return 0;
}

UINT32 MMOQuestData::GetCompletedQuestMaxCount(INT32 questId) const
{
	const UQuest* quest = GetQuestData(questId);
	if (quest)
	{
		return quest->MaxCompleteCount;
	}

	return 0;
}

UINT32 MMOQuestData::GetQuestCompletedDate(INT32 questId, INT32& year, INT32& month, INT32& day, INT32& dayOfWeek, INT32& hour, INT32& min, INT32& seconds) const
{
	if (questId <= MAX_NORMAL_QUEST_COUNT)
		return 0;

	UINT32 time = _RepeatQuestScanTime;//[questId - MAX_NORMAL_QUEST_COUNT];
	appSecondsToLocalTime(time, year, month, dayOfWeek, day, hour, min, seconds);

	struct tm* TimeStruct;
	INT64 time64 = time;
	TimeStruct = localtime(&time64);
	if (TimeStruct)
	{
		year = TimeStruct->tm_year;
		month = TimeStruct->tm_mon;
		day = TimeStruct->tm_mday;
	}

	return time;
}

const AcceptedQuestData* MMOQuestData::GetAcceptedQuestData(INT32 questId) const
{
	AcceptedQuestDataMap::const_iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		return &iter->second;
	}

	return 0;
}

AcceptedQuestData* MMOQuestData::GetAcceptedQuestData(INT32 questId)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		return &iter->second;
	}

	return 0;
}

TArray<UQuest*> MMOQuestData::GetAcceptedQuestsByType(EQUEST_TYPE type)
{
	TArray<UQuest*> quests;
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		UQuest* quest = (UQuest*)GetQuestData(iter->second.Id);
		if (quest && (type == E_qt_MAX || quest->QuestType == type))
		{
			quests.AddItem(quest);
		}
	}
	
	return quests;
}

TArray<UQuest*> MMOQuestData::GetAvailableQuestsByType(EQUEST_TYPE type)
{
	TArray<UQuest*> quests;
	for (auto iter = _QuestDataMap.begin(); iter != _QuestDataMap.end(); ++iter)
	{
		const UINT32 questId = iter->first;
		if (iter->second->QuestType == type &&
			IsQuestAccepted(questId) == FALSE && 
			IsQuestAvailable(questId))
		{
			UQuest* quest = (UQuest*)GetQuestData(questId);
			quests.Push(quest);
		}
	}

	return quests;
}

TArray<UQuest*> MMOQuestData::GetCompletedQuestsByType(EQUEST_TYPE type)
{
	TArray<UQuest*> quests;
	for (auto it = _QuestDataMap.begin(); it != _QuestDataMap.end(); ++it)
	{
		const UINT32 questId = it->first;
		if (it->second->QuestType == type
			&& IsQuestAccepted(questId) == FALSE
			&& IsQuestCompleted(questId))
		{
			UQuest* quest = (UQuest*)GetQuestData(questId);
			quests.Push(quest);
		}
	}

	return quests;
}

TArray<UQuest*> MMOQuestData::GetCompassableQuestsByType(EQUEST_TYPE type)
{
	TArray<UQuest*> quests;
	for (auto it = _QuestDataMap.begin(); it != _QuestDataMap.end(); ++it)
	{
		const UINT32 questId = it->first;
		if (it->second->QuestType == type
			&& IsQuestAccepted(questId) == TRUE
			&& IsQuestCompassable(questId))
		{
			UQuest* quest = (UQuest*)GetQuestData(questId);
			quests.Push(quest);
		}
	}

	return quests;
}

bool SortQuestTraceList(INT lQuestId, INT rQuestId)
{
	const UQuest* lQuest = MMOQuestData::getSingleton().GetQuestData(lQuestId);
	const UQuest* rQuest = MMOQuestData::getSingleton().GetQuestData(rQuestId);

	if (!lQuest || !rQuest)
	{
		return false;
	}

	if(!lQuest->IsTimeLimit() && rQuest->IsTimeLimit())
	{
		return true;
	}
	else if(lQuest->IsTimeLimit() && !rQuest->IsTimeLimit())
	{
		return false;
	}

	return lQuest->QuestType < rQuest->QuestType;
}

const QuestTraceList& MMOQuestData::GetAcceptedQuestTraceList()
{
	_AcceptedQuestTraceList.clear();
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		_AcceptedQuestTraceList.push_back(iter->first);
	}

	_AcceptedQuestTraceList.sort(SortQuestTraceList);
	return _AcceptedQuestTraceList;
}

const QuestTraceList& MMOQuestData::GetAvailableQuestTraceList()
{
	_AvailableQuestTraceList.clear();
	for (auto iter = _QuestDataMap.begin(); iter != _QuestDataMap.end(); ++ iter)
	{
		const UINT32 questId = iter->first;
		if (IsQuestAvailable(questId))
		{
			_AvailableQuestTraceList.push_back(questId);
		}
	}

	_AvailableQuestTraceList.sort(SortQuestTraceList);
	return _AvailableQuestTraceList;
}

EQuestHUDStatus MMOQuestData::IsNpcHasMyQuest(INT npcId, TArray<INT>* completeList, TArray<INT>* uncompleteList, TArray<INT>* availableList)
{
	EQuestHUDStatus state = EQHUDS_None;
	const QuestNpcInfo* questNpcInfo = NULL;
	QuestNpcInfoMap::const_iterator iterNpc = _QuestNpcInfoMap.find(npcId);
	if (iterNpc != _QuestNpcInfoMap.end())
	{
		questNpcInfo = &iterNpc->second;
	}

	if (!questNpcInfo)
	{
		return state;
	}

	// whether there are completed quests
	bool hasCompletedTask = false;
	const QuestDataMap& endQuestMap = questNpcInfo->EndQuestList;
	QuestDataMap::const_iterator iterQuest = endQuestMap.begin();
	for (; iterQuest != endQuestMap.end(); ++ iterQuest)
	{
		const UQuest* quest = iterQuest->second;
		if (IsQuestAccepted(quest->QuestTempID) == FALSE)
		{
			continue;
		}

		if (IsQuestCompassable( quest->GetTemplateID()))
		{
			hasCompletedTask = true;
			//
			state = EQHUDS_CanSubmit;
			//add to completed list
			if (completeList != NULL)
			{
				(*completeList).Push(quest->GetTemplateID());
			}
		}
		else if (uncompleteList != NULL)
		{
			//add to uncomplete list
			(*uncompleteList).Push(quest->GetTemplateID());
		}
	}

	// no completed quest, continue checking acceptable quests
	const QuestDataMap& startQuestMap = questNpcInfo->StartQuestList;
	iterQuest = startQuestMap.begin();
	for (; iterQuest != startQuestMap.end(); ++ iterQuest)
	{
		const UQuest* quest = iterQuest->second;
		if (IsQuestAvailable( quest->GetTemplateID()))
		{
			state = hasCompletedTask ? state : EQHUDS_Acceptable;
			if (availableList != NULL)
			{
				(*availableList).Push(quest->GetTemplateID());
			}
		}	
	}

	// check acceptable bounty quests
	if (npcId == 6456)
	{
		RewardQuestList::iterator iter = _RewardQuestList.begin();
		for (; iter != _RewardQuestList.end(); ++ iter)
		{
			if (IsQuestAvailable(iter->QuestId))
			{
				(*availableList).Push(iter->QuestId);
			}	
		}
	}

	return state;
}

INT MMOQuestData::GetAvailableQuestCount()
{
	INT count = 0;
	for (auto it = _QuestDataMap.begin(); it != _QuestDataMap.end(); ++it)
	{
		const UINT32 questId = it->first;
		if (IsQuestAccepted(questId) == FALSE && IsQuestAvailable(questId))
		{
			count ++;
		}
	}

	return count;
}

INT MMOQuestData::GetCompletedQuestCount() const
{
	INT count = 0;
	for (auto it = _QuestDataMap.begin(); it != _QuestDataMap.end(); ++it)
	{
		const UINT32 questId = it->first;
		if (IsQuestAccepted(questId) == FALSE && IsQuestCompleted(questId))
		{
			++ count;
		}
	}
	 
	return count;
}

QuestNpcInfo* MMOQuestData::_GetOrAddQuestNpcInfo(INT npcId)
{
	QuestNpcInfoMap::iterator iter = _QuestNpcInfoMap.find(npcId);
	if (iter != _QuestNpcInfoMap.end())
	{
		return &iter->second;
	}
	else
	{
		iter = _QuestNpcInfoMap.insert(std::make_pair(npcId, QuestNpcInfo())).first;
		return &iter->second;
	}
}

bool MMOQuestData::IsQuestAvailable(INT questId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return FALSE;
	}

	if (IsQuestAccepted(questId))
	{
		return FALSE;
	}

	if (IsQuestCompleted(questId))
	{
		return FALSE;
	}

	const UQuest* quest = GetQuestData((INT)questId);
	if (!quest)
	{
		debugf(NAME_Error, TEXT("[MMOQuestData::CanAccept] quest template not found:%d"), questId);
		return FALSE;
	}

	// whether activity
	if (ActivityManager::getSingleton().IsActivity(questId))
	{
		// whether VIP
		INT vipLevel = 0;
		for (INT i = eVIPTyp_LV1; i < eVIPTyp_LVMax; ++ i)
		{
			if (MMOVipManager::getSingleton().GetMyVipLeftSecond(i) > 0)
			{
				vipLevel = i;
				break;
			}
		}

		if (ActivityManager::getSingleton().GetActivityVipLevel(questId) > vipLevel)
		{
			return FALSE;
		}
	}

	if (quest->QuestType == E_qt_Circle)
	{// circle quest
		// determine whether it is the current circle
		CircleQuestInfo info;
		if (!GetCircleQuestInfo(quest->CircleID, info))
		{
			return FALSE;
		}

		if (info.QuestId != questId)
		{
			return FALSE;
		}
	}

	if (quest->QuestType == E_qt_Emegency)
	{
		INT coolDown = GetEmergencyQuestCoolDown(questId);
		if (coolDown > 0)
		{
			return FALSE;
		}

		// the max number of emergency quests acceptable today has been reached
		if (IsEmegencyQuestAllCompleteToday())
		{
			return FALSE;
		}
	}

	const UINT32 completeCount = GetCompletedQuestCount(questId);
	if (completeCount >= (UINT32)quest->MaxCompleteCount)
	{
		return FALSE;
	}

#define CHECK_AVAILABLE_QUEST_LIMIT_1(name, p1) \
	if (!MMOQuestData::_CheckAvailableQuestLimit_##name(p1)) \
	{ \
		return FALSE; \
	}


#define CHECK_AVAILABLE_QUEST_LIMIT_2(name, p1, p2) \
	if (!MMOQuestData::_CheckAvailableQuestLimit_##name(p1, p2)) \
	{ \
		return FALSE; \
	}

	CHECK_AVAILABLE_QUEST_LIMIT_2(Job, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Sex, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Master, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Disciple, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Husband, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Wife, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(GuildLeader, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(GuildMember, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(PreQuest, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Level, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Reputation, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Money, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Date, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Reputation, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Time, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Item, quest, mainPlayer)
	CHECK_AVAILABLE_QUEST_LIMIT_2(Team, quest, mainPlayer)

	return true;
}

UBOOL MMOQuestData::IsQuestCompassable( INT32 questId ) const
{
	const UQuest* quest = GetQuestData((INT)questId);
	if (!quest)
	{
		debugf(NAME_Error, TEXT("[MMOQuestData::IsQuestCompassable] quest template not found:%d"), questId);
		return FALSE;
	}

	const AcceptedQuestData* data = GetAcceptedQuestData(questId);
	if (!data)
	{
		return FALSE;
	}

#define CHECK_COMPLETE_QUEST_CONDITION_1(name, p1) \
	if (!MMOQuestData::_CheckCompleteQuestCondition_##name(p1)) \
	{ \
		return FALSE; \
	}


#define CHECK_COMPLETE_QUEST_CONDITION_2(name, p1, p2) \
	if (!MMOQuestData::_CheckCompleteQuestCondition_##name(p1, p2)) \
	{ \
		return FALSE; \
	}

	CHECK_COMPLETE_QUEST_CONDITION_2(Time, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(KillMonster, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Item, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(TalkNPC, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Explor, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Level, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Variable, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Reputation, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Buff, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(Doodad, quest, data)
	CHECK_COMPLETE_QUEST_CONDITION_2(KillPlayer, quest, data)

	return true;
}

const AcceptedQuestDataMap& MMOQuestData::GetAcceptedQuests() const
{
	return _AcceptedQuestDataMap;
}

AcceptedQuestDataMap& MMOQuestData::GetAcceptedQuests()
{
	return _AcceptedQuestDataMap;
}

void MMOQuestData::_RemoveAcceptedQuest(INT32 questId)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData& data = iter->second;
		// count kill-monster quests
		const UQuest* quest = GetQuestData((INT)questId);
		if (quest)
		{
			for (int i = 0; i < quest->CheckKillNpc.Num(); ++i)
			{
				if (i < MAX_CONDITION_PER_QUEST)
				{
					const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(i);
					if (killNpc)
					{
						if (killNpc->CheckKillNpcNewId > 0)
						{
							_RemoveMonsterAcceptedQuest(killNpc->CheckKillNpcNewId, questId);
						}
						else if (killNpc->CheckKillNpcGroupId > 0)
						{
							MonsterList monsterList;
							monsterList.clear();
							if (GetGroupMonsterList(killNpc->CheckKillNpcGroupId, monsterList))
							{
								MonsterList::iterator iterMonster = monsterList.begin();
								for (; iterMonster != monsterList.end(); ++iterMonster)
								{
									_RemoveMonsterAcceptedQuest(*iterMonster, questId);
								}
							}
						}
					}
				}
			}
		}

		_AcceptedQuestDataMap.erase(iter);
	}
}

void MMOQuestData::_AddStoryQuestChapterStage(UINT32 chapter, UINT32 stage)
{
	if (chapter <= 0 || stage <= 0)
	{
		return;
	}

	StoryQuestChapterStageMap::iterator iterChapter = _StoryQuestChapterStageMap.find(chapter);
	if (iterChapter == _StoryQuestChapterStageMap.end())
	{
		StoryQuestStageList storyQuestStageList;
		storyQuestStageList.clear();
		storyQuestStageList.push_back(stage);
		_StoryQuestChapterStageMap.insert(std::make_pair(chapter, storyQuestStageList));
	}
	else
	{
		// already there, do not add
		StoryQuestStageList& storyQuestStageList = iterChapter->second;
		StoryQuestStageList::iterator iter = storyQuestStageList.begin();
		for (; iter != storyQuestStageList.end(); ++ iter)
		{
			if (*iter == stage)
			{
				return;
			}
		}

		storyQuestStageList.push_back(stage);
	}
}

void MMOQuestData::_AddStoryQuestPastChapter(UINT32 chapter)
{
	if (chapter <= 0)
	{
		return;
	}

	StoryQuestChapterList::iterator iter = _StoryQuestPastChapterList.begin();
	for (; iter != _StoryQuestPastChapterList.end(); ++ iter)
	{
		if (*iter == chapter)
		{
			return;
		}
	}
	
	_StoryQuestPastChapterList.push_back(chapter);
}

UBOOL MMOQuestData::HasConditions(INT questId)
{
	const UQuest* quest = GetQuestData((INT)questId);
	if (quest)
	{
		for (INT i = 0; i < quest->CheckItem.Num(); ++i)
		{
			if (quest->CheckItem(i).CheckItemID > 0)
			{
				return TRUE;
			}
		}

		for (INT i = 0; i < quest->CheckKillNpc.Num(); ++i)
		{
			if (quest->CheckKillNpc(i).CheckKillNpcNewId > 0)
			{
				return TRUE;
			}

			if (quest->CheckKillNpc(i).CheckKillNpcGroupId > 0)
			{	
				return TRUE;
			}
		}

		for (INT i = 0; i < quest->CheckKillDoodad.Num(); ++i)
		{
			if (quest->CheckKillDoodad(i).CheckKillDoodadID)
			{
				return TRUE;
			}
		}

		for (INT i = 0; i < quest->CheckKillPlayer.Num(); ++i)
		{
			if (quest->CheckKillPlayer(i).CheckKillPlayerCnt > 0)
			{
				return TRUE;
			}
		}

		for (INT i = 0; i < quest->CheckVar.Num(); ++i)
		{
			if (quest->CheckVar(i).CheckVarType == TEXT("vtQuest") &&
				quest->CheckVar(i).CheckVarID >= 0 && 
				quest->CheckVar(i).CheckVarID < MAX_VARIABLE_PER_QUEST)
			{
				return TRUE;
			}
			else if (quest->CheckVar(i).CheckVarType == TEXT("vtClient") &&
				quest->CheckVar(i).CheckVarID == ECVT_Quest)
			{
				return TRUE;
			}
			else if (quest->CheckVar(i).CheckVarType == TEXT("vtItemRecycle") &&
				quest->CheckVar(i).CheckVarID >= 0 && 
				quest->CheckVar(i).CheckVarID < MAX_ROLE_VARIABLE_COUNT)
			{
				return TRUE;
			}
		}

		if (quest->CheckLevel > 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void MMOQuestData::TraceAllQuest(bool traceOn)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		const UQuest* quest = GetQuestData((INT)(iter->first));
		if (quest)
		{
			if (quest->QuestType == E_qt_Trunk)
			{
				// main quests are always tracked
				iter->second.IsTraced = true;
			}
			else
			{
				iter->second.IsTraced = traceOn;
			}
		}
	}
}

void MMOQuestData::TraceQuest(INT questId, bool traceOn)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		const UQuest* quest = GetQuestData(questId);
		if (quest)
		{
			if (quest->QuestType == E_qt_Trunk)
			{
				// main quests are always tracked
				iter->second.IsTraced = true;
			}
			else
			{
				iter->second.IsTraced = traceOn;
			}
		}
	}
}

void MMOQuestData::UpdateQuestNpcKillCount(INT questId, UINT16 index, INT value)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData& data = iter->second;
		if (index < MAX_CONDITION_PER_QUEST)
		{
			data.KillNpcCount[index] = value;

			// count kill-monster quests
			const UQuest* quest = GetQuestData((INT)questId);
			if (quest)
			{
				const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(index);
				if (killNpc)
				{
					if (killNpc->CheckKillNpcNewId && data.KillNpcCount[index] >= killNpc->CheckKillNpcCnt)
					{
						_RemoveMonsterAcceptedQuest(killNpc->CheckKillNpcNewId, questId);
						// update quest state only when quest conditions are met, to avoid stutter
						MMOQuestManager::getSingleton().UpdateQuestStatus();
					} 
					else if (killNpc->CheckKillNpcGroupId && data.KillNpcCount[index] >= killNpc->CheckKillNpcCnt)
					{
						MonsterList monsterList;
						monsterList.clear();
						if (GetGroupMonsterList(killNpc->CheckKillNpcGroupId, monsterList))
						{
							MonsterList::iterator iterMonster = monsterList.begin();
							for (; iterMonster != monsterList.end(); ++ iterMonster)
							{
								_RemoveMonsterAcceptedQuest(*iterMonster, questId);
							}
						}
						// update quest state only when quest conditions are met, to avoid stutter
						MMOQuestManager::getSingleton().UpdateQuestStatus();
					}
				}
			}
		}
	}
}

void MMOQuestData::UpdateRoleVariable(UINT16 index, INT val)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = GetQuestData((INT)data.Id);
		if (quest)
		{
			for (INT i = 0; i < quest->CheckVar.Num(); ++i)
			{
				FQuestCheckVar checkVar = quest->CheckVar(i);
				if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
					checkVar.CheckVarID >= 0 && 
					checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT && 
					checkVar.CheckVarID == index)
				{
					data.RoleVariable[index] = val;
					MMOQuestManager::getSingleton().UpdateQuestTargets(quest->GetTemplateID());
					MMOQuestManager::getSingleton().UpdateQuestStatus();
				}
			}
		}
	}
}

void MMOQuestData::UpdateQuestVariable(INT questId, UINT16 index, INT val)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData& data = iter->second;
		if (index < MAX_VARIABLE_PER_QUEST)
		{
			data.QuestVariable[index] = val;
		}
	}
}

void MMOQuestData::UpdateAllClientVariable(INT32 clientVariable[MAX_CLIENT_VARIABLE_COUNT])
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = GetQuestData((INT)data.Id);
		if (quest)
		{
			for (INT i = 0; i < quest->CheckVar.Num(); ++i)
			{
				FQuestCheckVar checkVar = quest->CheckVar(i);
				if (checkVar.CheckVarType == TEXT("vtClient") &&
					checkVar.CheckVarID == ECVT_Quest)
				{
					data.ClientVariable[checkVar.CheckVarID] = clientVariable[checkVar.CheckVarID];
					MMOQuestManager::getSingleton().UpdateQuestTargets(quest->GetTemplateID());
					MMOQuestManager::getSingleton().UpdateQuestStatus();
				}
			}
		}
	}
}

void MMOQuestData::UpdateClientVariable(UINT16 index, INT val, INT classVal)
{
	if (val == (INT)GQT_Invalid)
	{
		return;
	}

	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = GetQuestData((INT)data.Id);
		if (!quest)
		{
			continue;
		}

		for (INT i = 0; i < quest->CheckVar.Num(); ++ i)
		{
			FQuestCheckVar checkVar = quest->CheckVar(i);
			if (checkVar.CheckVarType == TEXT("vtClient") &&
				checkVar.CheckVarID == ECVT_Quest && 
				checkVar.CheckVarID == index)
			{
				if (classVal > 0)
				{
					for (int j = 0; j < quest->CheckClassVar.Num(); ++ j)
					{
						FQuestCheckClassVar checkClassVar = quest->CheckClassVar(j);
						INT classId = MMOFacade::getSingleton().GetMainActor()->GetClassId();
						if (classId == j && checkClassVar.CheckClassVarCnt == classVal)
						{
							data.ClientVariable[checkVar.CheckVarID] |= 1 << val;
							MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)checkVar.CheckVarID, data.ClientVariable[checkVar.CheckVarID]);
							MMOQuestManager::getSingleton().UpdateQuestTargets(quest->GetTemplateID());
							MMOQuestManager::getSingleton().UpdateQuestStatus();
						}
					}
				}
				else
				{
					data.ClientVariable[checkVar.CheckVarID] |= 1 << val;
					MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)checkVar.CheckVarID, data.ClientVariable[checkVar.CheckVarID]);
					MMOQuestManager::getSingleton().UpdateQuestTargets(quest->GetTemplateID());
					MMOQuestManager::getSingleton().UpdateQuestStatus();
				}
			}
		}
	}
}

void MMOQuestData::SetQuestFailed(INT questId)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData& data = iter->second;
		data.LeftTime = 0;
		data.IsFailed = true;		
	}
}

void MMOQuestData::AbandonAllEmergencyQuest()
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++iter)
	{
		const UQuest* quest = GetQuestData(iter->second.Id);
		if (quest && quest->QuestType == E_qt_Emegency && quest->CanAbandon)
		{
			// Bug:#24687
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer && (mainPlayer->MoveType != EMT_PrepareStall && mainPlayer->MoveType != EMT_InStall))
			{
				CMessageHandler::GetInstance().Send_AbandonQuest((INT)(iter->first));
			}
		}
	}
}

void MMOQuestData::UpdateQuestTime(INT timeCount)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = GetQuestData(iter->second.Id);
		if (quest && quest->IsTimeLimit())
		{
			if (data.IsFailed)
			{
				// emergency quest, auto abandon when time is up
				if (quest->QuestType == E_qt_Emegency && quest->CanAbandon)
				{ 
					// Bug:#24687
					AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
					if (mainPlayer && (mainPlayer->MoveType != EMT_PrepareStall && mainPlayer->MoveType != EMT_InStall))
					{
						CMessageHandler::GetInstance().Send_AbandonQuest((INT)(iter->first));
					}
				}
			} 
			else
			{
				if (data.LeftTime > 0)
				{
					data.LeftTime -= timeCount;
					if (data.LeftTime <= 0)
					{
						data.LeftTime = 0;
						data.IsFailed = true;
					}

					MMOGameEvent_Quest questEvent(MMOGameEvent_Quest::EQET_UpdateQuestTime, quest->GetTemplateID());
					MMOUI::PostGameEvent(&questEvent);
				}
			}
		}
	}
}

void MMOQuestData::UpdateQuestDoodadKillCount(INT questId, UINT16 index, INT val)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData& data = iter->second;
		if (index < MAX_CONDITION_PER_QUEST)
		{
			data.KillDoodadCount[index] = val;
		}
	}
}

void MMOQuestData::UpdateQuestPlayerKillCount(INT questId, UINT16 index, INT val)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.find(questId);
	if (iter != _AcceptedQuestDataMap.end())
	{
		AcceptedQuestData& data = iter->second;
		if (index < CHECK_KILL_PLAYER_CONDITION)
		{
			data.KillPlayerCount[index] = val;
		}
	}
}

void MMOQuestData::UpdateQuestReachLevel(INT curLevel)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		const UQuest* quest = GetQuestData(iter->second.Id);
		if (quest && quest->CheckLevel > 0)
		{
			iter->second.Level = curLevel;
			MMOQuestManager::getSingleton().UpdateQuestTargets(quest->GetTemplateID());
			MMOQuestManager::getSingleton().UpdateQuestStatus();
		}
	}
}

void MMOQuestData::_UpdateAcceptedQuestVariable()
{
	// if the condition was already met before accepting the quest, update the current completion status
	UpdateClientVariable(ECVT_Quest, FriendManager::getSingleton().GetTotalFriendNum() > 0 ? GQT_Friend : GQT_Invalid);
	UpdateClientVariable(ECVT_Quest, MMOGuildData::getSingleton().GetGuildBaseInfo().Id > 0 ? GQT_Guild : GQT_Invalid);
	UpdateClientVariable(ECVT_Quest, TeamManager::getSingleton().HasTeam() ? GQT_Team : GQT_Invalid);
	UpdateClientVariable(ECVT_Quest, MMOFacade::getSingleton().GetContainerSize(E_CONSLOT_DEFAULT) > UCONST_OriBagCap ? GQT_Extendbag : GQT_Invalid);

	// learn-ability quest, if already learned
	TMap<INT, class UMMOAbility*> allLearnedSkills = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();
	for (TMap<INT, class UMMOAbility*>::TIterator iter(allLearnedSkills); iter; ++iter)
	{
		UMMOAbility* mainAbility = iter.Value();
		if (!mainAbility)
		{
			continue;
		}

		UpdateClientVariable(ECVT_Quest, GQT_LearnSkill, mainAbility->GetTemplateID());
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && mainPlayer->ItemManager)
	{
		// obtain-item quest, if already obtained
		for (INT i = 0; i < mainPlayer->ItemManager->BagItemList.Num(); ++i)
		{
			UItem* item = mainPlayer->ItemManager->BagItemList(i);
			if (!item)
			{
				continue;
			}

			MMOQuestManager::getSingleton().UpdateQuestItem(item->GetTemplateID(), FALSE);
			UpdateClientVariable(ECVT_Quest, GQT_GetItem, item->GetTemplateID());
		}

		// equip-item quest, if already equipped
		for (INT i = 0; i < mainPlayer->ItemManager->EquippedItemList_Normal.Num(); ++i)
		{
			UItem* item = mainPlayer->ItemManager->EquippedItemList_Normal(i);
			if (!item)
			{
				continue;
			}

			UpdateClientVariable(ECVT_Quest, GQT_EquipItem, item->GetTemplateID());
		}
	}
}

void MMOQuestData::AcceptQuest(INT questId)
{
	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST questData;
	ZeroMemory((void*)&questData, sizeof(questData));
	questData.nQuestID = questId;
	INT year, month, dayOfWeek, day, hour, min, sec, mSec;
	appSystemTime(year, month, dayOfWeek, day, hour, min, sec, mSec);
	FString timeStr = FString::Printf(TEXT("%d.%d.%d-%d.%d.%d"), year, month, day, hour, min, sec);
	time_t time = appStrToSeconds(*timeStr);
	questData.dwAcceptTime = time;
	_AddAcceptedQuest(questData, true);

	if (quest->QuestType == E_qt_Reward)
	{
		// if it is a bounty quest
		RewardQuestList::iterator iter = _RewardQuestList.begin();
		for (; iter != _RewardQuestList.end(); ++ iter)
		{
			if (iter->QuestId == questId)
			{
				iter->QuestState = EQS_Accept;
				break;
			}
		}
	}

	_AddStoryQuestPastChapter(quest->QuestChapter);

	_UpdateAcceptedQuestVariable();
}

void MMOQuestData::AbandonQuest(INT questId)
{
	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	_RemoveAcceptedQuest(questId);

	if (quest->QuestType == E_qt_Reward)
	{
		RewardQuestList::iterator iter = _RewardQuestList.begin();
		for (; iter != _RewardQuestList.end(); ++ iter)
		{
			if (iter->QuestId == questId)
			{
				iter->QuestState = EQS_Available;
				break;
			}
		}
	}
}

void MMOQuestData::CompleteQuest(INT questId)
{
	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	if (questId > 0 && questId < MAX_NORMAL_QUEST_COUNT)
	{
		_NormalQuest[questId / 8] |= (1 << (questId % 8));
	}
	else if (questId >= REPEAT_QUEST_START_ID && questId < REPEAT_QUEST_START_ID + MAX_REPEAT_QUEST_COUNT)
	{
		++_RepeatQuest[questId - REPEAT_QUEST_START_ID];
	}

	_RemoveAcceptedQuest(questId);

	if (quest->QuestType == E_qt_Reward)
	{// bounty quest
		RewardQuestList::iterator iter = _RewardQuestList.begin();
		for (; iter != _RewardQuestList.end(); ++ iter)
		{
			if (iter->QuestId == questId)
			{
				iter->QuestState = EQS_Completed;
				break;
			}
		}

		if (_RewardQuestFreeCount > 0)
		{
			_RewardQuestFreeCount --;
		} 
		else if (_RewardQuestCostCount > 0)
		{
			_RewardQuestCostCount --;
		}
	}
	else if(quest->QuestType == E_qt_Emegency)
	{
		if (_EmergencyQuestCompleteCount < MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY)
		{
			_EmergencyQuestCompleteCount ++;
		}
	}

	// if it is a circle quest
	if (quest->CircleID > 0)
	{
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			CircleQuestData data;
			if (GetCircleQuestData(quest->CircleID, mainPlayer->ActorAttributes->LevelAttributes.Level, data))
			{
				CircleQuestInfoMap::iterator iter = _CircleQuestInfoMap.find(quest->CircleID);
				if (iter != _CircleQuestInfoMap.end())
				{
					if (iter->second.CircleIndex < data.CircleSize - 1)
					{
						iter->second.CircleIndex ++;
					}
					else
					{
						iter->second.CircleIndex = 0;
						if (iter->second.CircleRound < data.CircleRoundMax)
						{
							iter->second.CircleRound ++;
						}
					}
				}
			}
		}
	}

	// completed story quests
	//_AddStoryQuestChapterStage(quest->QuestChapter, quest->QuestStage);taogetagfdgd
	_AddStoryQuestPastChapter(quest->QuestChapter);
}

void MMOQuestData::GetAwardItemList(INT questId, QuestAwardItemInfoList& list)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	EGenderType gender = (EGenderType)mainPlayer->ActorAttributes->GenderType;
	EPlayerClass job = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;

	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (GetCircleQuestInfo(quest->CircleID, info))
		{
			CircleQuestRewardData awardData;
			if (GetCircleQuestRewardData(info.CircleId, info.CircleIndex, mainPlayer->ActorAttributes->LevelAttributes.Level, awardData))
			{
				for (INT i = 0; i < MAX_CIRCLE_AWARD_ITEM_COUNT; ++ i)
				{
					// filter items with ID 0
					if (awardData.AwardItemId[i] > 0)
					{
						UItem* item = MMOFacade::getSingleton().FindItemTemplate(awardData.AwardItemId[i]);
						if (item)
						{
							QuestAwardItemInfo info;
							info.ItemTemplate = item;
							info.Count = awardData.AwardItemCount[i];
							info.Index = i;
							list.push_back(info);
						}
					}
				}
			}
		}
	}
	else
	{
		for (INT i = 0; i < quest->AwardItem.Num(); ++ i)
		{
			const FQuestAwardItem* awardItem = &quest->AwardItem(i);
			UItem* item = ((UQuest*)quest)->GetAwardItem(i);
			if (item)
			{
				QuestAwardItemInfo info;
				info.ItemTemplate = item;
				info.Count = awardItem->AwardItemCnt;
				info.Index = i;
				list.push_back(info);
			}
		}
	}

	if (list.size() > 4)
	{
		list.resize(4);
		appMsgf(AMT_OK, TEXT("Quest [%d][%s] fixed reward count exceeds the maximum of 4. Please check the item's 'class' and 'gender' restriction parameters!"),
			quest->GetTemplateID(), *quest->QuestName);
	}
}

void MMOQuestData::GetSelectableAwardItemList(INT questId, QuestAwardItemInfoList& list)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	EGenderType gender = (EGenderType)mainPlayer->ActorAttributes->GenderType;
	EPlayerClass job = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;

	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (GetCircleQuestInfo(quest->CircleID, info))
		{
			CircleQuestRewardData awardData;
			if (GetCircleQuestRewardData(info.CircleId, info.CircleIndex, mainPlayer->ActorAttributes->LevelAttributes.Level, awardData))
			{
				for (INT i = 0; i < MAX_SELECTABLE_REWARD_COUNT; ++ i)
				{
					// filter items with ID 0
					if (awardData.SelectableAwardItemId[i] > 0)
					{
						UItem* item = MMOFacade::getSingleton().FindItemTemplate(awardData.SelectableAwardItemId[i]);
						if (item)
						{
							QuestAwardItemInfo info;
							info.ItemTemplate = item;
							info.Count = awardData.SelectableAwardItemCount[i];
							info.Index = i;
							list.push_back(info);
						}
					}
				}
			}
		}
	}
	else
	{
		for (INT i = 0; i < quest->SelectableAwardItem.Num(); ++ i)
		{
			const FQuestSelectableAwardItem* awardItem = &quest->SelectableAwardItem(i);
			UItem* item = ((UQuest*)quest)->GetSelectableAwardItem(i);
			if (item)
			{
				if (_IsItemForMe(job, gender, item))
				{
					QuestAwardItemInfo info;
					info.ItemTemplate = item;
					info.Count = awardItem->SelectableAwardItemCnt;
					info.Index = i;
					list.push_back(info);
				}
			}
		}
	}

	if (list.size() > 4)
	{
		list.resize(4);
		appMsgf(AMT_OK, TEXT("Quest [%d][%s] optional reward count exceeds the maximum of 4. Please check the item's 'class' and 'gender' restriction parameters!"),
			quest->GetTemplateID(), *quest->QuestName);
	}
}


INT MMOQuestData::GetSelectableAwardItemCount(INT questId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return 0;
	}

	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return 0;
	}

	EGenderType gender = (EGenderType)mainPlayer->ActorAttributes->GenderType;
	EPlayerClass job = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;

	INT count = 0;
	for (INT i = 0; i < quest->SelectableAwardItem.Num(); ++ i)
	{
		const FQuestSelectableAwardItem* awardItem = &quest->SelectableAwardItem(i);
		UItem* item = ((UQuest*)quest)->GetSelectableAwardItem(i);
		if (awardItem && item)
		{
			if (_IsItemForMe(job, gender, item))
			{
				++ count;
			}
		}
	}

	return count;
}

INT MMOQuestData::GetDefaultSelectableAwardItemIndex(INT questId)
{
	const UQuest* quest = GetQuestData(questId);
	if (!quest)
	{
		return -1;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return -1;
	}

	if (!mainPlayer->ActorAttributes)
	{
		return -1;
	}

	EGenderType gender = (EGenderType)mainPlayer->ActorAttributes->GenderType;
	EPlayerClass job = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;

	for (INT i = 0; i < quest->SelectableAwardItem.Num(); ++ i)
	{
		const FQuestSelectableAwardItem* awardItem = &quest->SelectableAwardItem(i);
		UItem* item = ((UQuest*)quest)->GetSelectableAwardItem(i);
		if (awardItem && item)
		{
			if (_IsItemForMe(job, gender, item))
			{
				return i;
			}
		}
	}

	return -1;
}


const RewardQuestList& MMOQuestData::GetRewardQuestList() const
{
	return _RewardQuestList;
}

const QuestDataList& MMOQuestData::GetLevelTriggerQuestList() const
{
	return _LevelTriggerQuestList;
}

void MMOQuestData::SetRewardQuestCostCount(INT count)
{
	_RewardQuestCostCount = count;
}

void MMOQuestData::UpdateRewardQuestList(const INT32 (&rewardQuestId)[MAX_REWARD_QUEST_COUNT_PER_ROLE])
{
	_RewardQuestList.clear();

	// put accepted quests in first
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		const UQuest* quest = GetQuestData(iter->second.Id);
		if (quest && quest->QuestType == E_qt_Reward && quest->GetTemplateID() != INVALID_QUEST_ID)
		{
			RewardQuestInfo tInfo;
			tInfo.QuestId = quest->GetTemplateID();
			tInfo.QuestState = EQS_Accept;
			_RewardQuestList.push_back(tInfo);
		}
	}

	// then put new quests in
	for (UINT8 i = 0; i < MAX_REWARD_QUEST_COUNT_PER_ROLE; ++ i)
	{
		bool bExist = false;
		RewardQuestList::iterator iter = _RewardQuestList.begin();
		for (; iter != _RewardQuestList.end(); ++ iter)
		{
			if (iter->QuestId == rewardQuestId[i])
			{
				bExist = true;
			}
		}

		if (bExist)
		{
			continue;
		}

		if (rewardQuestId[i] != INVALID_QUEST_ID)
		{
			RewardQuestInfo tInfo;
			tInfo.QuestId = rewardQuestId[i];
			tInfo.QuestState = EQS_Available;
			_RewardQuestList.push_back(tInfo);
		}
	}
}

INT MMOQuestData::GetAcceptedRewardQuestCount()
{
	UINT32 count = 0;
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		const UQuest* quest = GetQuestData(iter->second.Id);
		if (quest && quest->QuestType == E_qt_Reward && quest->GetTemplateID() != INVALID_QUEST_ID)
		{
			count ++;
		}
	}

	return count;
}

INT MMOQuestData::GetTopLevelRewardQuestCount()
{
	UINT32 count = 0;
	RewardQuestList::iterator iter = _RewardQuestList.begin();
	for (; iter != _RewardQuestList.end(); ++ iter)
	{
		const UQuest* quest = GetQuestData(iter->QuestId);
		// top-tier quests are defined as three stars or above (#9924)
		if (quest && quest->StarLevel >= 3 && !IsQuestAccepted(quest->GetTemplateID()))
		{
			count ++;
		}
	}

	return count;
}

UBOOL MMOQuestData::GetCircleQuestData(INT circleId, INT playerLevel, CircleQuestData& data)
{
	CircleQuestDataMap::iterator iter = _CircleQuestDataMap.find(MAKE_CIRCLE_QUEST_KEY(circleId, 0, playerLevel));
	if (iter != _CircleQuestDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

UBOOL MMOQuestData::GetCircleQuestRewardData(INT circleId, INT circleIndex, INT playerLevel, CircleQuestRewardData& reward)
{
	CircleQuestData data;
	if (GetCircleQuestData(circleId, playerLevel, data))
	{
		if (circleIndex >= 0 && circleIndex < data.CircleSize)
		{
			CircleQuestRewardDataMap::iterator iter = _CircleQuestRewardDataMap.find(data.RewardId[circleIndex]);
			if (iter != _CircleQuestRewardDataMap.end())
			{
				reward = iter->second;
				return TRUE;
			}
		}
	}
	return FALSE;
}

UBOOL MMOQuestData::GetCircleQuestInfo(INT circleId, CircleQuestInfo& info) const
{
	CircleQuestInfoMap::const_iterator iter = _CircleQuestInfoMap.find(circleId);
	if (iter != _CircleQuestInfoMap.end())
	{
		info = iter->second;
		return TRUE;
	}

	return FALSE;
}

void MMOQuestData::SetNextCircleQuest(INT questId, INT circleId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		CircleQuestData data;
		if (GetCircleQuestData(circleId, mainPlayer->ActorAttributes->LevelAttributes.Level, data))
		{
			CircleQuestInfoMap::iterator iter = _CircleQuestInfoMap.find(circleId);
			if (iter != _CircleQuestInfoMap.end())
			{
				iter->second.QuestId = questId;
			}
		}
	}
}

UBOOL MMOQuestData::HasCircleQuest()
{
	CircleQuestInfoMap::const_iterator iter = _CircleQuestInfoMap.begin();
	for (; iter != _CircleQuestInfoMap.end(); ++ iter)
	{
		if (iter->second.QuestId != INVALID_QUEST_ID)
		{
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Job(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->ClassLimit != EPCT_None)
	{
		EPlayerClass playerClass = (EPlayerClass)mainPlayer->GetClassId();
		if (playerClass != quest->ClassLimit)
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Sex(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->SexLimit != EGT_None)
	{
		EGenderType gender = (EGenderType)mainPlayer->GetSex();
		if (gender != quest->SexLimit)
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Master(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->RelationLimit == E_qrt_Master)
	{
		if (MasterStudentManager::getSingleton().GetMasterStudentState() != E_MASTER_TEACHER)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Disciple(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->RelationLimit == E_qrt_Disciple)
	{
		if (MasterStudentManager::getSingleton().GetMasterStudentState() != E_MASTER_STUDENT)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Husband(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->RelationLimit == E_qrt_Husband)
	{
		return FALSE;
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Wife(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->RelationLimit == E_qrt_Wife)
	{
		return FALSE;
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_GuildLeader(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->RelationLimit == E_qrt_GuildLeader)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if (baseInfo.Id == 0)
		{
			return FALSE;
		}

		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainPlayer->ServerID);
			if (post != GUILD_POSITION_LEADER)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_GuildMember(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->RelationLimit == E_qrt_GuildMember)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if (baseInfo.Id == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Team(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (!TeamManager::getSingleton().HasTeam())
	{
		return TRUE;
	}
	
	TArray<MMOTeamMember>& teamMemberList = TeamManager::getSingleton().GetTeamMemberList();
	for (INT i = 0; i < teamMemberList.Num(); ++ i)
	{
		const MMOTeamMember& teamMember = teamMemberList(i);
	}
	/*
	if (quest->RelationLimit == E_qrt_TeamMaster)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if (baseInfo.Id == 0)
		{
			return FALSE;
		}
	}
	else if (quest->RelationLimit == E_qrt_TeamHusband)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if (baseInfo.Id == 0)
		{
			return FALSE;
		}
	}
	else if (quest->RelationLimit == E_qrt_TeamGuild)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if (baseInfo.Id == 0)
		{
			return FALSE;
		}
	}
	*/
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_PreQuest(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->PreQuestId1 > 0)
	{
		const UINT32 completeCount = GetCompletedQuestCount(quest->PreQuestId1);
		if (completeCount == 0)
		{
			return FALSE;
		}
	}

	if (quest->PreQuestTempID2 > 0)
	{
		const UINT32 completeCount = GetCompletedQuestCount(quest->PreQuestId2);
		if (completeCount == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Level(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}
	
	// Quest #25516
	if (quest->QuestLevel >= 39)
	{
		UINT32 clientOption = CMessageHandler::GetInstance().GetClientOption();
		if ((clientOption & CLIENT_OPTION_BIT_GUILD_WAR) == 0)
		{
			return FALSE;
		}
	}
	
	INT playerLevel = mainPlayer->GetActorLevel();
	if (playerLevel < quest->MinLevelLimit || playerLevel > quest->MaxLevelLimit)
	{
		return FALSE;
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Reputation(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->ReputationMaxLimit)
	{
		if (MMOFacade::getSingleton().GetPlayerReputation(mainPlayer,quest->ReputationForceID) > quest->ReputationMaxLimit)
		{
			return FALSE;
		}
	}

	if (quest->ReputationMinLimit)
	{
		if (MMOFacade::getSingleton().GetPlayerReputation(mainPlayer, quest->ReputationForceID) < quest->ReputationMinLimit)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Money(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	if (quest->MoneyAmountLimit)
	{
		if (MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)quest->MoneyTypeLimit) < quest->MoneyAmountLimit)
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Date(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	INT YearNow, MonthNow, DayOfWeekNow, DayNow, HourNow, MinNow, SecNow, MSecNow;
	appSystemTime(YearNow, MonthNow, DayOfWeekNow, DayNow, HourNow, MinNow, SecNow, MSecNow);
	const UINT32 currentTime = time(NULL);

	if (quest->YearLimit && quest->YearLimit != YearNow)
	{
		return FALSE;
	}

	if (quest->MonthLimit > 0 && quest->MonthLimit != MonthNow)
	{
		return FALSE;
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckAvailableQuestLimit_Time(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	INT Year, Month, DayOfWeek, Day, Hour, Min, Sec;
	const UINT32 lastTime = GetQuestCompletedDate(quest->GetTemplateID(), Year, Month, Day, DayOfWeek, Hour, Min, Sec);

	INT YearNow, MonthNow, DayOfWeekNow, DayNow, HourNow, MinNow, SecNow, MSecNow;
	appSystemTime(YearNow, MonthNow, DayOfWeekNow, DayNow, HourNow, MinNow, SecNow, MSecNow);
	const UINT32 currentTime = time(NULL);

	if (quest->AcceptTimeStart.Month > 0 && quest->AcceptTimeStart.Day > 0
		&& quest->AcceptTimeEnd.Month > 0 && quest->AcceptTimeEnd.Day > 0)
	{
		FString startTime = FString::Printf(TEXT("%d.%d.%d-%d.%d.%d"), 
			YearNow,
			quest->AcceptTimeStart.Month ? quest->AcceptTimeStart.Month : MonthNow,
			quest->AcceptTimeStart.Day ? quest->AcceptTimeStart.Day : DayNow,
			quest->AcceptTimeStart.Hour, quest->AcceptTimeStart.Minute, quest->AcceptTimeStart.Second);

		FString endTime = FString::Printf(TEXT("%d.%d.%d-%d.%d.%d"), 
			YearNow,
			quest->AcceptTimeEnd.Month ? quest->AcceptTimeEnd.Month : MonthNow,
			quest->AcceptTimeEnd.Day ? quest->AcceptTimeEnd.Day: DayNow,
			quest->AcceptTimeEnd.Hour, quest->AcceptTimeEnd.Minute, quest->AcceptTimeEnd.Second);

		time_t tStart = appStrToSeconds(*startTime);
		time_t tEnd = appStrToSeconds(*endTime);

		if (currentTime < tStart || currentTime > tEnd)
		{
			return FALSE;
		}
	}

#if 0 // TODO: time limit
	if (quest->WeekdayLimit != -1 && quest->WeekdayLimit != DayOfWeekNow)
		return false;

	if (quest->DateLimit && quest->DateLimit != DayNow)
		return false;

	if (quest->DateLimit && quest->Duration >= 0)
	{
		FString startDate = FString::Printf(TEXT("%d.%d.%d-0.0.0"), 
			quest->YearLimit ? quest->YearLimit : YearNow,
			quest->MonthLimit ? quest->MonthLimit : MonthNow,
			quest->DateLimit);
		time_t t = appStrToSeconds(*startDate);

		if (currentTime > t + quest->Duration * 86400 || currentTime < t)
			return false;
	}

	if (quest->WeekdayLimit != -1 && quest->Duration >= 0)
	{
		INT startDate = quest->DateLimit;
		INT endDate = (quest->DateLimit + quest->Duration) % 7;

		if (endDate > startDate)
		{
			if (DayOfWeekNow < startDate || DayOfWeekNow > endDate)
				return false;
		}
		else
		{
			if (DayOfWeekNow < startDate && DayOfWeekNow > endDate)
				return false;
		}
	}
#endif

	return TRUE;
}


UBOOL MMOQuestData::_CheckAvailableQuestLimit_Item(const UQuest* quest, AMMOPlayer* mainPlayer) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!mainPlayer)
	{
		return FALSE;
	}

	for (INT i = 0; i < quest->ItemLimit.Num(); ++ i)
	{
		if (quest->ItemLimit(i).CheckItemID > 1)
		{
			INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(quest->ItemLimit(i).CheckItemID, E_CONSLOT_DEFAULT);
			if (itemCount < quest->ItemLimit(i).CheckItemCnt)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Time(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	if (quest->IsTimeLimit())
	{
		if (data->LeftTime <= 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_KillMonster(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	for (int i = 0; i < quest->CheckKillNpc.Num(); ++ i)
	{
		auto killNpc = quest->CheckKillNpc(i);
		if (killNpc.CheckKillNpcNewId > 0)
		{
			if ((data->KillNpcCount)[i] < killNpc.CheckKillNpcCnt)
			{
				return FALSE;
			}
		}
		else if (killNpc.CheckKillNpcGroupId > 0)
		{
			if ((data->KillNpcCount)[i] < killNpc.CheckKillNpcCnt)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_KillPlayer(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	for (int i = 0; i < quest->CheckKillPlayer.Num(); ++ i)
	{
		auto killPlayer = quest->CheckKillPlayer(i);
		if (killPlayer.CheckKillPlayerCnt > 0 && (data->KillPlayerCount)[i] < killPlayer.CheckKillPlayerCnt)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Item(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	for (int i = 0; i < quest->CheckItem.Num(); ++ i)
	{
		auto checkItems = quest->CheckItem(i);
		if (quest->CheckItem(i).CheckItemID > 0)
		{
			INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(quest->CheckItem(i).CheckItemID, E_CONSLOT_DEFAULT);
			if (itemCount < checkItems.CheckItemCnt)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_TalkNPC(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	if (quest->TalkNpcID != 0)
	{
		if ((quest->TalkVarID >= 0) &&
			(quest->TalkVarID < MAX_VARIABLE_PER_QUEST) && 
			(data->QuestVariable[quest->TalkVarID] != 1))
		{
			return FALSE;
		}
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Explor(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	if ((quest->ExplorVarID >= 0) &&
		(quest->ExplorVarID < MAX_VARIABLE_PER_QUEST) &&
		(data->QuestVariable[quest->ExplorVarID] != 1))
	{
		return FALSE; 
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Level(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	if (quest->CheckLevel > 0)
	{
		if (data->Level < quest->CheckLevel)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Variable(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	for (int i = 0; i < quest->CheckVar.Num(); ++ i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType == TEXT("vtQuest") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_VARIABLE_PER_QUEST)
		{
			if (data->QuestVariable[checkVar.CheckVarID] != checkVar.CheckVarCnt)
			{
				return FALSE;
			}
		}
		else if (checkVar.CheckVarType == TEXT("vtClient") &&
			checkVar.CheckVarID == ECVT_Quest)
		{
			if ((data->ClientVariable[checkVar.CheckVarID] & (INT32)(1 << checkVar.CheckVarCnt)) == 0)
			{
				return FALSE;
			}
		}
		else if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT)
		{
			if (data->RoleVariable[checkVar.CheckVarID] != checkVar.CheckVarCnt)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Reputation(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	if (quest->CheckReputationForceID)
	{
		INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), quest->CheckReputationForceID);
		if (reputation < quest->CheckReputation)
		{
			return FALSE;
		}
	}
	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Buff(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	return TRUE;
}

UBOOL MMOQuestData::_CheckCompleteQuestCondition_Doodad(const UQuest* quest, const AcceptedQuestData* data) const
{
	if (!quest)
	{
		return FALSE;
	}

	if (!data)
	{
		return FALSE;
	}

	for (int i = 0; i < quest->CheckKillDoodad.Num(); ++ i)
	{
		const FQuestCheckKillDoodad* checkKillDoodad = &quest->CheckKillDoodad(i);	
		if (checkKillDoodad->CheckKillDoodadID && 
			(data->KillDoodadCount)[i] < checkKillDoodad->CheckKillDoodadCnt)
		{
			return FALSE;
		}
	}
	return TRUE;
}

bool MMOQuestData::_IsItemForMe(EPlayerClass job, EGenderType gender, const UItem* item)
{
	if (!item)
	{
		return false;
	}

	return ((item->RequireGender == EGenderType::EGT_None) || (item->RequireGender == gender)) && 
		(MMO::IsForAllPlayerClass(item->RequireClass) || MMO::CanBeUsedByPlayerClass(job, item->RequireClass));
}

EQestState MMOQuestData::GetQuestState(INT32 questId)
{
	if (questId == INVALID_QUEST_ID)
	{
		return EQS_Null;
	}

	if (IsQuestAvailable(questId))
	{
		return EQS_Available;
	}

	if (IsQuestAccepted(questId))
	{
		return EQS_Accept;
	}
	
	if (IsQuestCompleted(questId))
	{
		return EQS_Completed;
	}

	return EQS_Null;
}

UBOOL MMOQuestData::GetStoryQuestData(UINT32 chapter, UINT32 stage, StoryQuestData& data)
{
	StoryQuestDataMap::iterator iter = _StoryQuestDataMap.find(MAKE_STORY_QUEST_KEY(chapter, stage));
	if (iter != _StoryQuestDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

const StoryQuestChapterStageMap& MMOQuestData::GetStoryQuestChapterStageMap()
{
	return _StoryQuestChapterStageMap;
}

const StoryQuestChapterList& MMOQuestData::GetStoryQuestChapterList()
{
	return _StoryQuestPastChapterList;
}

UBOOL MMOQuestData::IsStoryQuestPast(INT chapter, INT stage)
{
	AcceptedQuestDataMap::iterator iter = _AcceptedQuestDataMap.begin();
	for (; iter != _AcceptedQuestDataMap.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = GetQuestData((INT)data.Id);
		if (quest)
		{
			if (quest->QuestChapter == 0 || quest->QuestStage == 0)
			{//non-story quest
				continue;
			}

			if (quest->QuestChapter >= chapter && quest->QuestStage >= stage)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

const QuestMapDataMap& MMOQuestData::GetQuestMapDataMap()
{
	return _QuestMapDataMap;
}

UBOOL MMOQuestData::_LoadQuestRecyle(const TCHAR* filePath)
{
	_QuestRecycleDataList.clear();
	QuestRecycleDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOQuestData::AddQuestRecycle(const QuestRecycleData& questRecycle)
{
	_QuestRecycleDataList.push_back(questRecycle);
}
void MMOQuestData::_UnloadQuestRecyle()
{
	_QuestRecycleDataList.clear();
}

UBOOL MMOQuestData::IsQuestRecycle(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return FALSE;
	}

	for (INT i = 0; i < quest->CheckVar.Num(); ++i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT)
		{
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOQuestData::_LoadRewardQuestCostData(const TCHAR* filePath)
{
	_RewardQuestCostDataMap.clear();
	RewardQuestCostDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOQuestData::AddRewardQuestCostData(const RewardQuestCostData& data)
{
	_RewardQuestCostDataMap.insert(std::make_pair(data.Time, data));
}

void MMOQuestData::_UnloadRewardQuestCostData()
{
	_RewardQuestCostDataMap.clear();
}

INT MMOQuestData::GetRewardQuestCost(INT time)
{
	RewardQuestCostDataMap::iterator iter = _RewardQuestCostDataMap.find(time);
	if (iter != _RewardQuestCostDataMap.end())
	{
		return iter->second.Cost;
	}

	return 0;
}

void MMOQuestData::AddEmergencyQuestCoolDown(INT questId, INT coolDown)
{
	EmergencyQuestCoolDownMap::iterator iter = _EmergencyQuestCoolDownMap.find(questId);
	if (iter != _EmergencyQuestCoolDownMap.end())
	{
		iter->second.CoolDown = coolDown;
	}
	else
	{
		EmergencyQuestCoolDownInfo info;
		info.QuestId = questId;
		info.CoolDown = coolDown;
		_EmergencyQuestCoolDownMap.insert(std::make_pair(questId, info));
	}
}

void MMOQuestData::UpdateEmergencyQuestCoolDown(INT deltaTime)
{
	EmergencyQuestCoolDownMap::iterator iter = _EmergencyQuestCoolDownMap.begin();
	for (; iter != _EmergencyQuestCoolDownMap.end();)
	{
		if (iter->second.CoolDown > 0)
		{
			iter->second.CoolDown -= deltaTime;
			iter ++;
		} 
		else
		{
			// notify map icons to refresh
			MMOGameEvent_Any evt(GameEvent_QuestStateChanged);
			MMOUI::PostGameEvent(&evt);

			iter = _EmergencyQuestCoolDownMap.erase(iter);
		}
	}
}

INT MMOQuestData::GetEmergencyQuestCoolDown(INT questId) const
{
	EmergencyQuestCoolDownMap::const_iterator iter = _EmergencyQuestCoolDownMap.find(questId);
	if (iter != _EmergencyQuestCoolDownMap.end())
	{
		return iter->second.CoolDown;
	}

	return 0;
}

UBOOL MMOQuestData::IsSingleMonsterQuest(INT questId)
{
	// Defect 11480, the case where the whole quest has only one monster
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return FALSE;
	}

	int monsterCount = 0;
	for (INT i = 0; i < quest->CheckKillNpc.Num(); ++ i)
	{
		const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(i);
		if (killNpc && killNpc->CheckKillNpcNewId > 0 && killNpc->CheckKillNpcCnt > 0)
		{
			monsterCount += killNpc->CheckKillNpcCnt;
		}
	}

	return monsterCount == 1;
}

UBOOL MMOQuestData::IsEmegencyQuestAllCompleteToday()
{
	return _EmergencyQuestCompleteCount >= MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY;
}

void MMOQuestData::ClearMonsterGroupData()
{
	_MonsterGroupMap.clear();
}

void MMOQuestData::AddMonsterGroupData(INT groupId, INT monsterId)
{
	MonsterGroupMap::iterator iter = _MonsterGroupMap.find(groupId);
	if (iter != _MonsterGroupMap.end())
	{
		iter->second.push_back(monsterId);
	}
	else
	{
		MonsterList monsterList;
		monsterList.clear();
		monsterList.push_back(monsterId);
		_MonsterGroupMap.insert(std::make_pair(groupId, monsterList));
	}
}

UBOOL MMOQuestData::GetGroupMonsterList(INT groupId, MonsterList& list)
{
	MonsterGroupMap::iterator iter = _MonsterGroupMap.find(groupId);
	if (iter != _MonsterGroupMap.end())
	{
		list = iter->second;
		return TRUE;
	}

	return FALSE;
}