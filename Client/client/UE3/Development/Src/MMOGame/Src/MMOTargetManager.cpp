/**
 * jimZhang@sdna.com 2013.1.22 
 *
 * 面向抽象逻辑对象的状态触发机制
 * 
 *
 *
 *
 */

#include "StdAfx.h"
#include "MMOTargetManager.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"

#define EPSILON			0.0001f
#define CLICK_TIME		0.3f
#define TIME_SYNC_FAST 	0.125f
#define TIME_SYNC_SLOW 	0.6f


MMOTargetManager::MMOTargetManager(void)
{
	Clear();
}

void MMOTargetManager::Clear()
{
	stateMainPlayer =	NULL;		//主角
	stateController	=	NULL;		//主控制
	stateStallState	=	FALSE;		//摆摊状态

	stateLeftBtnAbility =NULL;		//左键装技能

	stateDesireAbility =NULL;		//欲释放技能
	stateAbility	=	NULL;		//施放的技能
	stateKeyPressed	=	FALSE;		//按下键	
	stateShiftDown	=	FALSE;		//shift是否按下
	stateLeftBtnDown=	FALSE;		//鼠标左键是否按下
	stateLeftBtn	=	FALSE;		//鼠标左键事件	
	stateAbilityEnable= FALSE;		//是否可用技能
	stateMoveEnable	=	FALSE;		//可以行走
	stateMoveKeyDown=	FALSE;		//移动按键有按
	statePathOptimize = FALSE;		//寻路点是否为不为直线或者优化过
	statePickTarget=	FALSE;		//


	stateTargetActor=	NULL;		//目标actor
	stateTargetMMOActor=	NULL;	//目标mmoactor
	stateTargetMMOPlayer=	NULL;	//目标mmoplayer
	stateTargetMMOActorID		=0;		//
	stateLastTargetMMOActorID	=0;		//
	stateTargetNPC	=	NULL;		//目标npc
	//stateTargetItem	=	NULL;		//目标捡取物
	stateAttackble	=	FALSE;		//目标是否可攻击
	stateTeleportOrig	=	NULL;	//目标传送点
	stateTeleportNetNode=	NULL;	//目标传送网点
	stateIsDead			=	NULL;	//目标是否死亡
	stateIsShowTips		=	FALSE;

	stateAttackMMOActor = NULL;
	stateSelectMMOActor	= NULL;

	//stateTargetPos=		NULL;		//目标点
	stateLastTargetActor	= NULL;
	stateLastTargetMMOActor = NULL;

	stateDesireActor=	NULL;		//初始目标actor
	stateDesireMMOActor=NULL;		//初始目标mmoactor
	stateLastDesireMMOActor=NULL;	//
	//stateDesireMMOPlayer=	NULL;	//初始目标mmoplayer
	stateDesireNPC=		NULL;		//初始目标npc
	stateDesireNPCTempleteID = 0;	
// 	stateDesireItem=	NULL;		//初始目标捡取物
	stateDesireAttackble=NULL;		//初始目标是否可攻击
	//stateDesirePos=		NULL;		//初始目标点
	stateDoodadActor	= NULL;

	stateFocusActor		= NULL;		//焦点actor
	stateFocusMMOActor	= NULL;		//焦点mmoactor
	stateFocusMMOPlayer	= NULL;		//焦点mmoplayer


	stateCtrlState	=	CS_IDLE;	//角色控制状态，根据研究暗黑3得到两个状态，加上一个待机态
	stateDesireState=   DS_NORMAL;	//触发状态

	statePressedTime=	0.0f;		//按键累计时间;
	stateFastTime	=	0.0f;		//快循环计时;
	stateSlowTime	=	0.0f;		//慢循环计时;
	stateAiCdTime	=	0.0f;		//ai控制型cd;
	stateAbilityRespondTime=0.0f;	//等待技能消息回应时间;


	stateInterrupt	=	NULL;
	stateDesireMMOPlayer = NULL;
	stateSetFocusDelayTime = 0.0f;
	TimeQTE = -0.1f;
	stateWarningTime = 0.0;

	AresFlag = 0;		
	AresIndex = 0;
	AresDeltaTime = 0.0;
}

void MMOTargetManager::EnterScene()
{
	ResetSafeZoneDelayTime();
}

void MMOTargetManager::AITick(FLOAT DeltaTime)
{
	if(GIsEditor)
	{
		return;
	}

	//更新各种状态
	UpdateState(DeltaTime);

	//根据状态判断各分支---------------------------------------------------
		//按shift键施法
		stateShiftAbility();
		//原地释放技能
		stateStayAndUseAbility();
		//追击并释放技能
		stateMoveToAndUseAbility();
		//搜索敌人并释放技能
		stateSearchAndUseAbility();
		//走近并交互
		stateMoveToAndInteract();
		//走近并摆摊
		//stateMoveToAndStall();
		//走近捡东西
// 		stateMoveToPickItem();
		//走到目标点
		stateMoveToPoint();
		//走到指定坐标点
		stateMoveToDesirePoint();
		//键盘控制移动
		stateMoveByKey();
		//显示目标信息及头像
		stateShowInfo();
		//选中物体tips显示
		stateShowTips();
		//选中物体脚底光标显示
		stateShowSelectMark();
		//自动寻怪
		stateSearchEnemy();
		//自动切换键状态
		stateAIContral();
		//自动检测在安全区内收刀
		stateAIInSafeZone();
		//自动是否要释放解qte技能
		stateAIBossQTE();
		//摆摊时候点击处理
		stateClickStallState();
		//特殊的定时提示，比如主武器的耐久度不等间隔警示
		stateStackWarningState();
		//智能清除分解，鉴定，修理等状态，
		stateReleaseOperation();
		//gm 组合测试，自动成为战神
		stateUpdateAres();

	//----------------------------------------------------

	if(	stateDesireState==DS_TRIGGER )
	{
		stateDesireState = DS_HOLDON;
	}
	if( stateDesireState==DS_RELEASE )
	{
		stateDesireState = DS_NORMAL;
	}

	stateLastDesireMMOActor = stateDesireMMOActor;

	while(stateFastTime>TIME_SYNC_FAST)stateFastTime-=TIME_SYNC_FAST;
	while(stateSlowTime>TIME_SYNC_SLOW)stateSlowTime-=TIME_SYNC_SLOW;

}

