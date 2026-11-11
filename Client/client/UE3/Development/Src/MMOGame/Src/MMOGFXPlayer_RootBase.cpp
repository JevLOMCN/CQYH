#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_RootBase)

extern bool g_displayDragDropIcon;
extern void MMOUIWingPanel_Tick();	// MMOUIWingPanel.cpp

extern TArray<struct FGFxWidgetBinding> g_WidgetBindings;

void UMMOGFXPlayer_RootBase::PostInit()
{
	// 	for(INT i = 0;i< UIObjectList.Num(); ++i)
	// 	{
	// 		if(UIObjectList(i) != NULL)
	// 		{
	// 			UIObjectList(i)->Init();
	// 			UIObjectList(i)->eventInitOnce();		
	// 			//UIObjectList[i].InitWidget();
	// 		}
	// 	}
}

void UMMOGFXPlayer_RootBase::BindingWidgetsInfo()
{
	// binding
	for(INT i = 0; i < ViewData.Num(); ++i)
	{
		const FGFXUIViewInfo& InViewInfo = ViewData(i);
		SET_WARN_COLOR(COLOR_BLUE);
		warnf(NAME_DevGFxUI, TEXT("@@@@ LoadView Binding WidgetClass:%s    ViewName:%s    swf:%s"),
			*InViewInfo.WidgetClass.ToString(),
			*InViewInfo.ViewName.ToString(), *InViewInfo.SWFName);
		CLEAR_WARN_COLOR();

		UClass* uiClass = FindObject<UClass>(ANY_PACKAGE, *InViewInfo.WidgetClass.ToString(), FALSE);
		check(uiClass || TEXT("UI class not found!"));
		FGFxWidgetBinding uiCallbackBind;
		uiCallbackBind.WidgetClass = uiClass;
		uiCallbackBind.WidgetName = InViewInfo.ViewName;
		g_WidgetBindings.AddItem(uiCallbackBind);
	}
}

void UMMOGFXPlayer_RootBase::Init()
{
	Super::Init();

	RootMC = GetVariableObject(TEXT("_root"));
	ManagerMC = RootMC->GetObject(TEXT("rootContainer"));

	// TODOUI: 检查这个机制是否可以去掉
	MMOUI::RegisterMoviePlayer(this, FName(*GetClass()->GetName()));


	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MatineeSubtitle);
	evts.AddItem(GameEvent_LeaveGame);
	EventAdapter = new TGameEventAdapter<UMMOGFXPlayer_RootBase>(this, evts);

}

void UMMOGFXPlayer_RootBase::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_RootBase::PreLoadPanels()
{	
	if(GIsEditor)
	{
		return;
	}

	for(INT i = 0; i < ViewData.Num(); ++i)
	{
		if(ViewData(i).PreLoad && !IsViewLoaded(ViewData(i).ViewName))
		{
			SET_WARN_COLOR(COLOR_CYAN);
			warnf(NAME_Log, TEXT("Preloading ui:%s  %s"), *ViewData(i).ViewName.ToString(), *ViewData(i).SWFName);
			CLEAR_WARN_COLOR();
			LoadView(ViewData(i));
		}
	}
}

UBOOL UMMOGFXPlayer_RootBase::LoadView(FGFXUIViewInfo InViewInfo)
{
	// 	FASValue asval;
	// 	TArray<FASValue> args;
	// 	UGFxObject* ViewContainer = 0;
	// 	UGFxObject* ViewLoader = 0;

	// 	ViewContainer = ManagerMC->CreateEmptyMovieClip( InViewInfo.ViewName.ToString() + TEXT("Container"));
	// 	ViewLoader = ViewContainer->CreateEmptyMovieClip( InViewInfo.ViewName.ToString() );

	// 	asval.Type = AS_String;
	// 	asval.S = InViewInfo.SWFName;
	// 	args.AddItem(asval);
	// 
	// 	ViewContainer->SetVisible( FALSE );
	//ManagerMC->Invoke( TEXT("loadMovie"), args );

	// modified by zhangrui05
	// 这里避免含有多个Panel的swf被重复加载
	UBOOL swfLoaded = FALSE;
	for(INT i = 0; i < LoadedMovies.Num(); i++ )
	{
		if (LoadedMovies(i).ToString() == InViewInfo.SWFName)
		{
			swfLoaded = TRUE;
			break;
		}
	}
	if (!swfLoaded)
	{
		eventCallLoadMovie(InViewInfo.SWFName);
		LoadedMovies.AddItem(FName(*(InViewInfo.SWFName)));
	}

	LoadedViews.AddItem( InViewInfo.ViewName );

	Advance(0.1f); // 需要这个去让UI显示更新下

	return TRUE;
}

UBOOL UMMOGFXPlayer_RootBase::LoadViewByName(FName InViewName)
{
	for(INT i = 0; i < ViewData.Num(); i++ )
	{
		// Find the view data by the view name and check that it is not already loaded.
		if ( ViewData(i).ViewName == InViewName && !IsViewLoaded(InViewName) )
		{
			// Load the view.
			return LoadView( ViewData(i) );             
		}
	}

	return FALSE;
}

UBOOL UMMOGFXPlayer_RootBase::IsViewLoaded(FName InViewName)
{
	for(INT i = 0; i < LoadedViews.Num(); i++ )
	{
		// Check if the view has already been loaded using the view name.
		if ( LoadedViews(i) == InViewName )
		{
			return TRUE;
		}
	}
	return FALSE;
}

