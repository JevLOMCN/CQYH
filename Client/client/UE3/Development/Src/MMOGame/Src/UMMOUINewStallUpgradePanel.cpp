#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
//#include "StallManager.h"
#include "UStallManager.h"
#include "ClientActionConditionCheck.h"
#include "MMOMsgBoxManager.h"
#include "UStallManager.h"

IMPLEMENT_CLASS(UMMOUINewStallUpgradePanel)

void UMMOUINewStallUpgradePanel::Init()
{
	Super::Init();

	check( EventAdapter == NULL );
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Stall);
	EventAdapter = new TGameEventAdapter<UMMOUINewStallUpgradePanel>(this, evts);
}

void UMMOUINewStallUpgradePanel::UnInit()
{
	Super::UnInit();
}

void UMMOUINewStallUpgradePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		_CurTimeType = ESUT_DAY3;
		UpdateTimeList();
		UpdateCost(_CurTimeType);
	}
}

void UMMOUINewStallUpgradePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (evt && evt->GetEventType() == EGameEvent::GameEvent_Stall)
	{
		MMOGameEvent_StallEvent* stallEvent = (MMOGameEvent_StallEvent*)evt;
		if (stallEvent)
		{
		}
	}
}

void UMMOUINewStallUpgradePanel::UpdateHintText()
{
	FString text = TEXT("");
	eventCallUpdateHintText(text);
}

void UMMOUINewStallUpgradePanel::UpdateTimeList()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	for (INT i = 0; i < ESUT_MAX; ++ i)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("index"), index);

			FString day = TEXT("");
			switch (i)
			{
			case ESUT_DAY3:
				day = TEXT("3 Days");
				break;
			case ESUT_DAY7:
				day = TEXT("7 Days");
				break;
			case ESUT_DAY15:
				day = TEXT("15 Days");
				break;
			}
			
			object->SetString(TEXT("label"), day);

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateTimeList(objectArray);
}

void UMMOUINewStallUpgradePanel::UpdateCost(int timeType)
{
	switch (timeType)
	{
	case ESUT_DAY3:
		eventCallUpdateCost(UpgradeCost1);
		break;
	case ESUT_DAY7:
		eventCallUpdateCost(UpgradeCost2);
		break;
	case ESUT_DAY15:
		eventCallUpdateCost(UpgradeCost3);
		break;
	}
}

void UMMOUINewStallUpgradePanel::ASOnTimeListChangeSelect(int timeType)
{
	_CurTimeType = timeType;
	UpdateCost(timeType);
}

void UMMOUINewStallUpgradePanel::ASOnConfirmStallUpdate()
{
	CMessageHandler::GetInstance().Send_UpgradeStall(_CurTimeType);
	Show(FALSE);
}