UBOOL MMOTargetManager::UpdateState(FLOAT DeltaTime)
{

	//时间更新
	
	stateFastTime	+=	DeltaTime;
	stateSlowTime	+=	DeltaTime;
	if(statePressedTime)statePressedTime +=	DeltaTime;
	if(stateAiCdTime>0.0f)stateAiCdTime	+=	DeltaTime;	
	if(stateSetFocusDelayTime>0.0f)
		stateSetFocusDelayTime += DeltaTime;

	if(stateAiInSafeZoneTime>0.0f)
		stateAiInSafeZoneTime -= DeltaTime;

	if(TimeQTE>0.0f)
		TimeQTE -= DeltaTime;

	if(	AresFlag>0) 	
		AresDeltaTime -= DeltaTime;

	//警告更新
	stateWarningTime+=  DeltaTime;

	//控制更新
	stateMainPlayer =	MMOFacade::getSingleton().GetMainPlayer();
	stateController	=	MMOFacade::getSingleton().GetMainController();
	if(stateMainPlayer==NULL)return FALSE;
	if(stateController==NULL)return FALSE;

	stateStallState = stateMainPlayer->eventIsStallState();

	//消息冰冻时间更新
	if(stateController && stateController->WaitingForTheAbilityRespond>0)
	{
		stateAbilityRespondTime += DeltaTime;
		if(stateAbilityRespondTime>0.2f)
		{
			stateController->WaitingForTheAbilityRespond = 0;
			stateAbilityRespondTime = 0;
		}
	}

	if(stateTargetMMOActor && stateTargetMMOActor!=stateLastTargetMMOActor)
	{
		stateSetFocusDelayTime = 0.01f;
	}
	else if(stateTargetMMOActor!=stateLastTargetMMOActor)
	{
		stateSetFocusDelayTime = -1.0f;
	}


	//延时申请焦点详细信息
	if(stateSetFocusDelayTime>1.0f)
	{
		stateSetFocusDelayTime = -1.0f;
		if(stateTargetMMOActor)
			stateController->SendRequestSetFocus(stateTargetMMOActor->ServerID);
	}

	//判定能否穿透UI
	MMOFacade::UpdateCaptureInput();

	//捡取鼠标点击信息
	LastMouseHitNavMeshLocation = stateController->MouseHitNavMeshLocation;
	// zhaojun@snda
	UBOOL bNavMeshPicked = stateController->NativeGetPickPointNavMesh();
	if (!bNavMeshPicked)
	{
		stateController->MouseHitNavMeshLocation = stateController->MouseHitWorldLocation;
	}
	// ~zhaojun@snda
	stateController->NativeGetPickPoint();

	UBOOL CaptureInput = MMOFacade::GetCaptureInput();
	if(CaptureInput==FALSE)
	{
		SetTargetActor(stateController->GetTraceActor());
	}
	else
	{
		SetTargetActor(NULL);
	}
	//激发的技能进行更新
	stateAbility=OptimizeAbility(stateDesireAbility); 
	if(stateAbility&&stateAbility->ClientCommonAttribute.PickTarget)
	{
		statePickTarget = TRUE;
	}
	else
	{
		statePickTarget = FALSE;
	}
	//技能是否使用
	stateAbilityEnable	= stateController->CanUseAnyAbility(stateAbility);
	//蓝不够	
	if(stateAbility && stateMainPlayer && stateAbility->CanUseManaCheck(stateMainPlayer)==FALSE)
	{
		stateAbilityEnable = FALSE;
	}

	//if(stateAbility && MMOFacade::getSingleton().GetMyCurrentMP() < stateAbility->SpellCommonAttribute.InitialCostValue + stateAbility->SpellCommonAttribute.InitialCostValuePct*MMOFacade::getSingleton().GetMyMaxMP()/10000 )
	//{
	//	stateAbilityEnable = FALSE;
	//}

	//是否可以移动
	stateMoveEnable		= stateMainPlayer->CanMove();
	//有移动按键按下
	stateMoveKeyDown	= (MMOUI::GetUIManager()->WSADKey[0]!=EMCK_Empty);
		
	if(	(stateDesireMMOActor && MMOFacade::getSingleton().GetCharacter(stateDesireMMOActor->ServerID)==NULL ) ||	//如果被释放掉 
		(stateDesireMMOActor && stateDesireMMOActor->eventIsDead())		)										//如果已经死了
	{
		stateDesireActor	=	NULL;		
		stateDesireMMOActor	=	NULL;
		//stateDesireMMOPlayer=	NULL;
		stateDesireNPC=			NULL;
		stateDesireNPCTempleteID=0;
		//stateDesireItem=		NULL;
		stateDesireAttackble=	FALSE;
	}

	//寻路点是否为不为直线或者优化过
	statePathOptimize	= stateController->bLinearMovement; /*&& stateController->bOptimizeTargetPos==FALSE*///zhaojun@snda ;

	//shift,鼠标左键按下等状态更新
	stateShiftDown	=stateController->eventIsLeftShiftDown();
	stateLeftBtnDown=stateController->bLeftMousePressed;

	if(stateTargetMMOActor==NULL)
	{
		stateTargetPos = OptimizeTargetPos(stateController->MouseHitNavMeshLocation/*MouseHitWorldLocation*/);
	}
	else
	{
		stateTargetPos = stateController->MouseHitNavMeshLocation;
		//stateTargetPos = stateController->MouseHitWorldLocation;
	}

	stateInterrupt = FALSE;
	return TRUE;
}

void MMOTargetManager::ResetSafeZoneDelayTime()
{
	stateAiInSafeZoneTime = 2.0f;
}

void MMOTargetManager::SetDesirePos(FVector Pos, INT NPCTempleteID)
{
	stateDesirePos = Pos;
	stateDesireNPCTempleteID = NPCTempleteID;
	SetDesireTarget(NULL,NULL);
}

void MMOTargetManager::SetDesireTarget(UMMOAbility* Ability,AActor* Target,UBOOL bLeftMouseBtn)
{
	stateMainPlayer =	MMOFacade::getSingleton().GetMainPlayer();
	stateController	=	MMOFacade::getSingleton().GetMainController();

	if(stateMainPlayer==NULL)return;
	if(stateController==NULL)return;

	stateLastDesireMMOActor = stateDesireMMOActor;

	stateDesireActor	=	Target;		
	stateDesireMMOActor	=	Cast<AMMOActor>(Target);
	//stateDesireMMOPlayer=	Cast<AMMOPlayer>(Target);
	stateDesireNPC=			Cast<ANPC>(Target);
	//stateDesireItem=		Cast<AMMODropActor>(Target);
	stateDesireAttackble	= MMOFacade::getSingleton().IsAttackable(stateMainPlayer,stateDesireMMOActor);
	stateDesireAbility	= Ability;


	//stateAbilityRespondTime = -1.0f;

	if(stateDesireActor)
	{
		if(stateDesireNPC && stateDesireNPC->Template)
		{
			stateDesireNPCTempleteID = stateDesireNPC->Template->GetTemplateID();
		}
		else
		{
			stateDesireNPCTempleteID = 0;
		}
	}

	stateAttackMMOActor	= stateDesireMMOActor;

	SetFocusActor(Target);
	//隐藏鼠标点击光标
	if(stateController->TargetLocationMarker)
	{
		stateController->TargetLocationMarker->Hide();
	}
	//if(stateDesireMMOActor && stateLastDesireMMOActor!=stateDesireMMOActor)
	//{
	//	stateController->SetSelectTarget(stateDesireActor);
	//}

	UpdateState(0.0f);

	stateLeftBtnDown=	bLeftMouseBtn;
	statePressedTime=	0.0f;						//按键累计时间;
	stateFastTime	=	TIME_SYNC_FAST+EPSILON;		//快循环计时;
	stateSlowTime	=	TIME_SYNC_SLOW+EPSILON;		//慢循环计时;


	if(stateDesireMMOActor==NULL && stateLeftBtnDown && stateShiftDown==FALSE && stateLeftBtnDown)
	{
		stateCtrlState	=	CS_MOVE;
	}
	else{
		stateCtrlState	=	CS_BATTLEMOVE;
	}

	if(stateLeftBtnDown)//如果是左键激发
	{
		stateLeftBtnAbility =Ability;		//左键装技能
	}

	stateDesireState=	DS_TRIGGER;

//	warnf(NAME_Log, TEXT("=============================[MMOTargetManager::SetDesireTarget]"));
}