INT UMMOGFXPlayer_RootBase::GetPanelPriority(FName viewName)
{
	for(INT i = 0; i < ViewData.Num(); i++ )
	{
		// Check if the view has already been loaded using the view name.
		if ( ViewData(i).ViewName == viewName )
		{
			return ViewData(i).Priority;
		}
	}
	return 2;
}

FString UMMOGFXPlayer_RootBase::GetPanelPriorityAndSWF(FName viewName, INT& inPriority)
{
	for(INT i = 0; i < ViewData.Num(); i++ )
	{
		if ( ViewData(i).ViewName == viewName )
		{
			inPriority = ViewData(i).Priority;
			return ViewData(i).SWFName;
		}
	}

	inPriority = 2;
	return FString("");
}

UBOOL UMMOGFXPlayer_RootBase::WidgetInitialized(FName WidgetName,FName WidgetPath,class UGFxObject* Widget)
{
	warnf(NAME_DevGFxUI, TEXT("@@@@ WidgetInitialized WidgetName:%s    Path:%s"), *WidgetName.ToString(), *WidgetPath.ToString());

	for(INT i = 0; i < ViewData.Num(); i++ )
	{
		// Check if the view has already been loaded using the view name.
		if ( ViewData(i).ViewName == WidgetName )
		{
			SetWidgetPathBinding(Widget, WidgetPath);

			UMMOUIBase* uiBase = Cast<UMMOUIBase>(Widget);
			check(uiBase);
			uiBase->FriendlyName = WidgetName;
			UIObjectList.AddItem(uiBase);
			uiBase->eventInitLoad(this);

			MMOUI::SetPanelToMoviePlayer(WidgetName, FName(*GetClass()->GetName()));

			MMOUI::RegisterUIPanel(uiBase, uiBase->FriendlyName); // TODO:移到更好的位置
			uiBase->Init();
			uiBase->eventInitOnce();
			return TRUE;
		}
	}

	// 1.05 swfs contain extra dialog clips (TeamSet, DaoYuSetting, PrivatePolicy, ItemSource, GuildBonus, ...)
	// that have no ViewData entry; hide them so they only exist when explicitly opened.
	if (Widget)
	{
		Widget->SetVisible(FALSE);
	}
	return FALSE;
}

UBOOL UMMOGFXPlayer_RootBase::WidgetUnloaded(FName WidgetName,FName WidgetPath,class UGFxObject* Widget)
{	
	warnf(NAME_DevGFxUI, TEXT("@@@@ --- WidgetUnloaded WidgetName:%s    Path:%s"), *WidgetName.ToString(), *WidgetPath.ToString());

	for(INT i = 0; i < ViewData.Num(); i++ )
	{
		// Check if the view has already been loaded using the view name.
		if ( ViewData(i).ViewName == WidgetName )
		{
			UMMOUIBase* uiBase = Cast<UMMOUIBase>(Widget);
			check(uiBase);
			UIObjectList.RemoveItem(uiBase);
			//uiBase->eventUnLoad();
			return TRUE;
		}
	}

	return FALSE;
}

void UMMOGFXPlayer_RootBase::PostAdvance(float DeltaTime)
{
	Super::PostAdvance(DeltaTime);
	MMOUIWingPanel_Tick();
	if(g_displayDragDropIcon)
	{
		FIntPoint pt = MMOFacade::GetMouseCursorPosition();
		UGFxObject* dropIcon = GetDragIcon();
		dropIcon->SetPosition(pt.X /*- w*/, pt.Y /*- h*/);
	}

// 	if (NULL != _Tips && _Tips->IsVisible)
// 		_Tips->PostAdvance(DeltaTime);
// 
// 	if(_BossBlood)
// 		_BossBlood->TickForDistance(DeltaTime);
}


void UMMOGFXPlayer_RootBase::OnNativeGameEvent (struct MMOGameEvent* evt)
{
	FVector2D scrSz = MMOFacade::GetScreenSize();
	INT scrW = (INT)scrSz.X;
	INT scrH = (INT)scrSz.Y;

	switch(evt->GetEventType())
	{
	case GameEvent_MatineeSubtitle:
		{
			MMOGameEvent_MatineeSubtitle* e = (MMOGameEvent_MatineeSubtitle*)evt;
			switch(e->SubtitleEventType)
			{
			case MMOGameEvent_MatineeSubtitle::EMSET_Show:
				{
					eventCallUCShow(e->ShowSubtitle ? TRUE : FALSE, scrW, scrH);
					debugf(TEXT("************** Show Matinee Subtitle:%d"), (INT)e->ShowSubtitle);
				}
				break;

			case MMOGameEvent_MatineeSubtitle::EMSET_UpdateSubtitle:
				{
					eventCallUCUpdateSubtitle(e->Subtitle);
					debugf(TEXT("************** Update Matinee Subtitle:%s"), *e->Subtitle);
				}
				break;
			}
		}
		break;

	case GameEvent_LeaveGame:
		{
			eventCallUCShow(FALSE, scrW, scrH);
			debugf(TEXT("************** Update Matinee Subtitle:GameEvent_LeaveGame"));
		}
		break;
	}
}