void MMOTargetManager::ClearDesireTarget(UMMOAbility* Ability)
{
	//结束需要结束的技能
	if( Ability && stateController)
	{
		if(Ability->ClientCommonAttribute.bRequestCancelOnRelease)
		{
			stateController->eventAddCmdCancelAbility(Ability);
		}
	}


	stateAbility		=	NULL;
	//stateTargetMMOActor	=	NULL;
	//stateDesireMMOActor	=	NULL;
	stateCtrlState		=	CS_IDLE;
	stateDesireState	=	DS_RELEASE;

}

AActor*	MMOTargetManager::GetDesireActor()
{
	return stateDesireActor;
}

void	MMOTargetManager::SetTargetActor(AActor* TargetActor)
{
	stateLastTargetActor=	stateTargetActor;
	stateLastTargetMMOActor = stateTargetMMOActor;
	stateLastTargetMMOActorID	= stateTargetMMOActorID;

	stateTargetActor	=   TargetActor;
	stateTargetMMOActor	=	Cast<AMMOActor>(stateTargetActor);
	if(stateTargetMMOActor )
	{
		stateTargetMMOActorID = stateTargetMMOActor->ServerID;
	}
	else
	{
		stateTargetMMOActorID = 0;
	}

	stateTargetMMOPlayer=	Cast<AMMOPlayer>(stateTargetActor);	
	stateTargetNPC		=	Cast<ANPC>(stateTargetActor);
	//stateTargetItem		=	Cast<ADropItemActor>(stateTargetActor);
	stateTeleportOrig	=	Cast<ATeleportOrig>(stateTargetActor);	
	stateTeleportNetNode=	Cast<ATeleportNetNode>(stateTargetActor);	
	stateDoodadActor	=   Cast<AMMODoodadActor>(stateTargetActor);
	stateAttackble		=	MMOFacade::getSingleton().IsAttackable(stateMainPlayer,stateTargetMMOActor);
	stateIsDead			=	stateTargetMMOActor?stateTargetMMOActor->eventIsDead():FALSE;

	if(stateTeleportOrig || stateTeleportNetNode){
		stateIsShowTips = TRUE;
	}
}

AMMOActor*	MMOTargetManager::GetTargetMMOActor()
{
	//if (stateTargetMMOActorID>0)
	//{
	//	STLInt2IntMap* pCharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	//	if(pCharacterMap){
	//		STLInt2IntMap::iterator pos = pCharacterMap->find(stateTargetMMOActorID);
	//		if (pos != pCharacterMap->end())
	//		{
	//			return Cast<AMMOPlayer>(UObject::GetIndexedObject( pos->second ));
	//		}
	//	}
	//}
	return NULL;
}

AMMOActor*	MMOTargetManager::GetLastTargetMMOActor()
{
	//if (stateLastTargetMMOActorID>0)
	//{
	//	STLInt2IntMap* pCharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	//	if(pCharacterMap){
	//		STLInt2IntMap::iterator pos = pCharacterMap->find(stateLastTargetMMOActorID);
	//		if (pos != pCharacterMap->end())
	//		{
	//			return Cast<AMMOPlayer>(UObject::GetIndexedObject( pos->second ));
	//		}
	//	}
	//}
	return NULL;
}


AMMOActor*	MMOTargetManager::GetSelectMMOActor()
{
	return stateSelectMMOActor;
}

void	MMOTargetManager::SetSelectMMOActor(AMMOActor* SelectActor)
{
	stateSelectMMOActor = SelectActor;
}


void MMOTargetManager::SetFocusActor(AActor* FocusActor)
{
	stateFocusActor		= FocusActor;		
	stateFocusMMOActor	= Cast<AMMOActor>(stateFocusActor);
	stateFocusMMOPlayer = Cast<AMMOPlayer>(stateFocusActor);
}

AActor*	MMOTargetManager::GetFocusActor()
{
	return stateFocusActor; 
}

void	MMOTargetManager::ClearActor(AActor* Actor)
{
	if(Actor && Actor==stateFocusActor)
	{
		SetFocusActor(NULL);
	}
	if(Actor && Actor==stateTargetActor)
	{
		UpdateState(0.0f);
	}
	if(Actor && Actor==stateDesireActor)
	{
		stateDesireActor	= NULL;
		stateDesireMMOActor = NULL;
		//stateDesireMMOPlayer= NULL;
		stateDesireNPC		= NULL;
		//stateDesireItem		= NULL;
	}
	if(Actor && Actor==stateLastTargetActor)
	{
		stateLastTargetActor	= NULL;
		stateLastTargetMMOActor = NULL;
	}
	if(Actor && Actor==stateLastDesireMMOActor)
	{
		stateLastDesireMMOActor = NULL;
	}
	if(Actor && Actor==stateSelectMMOActor)
	{
		stateSelectMMOActor = NULL;
	}


}



FVector MMOTargetManager::OptimizeTargetPos(FVector TargetPos)
{
	//防止点击点距离角色太近，产生平移可能
	FVector location = stateMainPlayer->Location - FVector(0.0,0.0,stateMainPlayer->CylinderComponent->CollisionHeight);
	FLOAT dis = (TargetPos-location).Size2D();
	FLOAT minDis = stateMainPlayer->CylinderComponent->CollisionRadius*3.0;
	if( dis<minDis )
	{
		TargetPos = TargetPos + (TargetPos-location)*(minDis-dis)/dis ;
	}
	return TargetPos;
}

UMMOAbility*	MMOTargetManager::GetDefaultAbility()
{
	//默认技能
	switch( MMOFacade::getSingleton().GetMainActor()->GetClassId() )
	{
	case EPCT_PlayerClassA://战士
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1000, 0);
		break;
	case EPCT_PlayerClassB://法师
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(2500, 0);
		break;
	case EPCT_PlayerClassC://刺客
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1500, 0);
		break;
	case EPCT_PlayerClassD://弓手
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(2000, 0);
		break;
	case EPCT_PlayerClassE://道士
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(3000, 0);
		break;
	case EPCT_PlayerClassG://Lancer (1.05)
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1200, 0);
		break;
	case EPCT_PlayerClassH://Sorcerer (1.05)
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(3200, 0);
		break;
	default:
		return NULL;
	}
}


UMMOAbility* MMOTargetManager::OptimizeAbility(UMMOAbility* Ability)
{
	if(Ability&&Ability->ClientCommonAttribute.UseDefaultAbility)
	//if(Ability)
	{
		//蓝不够	//copy from server by JimZhang
		if(stateMainPlayer &&
			Ability->CanUseManaCheck(stateMainPlayer)==FALSE)
		//if(MMOFacade::getSingleton().GetMyCurrentMP() < Ability->SpellCommonAttribute.InitialCostValue + Ability->SpellCommonAttribute.InitialCostValuePct*MMOFacade::getSingleton().GetMyMaxMP()/10000 )
		//if(MMOFacade::getSingleton().GetMyCurrentMP()<10)
		{
			return GetDefaultAbility();
		}
	}
	return Ability;
}

INT	 MMOTargetManager::OptimizeTargetID(FRotator Rotator,FVector Location,FLOAT Radius,SEARCH_TYPE SType)
{
	//STLInt2IntMap* MMOActorMap = MMOFacade::getSingleton().GetCharacterMap();
	//FLOAT MaxDistance = Radius;
	//FLOAT MinDistance = 0.0;
	//FLOAT MaxWeightDistance = Radius*10.0f;
	//FLOAT CurDistance = 0;
	//INT   ObjID		  = 0;

	//for (STLInt2IntMap::iterator Itor=MMOActorMap->begin();  Itor!=MMOActorMap->end(); Itor++ )
	//{
	//	AMMOActor* pMMOActor = Cast<AMMOActor>(UObject::GetIndexedObject(Itor->second));
	//	if(pMMOActor==NULL)continue;

	//	if(MMOFacade::getSingleton().IsAttackable( stateMainPlayer ,pMMOActor)==FALSE) continue;

	//	if(SType==ST_INNER_DAMAGEABLE ||SType==ST_CLOSED_DAMAGEABLE)
	//	{
	//		if(pMMOActor->GetClass()->ImplementsInterface( UIAbilityDamageable::StaticClass())==FALSE)
	//		{
	//			continue;
	//		}
	//	}

	//	if(pMMOActor->eventIsDead())
	//	{
	//		continue;
	//	}

	//	FVector ObjVector = pMMOActor->Location-Location;
	//	CurDistance = ObjVector.Size2D();
	//	if(CurDistance<MaxDistance && CurDistance>MinDistance )
	//	{
	//		//计算方向系数-------------------------------------------
	//		ObjVector.Z = 0.0f;
	//		FRotator ObjRotator = ObjVector.Rotation();
	//		INT    Weight		= abs(ObjRotator.Yaw - Rotator.Yaw);
	//		if(Weight*2>MAXWORD)
	//		{
	//			Weight =  MAXWORD-Weight;
	//		}
	//		FLOAT WeightLv = FLOAT(Weight*2000/MAXWORD) * 0.001f;
	//		FLOAT CurWeightDistance = CurDistance*(1.0f+WeightLv*3.0f);
	//		//-------------------------------------------------------------


	//		if(CurWeightDistance<MaxWeightDistance){
	//			MaxDistance = CurDistance; 
	//			MaxWeightDistance = CurWeightDistance;
	//			ObjID		= Itor->second;
	//			if(SType==ST_INNER_ALL ||SType==ST_INNER_DAMAGEABLE ||SType==ST_INNER_FRIENDLY)
	//			{
	//				//break;
	//			}
	//		}
	//	}
	//}

	//return ObjID;
	return 0;
}

FVector MMOTargetManager::OptimizeMoveOffset(BYTE* pMode )
{
	FVector vec = FVector(0.0,0.0,0.0);
	FLOAT	scale = 2.5;
	for(int i=0;i<4;i++)
	{
		switch (*(pMode+i))
		{
		case EMCK_Left://left
			vec.X += 64.0;
			vec.Y += -64.0;
			vec.Z += 0.0;
			break;
		case EMCK_Right://right
			vec.X += -64.0;
			vec.Y += 64.0;
			vec.Z += 0.0;
			break;
		case EMCK_Front://front
			vec.X += 64.0;
			vec.Y += 64.0;
			vec.Z += 0.0;
			break;
		case EMCK_Back://back
			vec.X += -64.0;
			vec.Y += -64.0;
			vec.Z += 0.0;
			break;
		default:
			vec.X += 0.0;
			vec.Y += 0.0;
			vec.Z += 0.0;
		}
	}

	return vec*scale;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//如果是按住shift键，则原地放技能
UBOOL MMOTargetManager::stateShiftAbility()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateShiftDown==TRUE && 
		stateAbility!=NULL  && 
		stateAbilityEnable	)
	{
		//直接施放技能
		stateController->eventClearAllCmd();
		if(stateTargetMMOActor){
			stateController->eventAddCmdUseAbility(stateAbility,stateTargetMMOActor, stateTargetMMOActor->Location);
		}
		else
		{
			stateController->eventAddCmdUseAbility(stateAbility,NULL, stateController->MouseHitWorldLocation);
		}
		stateAbilityRespondTime = 0;
		stateInterrupt = TRUE;
		return TRUE;
	}
	return FALSE;
}

//原地释放技能-------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateStayAndUseAbility()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateAbility!=NULL &&
		stateController &&
		stateMainPlayer	&&
		stateAbilityEnable &&
		stateShiftDown==FALSE &&
		statePickTarget == FALSE &&
		(stateDesireMMOActor==FALSE || stateDesireAttackble)
		)
	{
		warnf(NAME_Warning, TEXT("=============================[MMOTargetManager::stateStayAndUseAbility]"));

		if(stateAbility->SpellCommonAttribute.CanMoveInPrepare)
		{
			INT ServerID = stateTargetMMOActor?stateTargetMMOActor->ServerID:0 ;
			//stateController->RequestUseAbility(ServerID, stateAbility->SlotHotKeyID, stateAbility->SpellCommonAttribute.Level, stateTargetPos );
			stateInterrupt = TRUE;
		}
		else{
			//直接施放技能
			stateController->eventClearAllCmd();
			//stateController->eventAddCmdUseAbility(stateAbility,stateDesireMMOActor,stateTargetPos);
			stateAbilityRespondTime = 0;
			stateInterrupt = TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

//追击并释放技能-------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateMoveToAndUseAbility()
{	
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateAbility!=NULL &&
		stateController &&
		stateMainPlayer	&&
		stateAbilityEnable &&
		stateShiftDown==FALSE  &&
		stateDesireMMOActor && 
		stateDesireAttackble &&
		stateIsDead==FALSE
		)
	{
		stateController->eventClearAllCmd();
		float dis = stateAbility->SpellCommonAttribute.MaxDistance;
		if(stateAbility->SpellCommonAttribute.UseWeaponRange)
		{
			dis = stateMainPlayer->CurrentAttackRange;
		}
		if((stateMainPlayer->Location-stateDesireMMOActor->Location).Size() >= dis)
		{
			if(stateMoveEnable==FALSE)return FALSE;
			//追踪过去，距离一个施法距离时候，结束
			stateController->eventAddCmdMoveToActor(stateDesireMMOActor,stateDesireMMOActor->Location,dis*0.9);
		}
		if(stateAbilityEnable)
		{
			//施放技能
			stateController->eventAddCmdUseAbility(stateAbility,stateDesireMMOActor,stateDesireMMOActor->Location,FALSE);
			stateAbilityRespondTime = 0;
		}

		stateInterrupt = TRUE;
		return TRUE;

	}
	return FALSE;
}

//搜索敌人并释放技能-------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateSearchAndUseAbility()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateAbility!=NULL &&
		stateController &&
		stateMainPlayer	&&
		stateDesireMMOActor==NULL && //初始目标没有  
		stateAbilityEnable &&
		statePickTarget &&
		stateShiftDown==FALSE  )
	{

		if(	stateAttackMMOActor && 
			stateController->GetCmdNum(CMD_ABILITY)==0 &&
			MMOFacade::getSingleton().IsAttackable(stateMainPlayer,stateAttackMMOActor) )
		{
			stateController->eventClearAllCmd();
			if((stateMainPlayer->Location-stateAttackMMOActor->Location).Size() >= stateAbility->SpellCommonAttribute.MaxDistance)
			{
				if(stateMoveEnable==FALSE)return FALSE;
				//追踪过去，距离一个施法距离时候，结束
				stateController->eventAddCmdMoveToActor(stateAttackMMOActor,stateAttackMMOActor->Location,stateAbility->SpellCommonAttribute.MaxDistance*0.9);
			}
			if(stateAbilityEnable)
			{
				//施放技能
				stateController->eventAddCmdUseAbility(stateAbility,stateAttackMMOActor,stateAttackMMOActor->Location,FALSE);
				stateAbilityRespondTime = 0;
			}
			return TRUE;
		}
	}

	return FALSE;
}

//走近并交互----------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateMoveToAndInteract()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState>=CS_MOVE && 
		stateController &&
		stateMainPlayer	&&
		stateShiftDown==FALSE && 
		stateDesireActor !=NULL &&	//要有指定目标
		stateDesireAttackble == FALSE
		) 
	{

		TScriptInterface<IIInteractableActor> interactableActor = ScriptInterfaceCast<IIInteractableActor>(stateDesireActor);
		if( interactableActor || stateDesireNPCTempleteID>0 ){
			//NPC interact
			stateController->eventClearAllCmd();


			FLOAT interactDist = 0.0f;
			if(interactableActor)
			{
				interactDist = interactableActor->GetMaxInteractRadius();

				if((stateMainPlayer->Location - interactableActor->GetInteractActorLocation()).Size() >= interactDist)
				{
					//追踪过去，
					stateController->eventAddCmdMoveToActor(stateDesireNPC, interactableActor->GetInteractActorLocation(), interactDist * 0.8, stateDesireNPCTempleteID);
				}
			}
			else if(stateDesireNPCTempleteID > 0)
			{
				interactDist = stateController->InteractDistance;
				//追踪过去，
				stateController->eventAddCmdMoveToActor(NULL, FVector(0, 0, 0),  interactDist * 0.8, stateDesireNPCTempleteID);
			}

			//进行交互
			//stateController->SetSelectTarget(stateDesireActor);
			stateController->eventAddCmdInteract(interactableActor, 0, stateDesireNPCTempleteID);
			stateCtrlState = CS_IDLE;
			return TRUE;
		}
		
// 		ATeleportOrig* teleportOrig = Cast<ATeleportOrig>( stateDesireActor );
// 		if( teleportOrig != NULL ){
// 			//teleport interact
// 			stateController->eventClearAllCmd();
// 			stateController->eventAddCmdMoveToActor(teleportOrig,teleportOrig->Location, teleportOrig->TeleportClientRange*0.4,0);
// 
// 			//
// 			stateController->eventAddCmdInteract(teleportOrig,0,0);
// 			stateCtrlState=CS_IDLE;
// 			return TRUE;
// 		}
	}
	return FALSE;
}


//走近并摆摊----------------------------------------------------------------------------------------------------------------------------------
// UBOOL MMOTargetManager::stateMoveToAndStall()
// {
// 	if(	stateInterrupt==FALSE &&
// 		stateStallState==FALSE &&
// 		stateMoveEnable &&
// 		stateMoveKeyDown == FALSE &&
// 		stateCtrlState>=CS_MOVE && 
// 		stateController &&
// 		stateMainPlayer	&&
// 		stateShiftDown==FALSE && 
// 		stateDesireMMOPlayer  &&
// 		stateDesireMMOPlayer->MoveType==EMT_InStall)   //摆摊交互
// 	{
// 		stateController->eventClearAllCmd();
// 
// 		if(stateDesireMMOActor){
// 			if((stateMainPlayer->Location-stateDesireMMOActor->Location).Size() >= stateController->InteractDistance)
// 			{
// 				//追踪过去，
// 				stateController->eventAddCmdMoveToActor(stateDesireMMOActor,stateDesirePos, stateController->InteractDistance*0.9,stateDesireNPCTempleteID);
// 			}
// 		}
// 
// 		//进行交互
// 		//stateController->SetSelectTarget(stateDesireActor);
// 		stateController->eventAddCmdInteract(stateDesireMMOActor,0,stateDesireNPCTempleteID);
// 		stateCtrlState=CS_IDLE;
// 		return TRUE;
// 	}
// 	return FALSE;
// }


//走近捡东西
// UBOOL MMOTargetManager::stateMoveToPickItem()
// {
// 	if(	stateInterrupt==FALSE &&
// 		stateStallState==FALSE &&
// 		stateCtrlState>=CS_MOVE && 
// 		stateMoveKeyDown == FALSE &&
// 		stateController &&
// 		stateMainPlayer	&&
// 		stateShiftDown==FALSE && 
// 		stateDesireItem ) 
// 	{
// 		stateController->eventClearAllCmd();
// 
// 		if((stateMainPlayer->Location-stateDesireItem->Location).Size()
// 			>= stateController->MaxPickDistance)
// 		{
// 			//追踪过去，
// 			stateController->eventAddCmdMoveToActor(
// 				stateDesireItem, stateDesireItem->Location,
// 				stateController->MaxPickDistance * 0.7f);
// 		}
// 		//进行拾取
// 		stateController->eventAddCmdPickUpItem(stateDesireItem);
// 		stateCtrlState=CS_IDLE;
// 		return TRUE;
// 	}
// 	return FALSE;
// }


//走到目标点stateTargetPos, 这个是根据暗黑3模式实时寻出的一个鼠标转换点，增加鼠标操作的灵活性，概括为鼠标操作下的移动
UBOOL MMOTargetManager::stateMoveToPoint()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		//(stateTargetMMOActor==NULL || stateIsDead) &&
		(statePickTarget || stateLeftBtnDown) &&
		//(stateAbility!=NULL ||(stateAbility==NULL&&stateLeftBtnDown))	&&
		stateController &&
		stateMainPlayer	&&
		stateShiftDown==FALSE && 
		stateAttackMMOActor == NULL &&	//无可攻击
		//stateController->GetCmdNum(CMD_ABILITY)==0 && //没有技能命令
		stateCtrlState>=CS_MOVE	)
	{
		if(statePathOptimize)//根据目标点的寻路数据是否直线，进行快速刷新
		{
			if(stateFastTime > TIME_SYNC_FAST )
			{
				stateController->eventClearAllCmd();
				stateController->SetTargetLocationMarker(stateTargetPos,stateDesireMMOActor);
				stateController->eventAddCmdMoveToPoint( stateTargetPos,TRUE );
			}
			else //只进行转向
			{
				if(stateMoveEnable){//在可移动的情况下才进行实时转向
					//实时更新朝向
					FVector Value;
					FVector Des =  stateTargetPos;
					if(stateMainPlayer->eventGetValidPoint(Des, Value))
					{
						Des = Value;
						Des.Z += stateMainPlayer->GetCollisionHeight();
					}

					Value = Des - stateMainPlayer->Location;
					Value = Value.SafeNormal2D();

					FRotator Rot = Value.Rotation();
					stateMainPlayer->LockDesiredRotation(false, true);
					stateMainPlayer->SetDesiredRotation(Rot, false, true);
				}
			}
		}
		else
		{
			if(stateSlowTime > TIME_SYNC_SLOW )
			{
				stateController->eventClearAllCmd();
				stateController->SetTargetLocationMarker(stateTargetPos,stateDesireMMOActor);
				stateController->eventAddCmdMoveToPoint( stateTargetPos,TRUE );
			}
		}

		//if(stateCtrlState==CS_BATTLEMOVE && stateLeftBtnDown==TRUE)
		//{
		//	//暗黑三的特殊方法，左键点击无目标，自动切到位移状态
		//	stateCtrlState=CS_MOVE;
		//}

		return TRUE;
	}
	return FALSE;
}

//走到指定坐标点stateDesirePos，这是初始设定的一个目标点，与鼠标无关，概括为预设定目标点下的移动
UBOOL MMOTargetManager::stateMoveToDesirePoint()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateDesireActor==NULL &&
		stateDesireAbility==NULL &&
		stateController &&
		stateDesireNPCTempleteID==0 &&
		stateCtrlState>=CS_MOVE	)
	{
		stateController->eventClearAllCmd();
		stateController->eventAddCmdMoveToPoint( stateDesirePos,TRUE );
		stateCtrlState=CS_IDLE;
		return TRUE;
	}
	return FALSE;
}

//键盘控制移动
UBOOL MMOTargetManager::stateMoveByKey()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMoveEnable &&
		stateMoveKeyDown &&
		stateController &&
		stateMainPlayer	&&
		stateShiftDown==FALSE && 
		stateFastTime > TIME_SYNC_FAST &&
		stateShiftDown==FALSE  )
	{
		stateController->eventClearAllCmd();
		//走过去
		FVector Pos = stateMainPlayer->Location +OptimizeMoveOffset(MMOUI::GetUIManager()->WSADKey);
		stateController->eventAddCmdMoveToPoint(Pos,TRUE);
	}
	return FALSE;
}

//选中显示第三方头像
UBOOL MMOTargetManager::stateShowInfo()
{
	//当前目标
	if(	stateLastTargetMMOActor!=stateTargetMMOActor )
	{
		MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_CurrentTarget, stateTargetMMOActor, stateSelectMMOActor);
		MMOUI::PostGameEvent(&evt);
	}

	//选择目标
	if( stateDesireMMOActor!=stateLastDesireMMOActor )
	{
		//if(stateDesireAttackble){
		//	MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, NULL);
		//	MMOUI::PostGameEvent(&evt);
		//}
		//else
		{
			MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, stateSelectMMOActor);
			MMOUI::PostGameEvent(&evt);
		}
	}
	return FALSE;
}

//选中物体tips显示
UBOOL MMOTargetManager::stateShowTips()
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		//stateLastTargetActor = NULL;
		//stateTargetActor	 = NULL;
		return FALSE;
	}

	//当前目标
	if(	stateLastTargetActor != stateTargetActor ){

		if(stateTeleportOrig)
		{
			//MMOUI::GetUIManager()->delegateShowFreeStyleTips(  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeleportOrig.Name") ), stateTeleportOrig->ToSceneName );
			MMOUI::GetUIManager()->delegateShowFreeStyleTips( TEXT(""), stateTeleportOrig->ToSceneName, CM_TIPS_Style2, 1 );
			//显示特效
			stateTeleportOrig->ActiveFoucsEffect( TRUE );
			stateIsShowTips = TRUE;
		}
		//else if(stateTeleportNetNode )
		//{
		//	MMOUI::GetUIManager()->delegateShowFreeStyleTips( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeleportNet.Name") ), stateTeleportNetNode->DisplayName );
		//	stateIsShowTips = TRUE;
		//}
        else if (stateDoodadActor)
        {
			const UDoodad* doodadTemp = stateDoodadActor->DoodadTemplate;
			if(doodadTemp && doodadTemp->DoododNameDisplay)
			{
				FString doodadTips;
				//=  MMOUI::TrimNumber(doodadTemp->DoodadName.ToString());
								
				// 提示道具需求
				if(doodadTemp->RequireItemID > 0)
				{
					//doodadTips += TEXT("<br>");
					FString locReqItem;
					
					if(doodadTemp->CostItem)
					{
						locReqItem = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireItem"));
					}
					else
					{
						locReqItem = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.CostItem"));
					}

					locReqItem = locReqItem.TrimQuotes();
					doodadTips += locReqItem + TEXT("<br>");

					UItem* item = MMOFacade::getSingleton().FindItemTemplate(doodadTemp->RequireItemID);
					if(item)
					{
						doodadTips += UITextParser::HTMLFormatItemColorString(item);
					}
					else
					{
						doodadTips += FString::Printf(TEXT("Item:%d"), doodadTemp->RequireItemID);
					}
				}

				// 有开启限制，不管能否开启，也就是VarValue不管是否正确时，都要显示到TIPS
				if(stateDoodadActor->IsVarRequired() && !doodadTemp->TipsText.IsEmpty())
				{
					doodadTips += MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireVar"));
					doodadTips += TEXT("<br>");
					doodadTips += doodadTemp->TipsText;
				}

				//MMOUI::GetUIManager()->delegateShowFreeStyleTips( MMOUI::TrimNumber(doodadTemp->DoodadName.ToString()), doodadTips );
				MMOUI::GetUIManager()->delegateShowFreeStyleTips( TEXT(""), MMOUI::TrimNumber(doodadTemp->DoodadName.ToString()), CM_TIPS_Style3, 1 );

				stateIsShowTips = TRUE;
			}
        }
        else if (stateIsShowTips)
        {
            MMOUI::GetUIManager()->delegateHideTips();

			//处理调转点特效关闭
			ATeleportOrig *lastTeleportOrig	= Cast<ATeleportOrig>(stateLastTargetActor);
			if( lastTeleportOrig != NULL ){
				lastTeleportOrig->ActiveFoucsEffect( FALSE );
			}

            stateIsShowTips = FALSE;
        }
	}

	return FALSE;
}

//选中物体脚底光标显示
UBOOL	MMOTargetManager::stateShowSelectMark()
{
	if(	stateController &&
		stateMainPlayer	&&
		stateSelectMMOActor!=stateDesireMMOActor &&
		stateLastDesireMMOActor!=stateDesireMMOActor &&
		stateSelectMMOActor!= stateDesireMMOActor )
	{
		//if(stateDesireMMOActor && stateSelectMMOActor!= stateDesireMMOActor){
			stateSelectMMOActor = stateDesireMMOActor;
			stateController->SetSelectTarget(stateSelectMMOActor);
			MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, stateSelectMMOActor);
			MMOUI::PostGameEvent(&evt);
		//}
	}


	if( stateSelectMMOActor &&
		stateMainPlayer	&&
		stateController &&
		(stateSelectMMOActor->Location-stateMainPlayer->Location).Size2D() > 1000 )
	{
		stateSelectMMOActor = NULL;
		stateDesireMMOActor = NULL;
		stateController->SetSelectTarget(stateSelectMMOActor);
		MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, stateSelectMMOActor);
		MMOUI::PostGameEvent(&evt);
	}

	return FALSE;
}


//自动寻怪.(战斗位移下，杀死一个怪，然后自动寻找附近攻击权重最大的怪)
UBOOL	MMOTargetManager::stateSearchEnemy()
{
	//死亡取消敌目标
	if( stateAttackMMOActor && (stateAttackMMOActor->eventIsDead() || stateAttackMMOActor->IsPendingKill()))
	{
		stateAttackMMOActor = NULL;
	}

	//超出范围取消敌目标
	if( stateAttackMMOActor &&
		stateAttackMMOActor!=stateDesireMMOActor &&
		stateMainPlayer &&
		stateAbility &&
		(stateMainPlayer->Location - stateAttackMMOActor->Location).Size2D() > stateAbility->SpellCommonAttribute.SearchDistance )
	{
		stateAttackMMOActor = NULL;
	}

	//战斗位移下才会自动寻怪
	if(	stateAttackMMOActor==NULL &&
		stateDesireMMOActor==NULL &&
		stateMainPlayer &&
		stateAbility &&
		stateCtrlState==CS_BATTLEMOVE)
	{

		//搜索
		INT AttackMMOActorIndex = OptimizeTargetID(stateMainPlayer->Rotation,stateMainPlayer->Location,stateAbility->SpellCommonAttribute.SearchDistance,ST_CLOSED_DAMAGEABLE);
		warnf(NAME_Warning, TEXT("=============================[MMOTargetManager::stateSearchEnemy]"));

		if(AttackMMOActorIndex>0){

			stateAttackMMOActor	= NULL;//Cast<AMMOActor>(UObject::GetIndexedObject(AttackMMOActorIndex));
		}
	}

	return FALSE;
}

//自动切换键状态
UBOOL	MMOTargetManager::stateAIContral()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateCtrlState==CS_IDLE &&
		stateMoveEnable &&
		stateLeftBtnAbility &&
		stateLeftBtnAbility!=stateDesireAbility &&
		stateLeftBtnDown )
	{

		warnf( NAME_Error,  TEXT("stateAIContral!") );
		SetDesireTarget(stateLeftBtnAbility,stateTargetActor,TRUE);
		return TRUE;
	}
	return FALSE;
}


//自动检测在安全区内收刀
UBOOL	MMOTargetManager::stateAIInSafeZone()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMainPlayer )
		//&& stateMainPlayer->IsInSafeZone &&
		//stateMainPlayer->ActorAttributes->byCombatPoseOn==FALSE )
	{
		//if(stateAbility && stateCtrlState==CS_BATTLEMOVE){
		//	ResetSafeZoneDelayTime();
		//}
		//else if(stateCtrlState<=CS_MOVE)
		//{

		//	if(	stateMainPlayer->IsInSafeZone && stateMainPlayer->ActorAttributes->byCombatPoseOn==FALSE )
		//	{
		//		if(stateAiInSafeZoneTime<=0.0f){
		//			warnf( NAME_Error,  TEXT("stateAIInSafeZone 1!") );
		//			ResetSafeZoneDelayTime();
		//			
		//			if(MMO::GetCurrentGameState() == EGameState_InGame){
		//				CMessageHandler::GetInstance().Send_CombatPoseOn(TRUE,1);
		//			}
		//			return TRUE;
		//		}
		//	}
		//	else if(stateMainPlayer->IsInSafeZone==FALSE && stateMainPlayer->ActorAttributes->byCombatPoseOn)
		//	{
		//		//不在移动中，则10秒检测一次
		//		if(stateAiInSafeZoneTime<=0.0f){
		//			warnf( NAME_Error,  TEXT("stateAIInSafeZone 2!") );
		//			ResetSafeZoneDelayTime();

		//			if(MMO::GetCurrentGameState() == EGameState_InGame){
		//				CMessageHandler::GetInstance().Send_CombatPoseOn(FALSE,0);
		//			}

		//			return TRUE;
		//		}
		//	}

		//}
	}

	return FALSE;
}



//自动是否要释放解qte技能
UBOOL	MMOTargetManager::stateAIBossQTE()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateCtrlState==CS_IDLE &&
		stateMainPlayer &&
		stateController &&
		TimeQTE < 0.0f )
	{
		UMMOUIBase* pQTEPanel = MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_HintCombat_BossQTE);
		if(pQTEPanel && pQTEPanel->IsVisible)
		{
			if( (LastMouseHitNavMeshLocation - stateController->MouseHitNavMeshLocation).Size2D() > 256 )
			{
				//CMessageHandler::GetInstance().Send_RequestUseAbility(stateMainPlayer->ServerID ,4010,stateMainPlayer->ServerID,stateMainPlayer->Location);
				//CMessageHandler::GetInstance().Send_RequestUseAbility(stateMainPlayer->ServerID ,qssBossQTE,stateMainPlayer->ServerID,stateMainPlayer->Location);
				
				TimeQTE = 0.3f;
			}
		}
		return TRUE;
	}
	return FALSE;
}


//摆摊的点击模型弹出UI显示--------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateClickStallState()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==TRUE &&
		stateController &&
		stateMainPlayer	&&
		stateDesireMMOActor &&
		stateLeftBtnDown==TRUE && 
		stateDesireMMOActor->IsMainPlayer()	//本身是目标
		) 
	{
			if(stateMainPlayer->MoveType == EMT_InStall)
			{
				UMMOUIBase *stallSettingPanel = MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Stall_StallSetting);
				if(stallSettingPanel==NULL || !stallSettingPanel->IsVisible)
				{
					MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting,TRUE,"");
				}

				//UMMOUIBase *stallSettingPanel = MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting);
				//if(stallSettingPanel && !stallSettingPanel->IsVisible)
				//{
				//	stallSettingPanel->Show(TRUE);
				//	return TRUE;
				//}
			}
			
	}
	return FALSE;
}

//特殊的定时提示，比如主武器的耐久度不等间隔警示--------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateStackWarningState()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateController &&
		stateMainPlayer &&
		stateMainPlayer->ItemManager &&
		stateWarningTime>=10.0f
		) 
	{

		//游戏中进行检测
		if(MMO::GetCurrentGameState() == EGameState_InGame)
		{
			//for(int i=EET_Normal_MainHand;i<EET_Total_Count;i++)
			//{
			//	FClientItemInfo* item = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED , EEquipType(i) );
			//	if(item )
			//	{
			//		UItem* pEquip = MMOFacade::getSingleton().FindItemTemplate(item->m_nTemplateID);
			//		if(pEquip){
			//			//小于5%
			//			if(item->m_nCurDur*20<=item->m_nMaxDur)
			//			{
			//				stateWarningTime = 0.0f;
			//				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemSubTypes.") ) + pEquip->ItemSubType )
			//																+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Equip.DurWarning1")) );
			//				//return TRUE;
			//			}
			//			else if(item->m_nCurDur*5<=item->m_nMaxDur)
			//			{
			//				if(stateWarningTime>=30.0f)
			//				{
			//					stateWarningTime = 0.0f;
			//					MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemSubTypes.") ) + pEquip->ItemSubType )
			//						+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Equip.DurWarning3")) );
			//					//return TRUE;
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}
	return FALSE;
}


//智能清除分解，鉴定，修理等状态，--------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateReleaseOperation()
{
	if(	stateInterrupt==FALSE &&
		stateController &&
		stateMainPlayer 
		) 
	{
		if(EGameState_InGame == MMO::GetCurrentGameState() && MMOUI::GetUIManager()->IsItemSlotActionListener() )
		{
			MMOUI::GetUIManager()->ClearCurrentItemSlotActionListenerUpdate();
		}
	}
	return FALSE;
}

//gm 组合测试，自动成为战神------------------------------------------------------------------------------------------------------------------------------------
UBOOL	MMOTargetManager::stateUpdateAres()
{
	if(	AresFlag>0 && 	
		stateMainPlayer &&
		stateController &&
		AresDeltaTime <= 0.0 )
	{
		//基础的提升
		if(AresFlag==1)
		{
			AresIndex = 0;

		}
		//修炼一把武器
		else if(AresFlag==2)
		{
			stateController->NativeDeleteBagItem(0,64);
		}
		else if(AresFlag==3)
		{
			if(AresIndex==0){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11210251,1) end") );//长剑
			}
			else if(AresIndex==1){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211151,1) end") );//头盔
			}
			else if(AresIndex==2){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211251,1) end") );//肩胛
			}
			else if(AresIndex==3){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211351,1) end") );//铠甲
			}
			else if(AresIndex==4){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211451,1) end") );//战腿
			}
			else if(AresIndex==5){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211551,1) end") );//护腰
			}
			else if(AresIndex==6){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211651,1) end") );//护腕
			}
			else if(AresIndex==7){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211751,1) end") );//战靴
			}
			else if(AresIndex==8){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211851,1) end") );//披风
			}
			else if(AresIndex==9){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11252154,1) end") );//手镯
			}
			else if(AresIndex==10){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11252354,1) end") );//指环
			}
			else if(AresIndex==11){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11252254,1) end") );//项链
			}
			else
			{
				//结束
				AresFlag = 0;
				return FALSE;
			}
		}
		else if(AresFlag==4)
		{
			for(INT i=0;i<64;i++){			
				FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, i);
				if(info && info->m_byIdentified==IDENTIFY_TYPE_NO)
				{
					CMessageHandler::GetInstance().Send_IdentifyItem( E_CONSLOT_DEFAULT, i );
				}
			}
		}
		else if(AresFlag==5)
		{
			INT MaxPotential = 0;
			INT MaxPow = 0;
			INT MaxSlot = 0;
			FClientItemInfo* MaxInfo = NULL;
			for(INT i=0;i<64;i++){			
				FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, i);
				if(info)
				{
					INT CurPotential = info->GetPotential(0);
					if(CurPotential>MaxPotential)
					{
						MaxPow	= info->Pow;
						MaxPotential = CurPotential;
						MaxSlot = i;
						MaxInfo = info;
					}
					else if(CurPotential==MaxPotential)
					{
						if(info->Pow>MaxPow)
						{
							MaxPow  =info->Pow;
							MaxSlot	= i;
							MaxInfo = info;
						}
					}
				}
			}

			if(MaxInfo)
			{
				INT EmptySlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_BANK);
				if(EmptySlot>=0){
					CMessageHandler::GetInstance().Send_MoveItem(E_CONSLOT_BANK, EmptySlot, E_CONSLOT_DEFAULT, MaxSlot);
				}
			}
		}
		else if(AresFlag==6)
		{
			CMessageHandler::GetInstance().Send_SortBag(EBLT_NormalItem);
		}
		else if(AresFlag==8)
		{
			//CMessageHandler::GetInstance().Send_SortBag(E_CONSLOT_BANK);
			AresIndex++;
			//跳到第二步，重复操作
			AresFlag = 2;
			return FALSE;
		}
		AresFlag++;
		AresDeltaTime=1.0;
	}
	return FALSE;
}
