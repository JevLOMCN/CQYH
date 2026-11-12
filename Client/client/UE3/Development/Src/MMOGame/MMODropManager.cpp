#include "stdafx.h"
#include "MMOFacade.h"
#include "MMODropManager.h"
#include "MMOStats.h"
#include "FSMIO.h"
#include "UITextParser.h"
#include "MMOTemplateObjectFinder.h"
#include "MMOGuideManager.h"
#include "MMOQuestManager.h"
#include "MMOAssistManager.h"

extern INT g_moveModGridSize;

MMODropManager::MMODropManager()
	: _pickupAll(false)
{
	MMOUI::RegisterEventHandler(this, GameEvent_LeaveGame);
	MMOUI::RegisterEventHandler(this, GameEvent_LeaveServerScene);
	AMMOHud::RegisterCanvasRenderable(this);
	_alphaMoving=FALSE;
	_alphaHoldFrame=0;
	_bForceRefresh=TRUE;
}

extern FLOAT g_SpawnDelay;

class AMMODropActor* MMODropManager::AddDropActor( bool isNew, const FVector& spawnLocation, const FVector& destLocation, INT itemTemplateID, INT itemCount, INT dropActorID, bool enableCollision, INT spawnOrder )
{
	if(FindDropActor(dropActorID) != NULL)
	{
		warnf(NAME_Error, TEXT("Duplicate drop id found:%d"), dropActorID);
		DestroyDropActor(dropActorID, TRUE);
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemTemplateID);
	if(!item)
	{
		warnf(NAME_Error, TEXT("[MMODropManager::AddDropActor]Item not find [%d]"), itemTemplateID);
		return NULL;
	}

	class AMMODropActor* obj = NULL;
	FName debugName;

#if 0 // oscar@snda names that may be duplicated cannot be specified like this
#if !SHIPPING_PC_GAME
	debugName = FName(*FString::Printf(TEXT("DropActor.ItemTemplate[%d]_ID[%d]"),
		itemTemplateID, dropActorID));
#else
	debugName = NAME_None;
#endif
#endif

	debugName = NAME_None;

	FVector dropLocation = destLocation;
	FVector startPoint = dropLocation + FVector(0, 0, 512); // 16CM error distance is enough
	FVector endPoint = dropLocation - FVector(0, 0, 512);
	FCheckResult hit(1.f);
	if (GWorld->SingleLineCheck(hit, NULL, endPoint, startPoint, TRACE_World/*TRACE_Blocking|TRACE_Terrain|TRACE_Level|TRACE_Volumes|TRACE_LevelGeometry*/) == FALSE)
	{
		//Target->DrawDebugSphere(hit.Location, 10, 16, 0, 255, 0, TRUE);
		//Target->DrawDebugSphere(dropLocation, 15, 5, 0, 255, 255, TRUE);
		dropLocation = hit.Location;
	}

	//FVector temp = MMOFacade::getSingleton().GetMainActor()->Location;

#if 0
	
	SET_WARN_COLOR(COLOR_PURPLE);
	warnf(NAME_Log, TEXT("AddDropActor ID:%d [%s] Client:%s"), dropActorID,
		*item->ItemName,
		*spawnLocation.ToString());
	CLEAR_WARN_COLOR();
#endif

	obj = Cast<AMMODropActor>(GWorld->SpawnActor(AMMODropActor::StaticClass(), debugName, spawnLocation/*FVector(dropLocation.X, dropLocation.Y, dropLocation.Z + 5.f)*/, FRotator(0, 0, 0), NULL, TRUE));
	if(obj)
	{
		obj->DropID = dropActorID;
		obj->TemplateItemID = itemTemplateID;
		obj->ItemTemplate = (UItem*)item;
		
		UStaticMesh* itemMesh = NULL;

		// Task #13020, add by zhangrui
		if (item->GetTemplateID() == obj->BindItemId)
		{// If it is bound gold
			if (itemCount <= obj->DropDivide1)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->BindDropMesh1, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide1 && itemCount <= obj->DropDivide2)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->BindDropMesh2, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide2 && itemCount <= obj->DropDivide3)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->BindDropMesh3, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide3 && itemCount <= obj->DropDivide4)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->BindDropMesh4, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide4)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->BindDropMesh5, TEXT("DefaultMMOGame.ini"));
			}
		}
		else if (item->GetTemplateID() == obj->GoldItemId)
		{// If it is gold coins
			if (itemCount <= obj->DropDivide1)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->GoldDropMesh1, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide1 && itemCount <= obj->DropDivide2)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->GoldDropMesh2, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide2 && itemCount <= obj->DropDivide3)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->GoldDropMesh3, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide3 && itemCount <= obj->DropDivide4)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->GoldDropMesh4, TEXT("DefaultMMOGame.ini"));
			}
			else if (itemCount > obj->DropDivide4)
			{
				itemMesh = LoadUEObject<UStaticMesh>(obj->GoldDropMesh5, TEXT("DefaultMMOGame.ini"));
			}
		}
		else
		{// If it is an item
			itemMesh = item->GetDropMesh();
		}

		if(itemMesh)
		{
			obj->Mesh->SetStaticMesh(itemMesh);
			if(enableCollision)
			{
				obj->CollisionComponent = obj->Mesh;
			}
			obj->ConditionalUpdateComponents();
		}
		else
		{
			warnf(NAME_Error, TEXT("[MMODropManager::AddDropActor]Item[%d][%s] drop mesh not found, use default drop mesh!"),
				itemTemplateID, *item->GetDisplayName());

			static UStaticMesh* Mesh = (UStaticMesh*)UObject::StaticLoadObject(UStaticMesh::StaticClass(),NULL,TEXT("EngineMeshes.Cube"),NULL,LOAD_None,NULL);
			obj->Mesh->SetStaticMesh(Mesh);
			if(enableCollision)
			{
				obj->CollisionComponent = obj->Mesh;
			}
			obj->ConditionalUpdateComponents();
		}

		FLOAT offsetZ = obj->CollisionComponent ? obj->CollisionComponent->Bounds.BoxExtent.Z * 0.5f : 0.0f;
		
		obj->TargetPosition = dropLocation + FVector(0, 0, offsetZ);
		//obj->Location = spawnLocation;//MMOFacade::getSingleton().GetMainActor()->Location;
		//obj->SpawnDelayTimer = spawnOrder * g_SpawnDelay;
		obj->SpawnDelayTimer = g_SpawnDelay; // #23577 Lu Chenlei: unified drop delay 0.15S
		obj->SetCollision( FALSE, FALSE, FALSE );

		FVector newSpawnLocation = spawnLocation;
		newSpawnLocation.Z = obj->TargetPosition.Z + 5;
		obj->SetLocation(newSpawnLocation);

		
		FDropActorInfo info;
		info.DropActor = obj;
		info.IsVisible = FALSE;
		info.CannotPickCD = -0.1f;

		if(!_freeDropHudQueue.empty())
		{
			info.Hud = _freeDropHudQueue.front();
			_freeDropHudQueue.pop_front();
		}
		else if(_dropTipsUI)
		{
			info.Hud = _dropTipsUI->AttachMovie( TEXT("DropItemHint"), FString::Printf( TEXT("DropActor%d"), dropActorID) );
			info.Hud->AddToRoot();
		}
		else
		{
			info.Hud = NULL;
		}

		if(_dropTipsUI && info.Hud)
		{
			FString IconName = TEXT("");
			INT realColor = UITextParser::HTMLGetItemRealColor(   (EItemColor)item->ItemColor );

			info.Hud->SetVisible(FALSE);
			info.IsVisible = FALSE;
			// Bot settings: for top-quality items, show name and mark color (even if show-name is unchecked)
			if (MMOAssistManager::getSingleton().IsDropValuable(item->ItemId))
			{
				//info.Hud->SetVisible(TRUE);
				//info.IsVisible = TRUE;
				realColor = _tcstol( TEXT("00ff00"), NULL, 16 );
			}
			else
			{
				// Visible only if the bot settings make it visible
				UBOOL defaultShow = GetShowDropHud();
				UBOOL inVisible = defaultShow & MMOAssistManager::getSingleton().IsDropVisible(item->ItemId);
				
				//info.Hud->SetVisible(inVisible);
				//info.IsVisible = inVisible;
			}
			
			_dropTipsUI->eventSetHpInfo( info.Hud, dropActorID, UITextParser::HTMLFormatItemName( item ), realColor, IconName, item->ItemColor );
		}

		//if(info.Hud)
		//{
		//	info.Hud->SetVisible(FALSE); // MMOUI::GetUIManager()->HintHideNameDrop ? FALSE : _showDropHud

		//	TArray<struct FASValue> Arguments0;
		//	FASValue ASValue0(EC_EventParm);

		//	ASValue0.I =  dropActorID; 
		//	ASValue0.Type = AS_Int;
		//	Arguments0.AddItem(ASValue0);

		//	//ASValue0.S = dropItem->PickItem->ItemData->GetDisplayName(); 
		//	ASValue0.S = UITextParser::HTMLFormatItemName( item );
		//	ASValue0.Type = AS_String;
		//	Arguments0.AddItem(ASValue0);

		//	INT realColor = UITextParser::HTMLGetItemRealColor(   (EItemColor)item->ItemColor );
		//	ASValue0.I =  realColor; 
		//	ASValue0.Type = AS_Int;
		//	Arguments0.AddItem(ASValue0);

		//	//INT realColor     = 0;
		//	//if( dropItem->PickItem->ItemData )
		//	//{
		//	//realColor     = UITextParser::HTMLGetItemRealColor( (EItemColor)dropItem->PickItem->ItemData->ItemColor );
		//	//}
		//	//ASValue0.I =  realColor; 
		//	//ASValue0.Type = AS_Int;
		//	//Arguments0.AddItem(ASValue0);

		//	FString cs = "img://MMO_Armors_Casque_Icon.A_z_casque02";
		//	ASValue0.S = cs;	//     TEXT("");
		//	ASValue0.Type = AS_String;
		//	Arguments0.AddItem(ASValue0);

		//	ASValue0.I =  item->ItemColor; 
		//	ASValue0.Type = AS_Int;
		//	
		//	Arguments0.AddItem(ASValue0);


		//	info.Hud->Invoke(TEXT("SetTipContent"), Arguments0 );
		//}
		
		_dropActoMap.insert(std::make_pair(dropActorID, info));
		
		if(isNew)
		{
			//// Play Matinee, hide drops
			//UBOOL bHidden = TRUE;
			//AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
			//if (pc && pc->bShowDropsByKismet==)
			//{
			//	bHidden = !(pc->bShowDropsByKismet);
			//}
			//obj->SetHidden(bHidden);

			obj->SetHidden(TRUE);
			obj->GotoState(FName(TEXT("SpawnAnim")));
		}
		else
		{
			obj->SetHidden(FALSE);
			obj->EndSpawnAnim(FALSE);
		}

		// Update the quest state of drops
		MMOQuestManager::getSingleton().UpdateQuestDropItemStatus(obj);
	}

	return obj;
}

void MMODropManager::DestroyDropActor( INT dropActorID, UBOOL removeFromWrold )
{
	if(removeFromWrold)
	{
		DropActorMap::iterator i = _dropActoMap.find(dropActorID);
		if(i != _dropActoMap.end())
		{
			_Destroy(i->second, removeFromWrold);
			_dropActoMap.erase(i);
		}
	}
}

void MMODropManager::DestroyAllDropActor()
{
	DropActorMap::iterator i = _dropActoMap.begin();
	for (; i != _dropActoMap.end(); ++i)
	{
		_Destroy(i->second, TRUE);
	}

	_dropActoMap.clear();
}

class AMMODropActor* MMODropManager::FindDropActor( INT dropActorID )
{
	DropActorMap::iterator i = _dropActoMap.find(dropActorID);
	if(i != _dropActoMap.end())
	{
		if(!i->second.DropActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			return i->second.DropActor;
		}
	}

	return NULL;
}

void MMODropManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEvent_LeaveGame:
	case GameEvent_LeaveServerScene:
		{
			DestroyAllDropActor();
		}
		break;
	}
}

void MMODropManager::_Destroy( FDropActorInfo& info, UBOOL removeFromWrold )
{
	if(removeFromWrold)
	{
		if(info.DropActor && !info.DropActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(info.DropActor);
		}
	
		info.DropActor = NULL;
	}

	if(info.Hud)
	{
		info.IsVisible = FALSE;
		info.Hud->SetVisible(FALSE);
		info.Hud->SetPosition(-200, -200);
		info.ScreenPos.X = -200;
		info.ScreenPos.Y = -200;
		_freeDropHudQueue.push_back(info.Hud);
	}
}

#if M_DEBUG_HUD_NAME_MOVE
INT g_hudMoveSpeed = 2;
#endif

void MMODropManager::SetDropActorCannotPick(INT nDropID,FLOAT pickCD)
{
	FDropActorInfo* dropActor = FindDropActorInfo(nDropID);
	if(dropActor)
	{
		dropActor->CannotPickCD = pickCD;
	}
}
FDropActorInfo* MMODropManager::FindDropActorInfo(INT nDropID)
{
	DropActorMap::iterator i = _dropActoMap.find(nDropID);
	if(i != _dropActoMap.end())
	{
		return &(i->second);
	}

	return NULL;
}

void MMODropManager::UpdateDropHudPos(FLOAT DeltaTime)
{
	CTRL_TICK("HUD_UpdateDropHudPos");
	m_CanPickUpDropID=0;
	m_CanPickUpInfo=NULL;
	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer==NULL)
	{
		return;
	}

	//if(_dropTipsUI && pMainPlayer)
	//{
	//	if(_alphaMoving != pMainPlayer->isMoving)
	//	{
	//		if(_alphaHoldFrame>0)
	//		{
	//			_alphaHoldFrame--;
	//		}
	//		else{
	//			_alphaMoving = pMainPlayer->isMoving;
	//			if(_alphaMoving)
	//			{
	//				_alphaCurValue=0.6f;
	//				_dropTipsUI->SetFloat(TEXT("alpha"), _alphaCurValue );
	//				//_dropTipsUI->SetVisible(FALSE);// >SetFloat(TEXT("alpha"), _alphaCurValue);
	//			}
	//			else
	//			{
	//				_alphaCurValue=0.9f;
	//				_dropTipsUI->SetFloat(TEXT("alpha"), _alphaCurValue );
	//			}
	//		}
	//	}
	//	else
	//	{
	//		_alphaHoldFrame=2;
	//	}
	//}

	const FLOAT maxPickDistance = 
		MMOFacade::getSingleton().GetMainController()->MaxPickDistance * 
		MMOFacade::getSingleton().GetMainController()->MaxPickDistance;

	FVector playerServerPos = pMainPlayer->ServerLocation;
	FVector playerPos = pMainPlayer->Location;

	DropActorMap::iterator i = _dropActoMap.begin();
	//FVector screenPos;
	FVector2D screenPos;
	FVector2D screenSz = MMOFacade::GetScreenSize();
	UBOOL defaultShow = GetShowDropHud();
	UBOOL show = defaultShow;
	INT num1 =0;
	INT num2 =0;

	for (; i != _dropActoMap.end(); ++i)
	{
		if(i->second.CannotPickCD>=0.0f)
		{
			i->second.CannotPickCD -= DeltaTime;
		}

		AMMODropActor* dropActor = i->second.DropActor;
		if(i->second.Hud && dropActor && !dropActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			FLOAT offsetZ = dropActor->CollisionComponent ?
				dropActor->CollisionComponent->Bounds.BoxExtent.Z * 4: 20.0f;


			UBOOL suc = MMOFacade::getSingleton().GetScreenPixelFromWorld( dropActor->Location + FVector(0, 0, 0) , screenPos );
			if(pMainPlayer->isMoving==FALSE)
			{
				screenPos.X = appFloor(screenPos.X + 0.5f);
				screenPos.Y = appFloor(screenPos.Y + 0.5f);
			}

			show = FALSE;
			if(dropActor->SpawnDelayTimer>0.0f) 
			{
				// Add by jimZhang@snda.com
				show = FALSE;
			}
			else if(screenPos.Y >= 0 && screenPos.Y <= screenSz.Y
				&& screenPos.X >= 0 && screenPos.X <= screenSz.X )
			{
				//show = GetShowDropHud() || dropActor->IsHover();
				show = defaultShow;

				//// The bot still needs to filter
				//if (!MMOAssistManager::getSingleton().IsDropVisible(dropActor->ItemTemplate->ItemId))
				//{
				//	show = FALSE;
				//}

				if(dropActor->ItemTemplate)
				{

					if (GetShowDropHud() == FALSE)
					{
						show = FALSE;
					}
					else
					{
						if (MMOAssistManager::getSingleton().IsDropValuable(dropActor->ItemTemplate->ItemId))
						{
							show = TRUE;
						}
						else if (MMOAssistManager::getSingleton().IsDropVisible(dropActor->ItemTemplate->ItemId) == FALSE)
						{
							show = dropActor->IsHover();
							//show = FALSE;
						}
					}
				}
			}


			if(show)
			{
				//UBOOL realShow = dropActor->IsHover() ? TRUE : GetShowDropHud();
				if(i->second.IsVisible==FALSE||_bForceRefresh)
				{ 
					
					i->second.IsVisible=TRUE;
					i->second.Hud->SetVisible(TRUE);
					i->second.ScreenPos = screenPos;
					i->second.Hud->SetPosition(screenPos.X, screenPos.Y - 30);
				}

				FVector2D DelScreenPos = i->second.ScreenPos-screenPos;
				if(DelScreenPos.SizeSquared()>0.25)
				{
					i->second.ScreenPos=screenPos;
					i->second.Hud->SetPosition(screenPos.X, screenPos.Y - 30);			
				}		

				num1++;
			}
			else
			{
				num2++;
				if(i->second.IsVisible==TRUE||_bForceRefresh)
				{
					
					i->second.IsVisible=FALSE;
					i->second.ScreenPos.X = -100;
					i->second.ScreenPos.Y = -100;
					i->second.Hud->SetPosition(-100, -100);		
					i->second.Hud->SetVisible(FALSE);
				}
			}

			// Pick up item
			FLOAT dist_SD = (playerServerPos - dropActor->Location).SizeSquared2D();
			FLOAT dist_SP = (playerPos - playerServerPos).SizeSquared2D();
			if(dist_SD<64.0f&&dist_SP<128.0f)
			{	
				if(i->second.CannotPickCD<0.0)
				{
					m_CanPickUpDropID = dropActor->DropID;
					m_CanPickUpInfo = &(i->second);

					if(_pickupAll)
					{
						if(dropActor->ItemTemplate && dropActor->ItemTemplate->ItemBindType != E_BINDTYPE_BOP)
						{
							CMessageHandler::GetInstance().Send_PickDropItem(dropActor->DropID, 0);
							i->second.CannotPickCD = 0.5f;
							dropActor->fPickupDelay = 0.5f;
						}
					}
					else if (dropActor->ItemTemplate && MMOAssistManager::getSingleton().IsDropAutoPick(dropActor->ItemTemplate->ItemId))
					{// Auto-pickup of items is set in the bot, auto-pickup
						CMessageHandler::GetInstance().Send_PickDropItem(dropActor->DropID, 0);
						i->second.CannotPickCD = 0.5f;
						dropActor->fPickupDelay = 0.5f;
					}
					else if(dropActor->ItemTemplate &&
							dropActor->ItemTemplate->IsMoney())
					{
						CMessageHandler::GetInstance().Send_PickDropItem(dropActor->DropID, 0);
						i->second.CannotPickCD = 0.5f;
						dropActor->fPickupDelay = 0.5f;
					}


				}			
			}
		}
	}

	_bForceRefresh = FALSE;
	_pickupAll = false;
}


void MMODropManager::OnCanvasPostRender( class UCanvas* canvas, FLOAT fDelta )
{
	//Change refresh method
	return;
}

void MMODropManager::SetDropTipsUI( class UMMOUIDropItemTips* tipsUI )
{
	_dropTipsUI = tipsUI;
	if(_dropTipsUI)
	{
		FLOAT AlphaValue = 1.0f;
		_dropTipsUI->SetFloat(TEXT("alpha"), AlphaValue );
	}
}

UMMOUIDropItemTips* MMODropManager::GetDropTipsUI()
{
	return _dropTipsUI;
}

void MMODropManager::PickupDropActorByUI(INT dropActorID)
{
	AMMODropActor* dropActor = FindDropActor(dropActorID);
	if(dropActor)
	{
		//MMOTargetManager::getSingleton().SetDesireTarget(NULL, dropActor, FALSE);
		FSMIO::getSingleton().Target.SetDropActor(dropActor);
		
	}
}

void MMODropManager::SetShowDropHud( UBOOL val )
{
	//m_bDesireShow = val;

	DropActorMap::iterator i = _dropActoMap.begin();
	for (; i != _dropActoMap.end(); ++i)
	{
		if (i->second.Hud && i->second.DropActor && i->second.DropActor->SpawnDelayTimer<=0.0f && !i->second.DropActor->HasAnyFlags(RF_Unreachable | RF_BeginDestroyed | RF_FinishDestroyed))
		{
			UBOOL realShow = i->second.DropActor->IsHover() ? TRUE : GetShowDropHud();

			i->second.IsVisible = realShow;
			i->second.Hud->SetVisible(realShow);
		}
	}
}

void MMODropManager::ShowDropActorHUD( INT dropActorID )
{
	DropActorMap::iterator i = _dropActoMap.find(dropActorID);
	if(i != _dropActoMap.end())
	{
		//i->second.IsVisible = true;


		if (i->second.Hud && i->second.DropActor && i->second.DropActor->SpawnDelayTimer<=0.0f && !i->second.DropActor->HasAnyFlags(RF_Unreachable | RF_BeginDestroyed | RF_FinishDestroyed))
		{
			UBOOL realShow = i->second.DropActor->IsHover() ? TRUE : GetShowDropHud();

			i->second.IsVisible = realShow;
			i->second.Hud->SetVisible(realShow);
		}
	}
}

void MMODropManager::SetDropTipsIcon(INT dropActorId, const FString& icon)
{
	DropActorMap::iterator iter = _dropActoMap.find(dropActorId);
	if (iter != _dropActoMap.end())
	{
		//FDropActorInfo info = iter->second;

		FDropActorInfo& info = iter->second;

		if (info.DropActor && info.Hud && _dropTipsUI && !info.DropActor->HasAnyFlags(RF_Unreachable | RF_BeginDestroyed | RF_FinishDestroyed))
		{
			UItem* item = MMOFacade::getSingleton().FindItemTemplate(info.DropActor->TemplateItemID);
			if (item)
			{

				FString IconName = TEXT("");
				INT realColor = UITextParser::HTMLGetItemRealColor(   (EItemColor)item->ItemColor );

				// Visible only if the bot settings make it visible

				UBOOL bSpawnEnd = info.DropActor->SpawnDelayTimer<=0.0f ? TRUE : FALSE;

				UBOOL bVisible = MMOAssistManager::getSingleton().IsDropVisible(item->ItemId) & bSpawnEnd;
				info.IsVisible = bVisible;
				info.Hud->SetVisible(bVisible);

				// Bot settings: for top-quality items, show name and mark color (even if show-name is unchecked)
				if (MMOAssistManager::getSingleton().IsDropValuable(item->ItemId))
				{
					info.IsVisible = TRUE & bSpawnEnd;
					info.Hud->SetVisible(TRUE & bSpawnEnd);
					realColor = _tcstol( TEXT("00ff00"), NULL, 16 );
				}

				_dropTipsUI->eventSetHpInfo( info.Hud, dropActorId, UITextParser::HTMLFormatItemName( item ), realColor,icon,item->ItemColor );

				//TArray<struct FASValue> Arguments;
				//FASValue ASValue(EC_EventParm);

				//ASValue.I = dropActorId; 
				//ASValue.Type = AS_Int;
				//Arguments.AddItem(ASValue);

				//ASValue.S = UITextParser::HTMLFormatItemName(item);
				//ASValue.Type = AS_String;
				//Arguments.AddItem(ASValue);

				//INT realColor = UITextParser::HTMLGetItemRealColor((EItemColor)item->ItemColor);
				//ASValue.I =  realColor; 
				//ASValue.Type = AS_Int;
				//Arguments.AddItem(ASValue);

				//ASValue.S = icon;
				//ASValue.Type = AS_String;
				//Arguments.AddItem(ASValue);

				//ASValue.I =  item->ItemColor; 
				//ASValue.Type = AS_Int;
				//Arguments.AddItem(ASValue);

				//info.Hud->Invoke(TEXT("SetTipContent"), Arguments );
			}
		}
	}
}

#if !SHIPPING_PC_GAME
void MMODropManager::TestDropAll( INT dist )
{
	size_t cnt = MMOFacade::getSingleton()._itemTemplateMap.size();
	size_t cntPerRow = (size_t)appSqrt((FLOAT)cnt);

	size_t idx = 0;
	FVector curPos = MMOFacade::getSingleton().GetMainActor()->Location;
	ItemTemplateMap::const_iterator it = MMOFacade::getSingleton()._itemTemplateMap.begin();
	for (; it != MMOFacade::getSingleton()._itemTemplateMap.end(); ++it)
	{
		FLOAT xOff, yOff;
		xOff = idx % cntPerRow * dist;
		yOff = idx / cntPerRow * dist;

		FVector dropLoc = curPos + FVector(xOff, yOff, 0.0f);
		AddDropActor(true, dropLoc, dropLoc, it->first, 1, idx, false, idx);
		++idx;
	}
}
#endif


UBOOL MMODropManager::GetShowDropHud() const
{
	//return m_bDesireShow || !MMOUI::GetUIManager()->HintHideNameDrop;
	return !MMOUI::GetUIManager()->HintHideNameDrop;
}

void DropGroupDataProcessor::ParseData( FDropGroup* obj, class CTabFile* dataRow, int rowIdx )
{
	if(obj->DropItemList.Num() == 0)
	{
		obj->TemplateID = TReadDataValue_Int<MMODropGroupDataManager>(TEXT("DropGroupID"), dataRow, rowIdx);
		obj->Enabled = (UBOOL)TReadDataValue_Int<MMODropGroupDataManager>(TEXT("Enabled"), dataRow, rowIdx);
		obj->TemplateDesc = TReadDataValue_FString<MMODropGroupDataManager>(TEXT("Desc"), dataRow, rowIdx);
	}

	FDropItemSet dropSet;
	
	dropSet.ItemTemplateID = TReadDataValue_Float<MMODropGroupDataManager>(TEXT("Item"), dataRow, rowIdx);
	dropSet.DropWeight = TReadDataValue_Float<MMODropGroupDataManager>(TEXT("Weight"), dataRow, rowIdx);
	dropSet.CountMin = TReadDataValue_Int<MMODropGroupDataManager>(TEXT("CountMin"), dataRow, rowIdx);
	dropSet.CountMax = TReadDataValue_Int<MMODropGroupDataManager>(TEXT("CountMax"), dataRow, rowIdx);

	obj->DropItemList.AddItem(dropSet);
}

extern FLOAT g_RPS;
extern FLOAT g_RPSDeAcc;
extern FLOAT g_ZGravity;
extern FLOAT g_InitZSpeed;
extern FLOAT g_SpawnDelay;

void DropGroupDataProcessor::PostInitData()
{
	GConfig->GetFloat(TEXT("MMOGame.MMODropActor"), TEXT("RPS"), g_RPS, GGameIni);
	GConfig->GetFloat(TEXT("MMOGame.MMODropActor"), TEXT("RPSDeAcc"), g_RPSDeAcc, GGameIni);
	GConfig->GetFloat(TEXT("MMOGame.MMODropActor"), TEXT("ZGravity"), g_ZGravity, GGameIni);
	GConfig->GetFloat(TEXT("MMOGame.MMODropActor"), TEXT("InitZSpeed"), g_InitZSpeed, GGameIni);
	GConfig->GetFloat(TEXT("MMOGame.MMODropActor"), TEXT("SpawnDelay"), g_SpawnDelay, GGameIni);
}

FDropGroup* DropGroupDataProcessor::CreateDataObject( INT templateID, bool& needAddToMap )
{
	FDropGroup* dropGrop = MMODropGroupDataManager::getSingleton().FindAndModifyData(templateID);

	if(!dropGrop)
	{
		needAddToMap = true;
		dropGrop = new FDropGroup;
		dropGrop->TemplateID = templateID;
	}
	else
	{
		needAddToMap = false;
	}

#if 0
	UDropGroup* item = MMODropGroupDataManager::getSingleton().FindAndModifyData(templateID);

	if(!item)
	{
		needAddToMap = true;
		item = Cast<UDropGroup>(GEngine->StaticConstructObject(UDropGroup::StaticClass()));
		item->AddToRoot();
	}
	else
	{
		needAddToMap = false;
	}
#endif

	return dropGrop;
}

class UItem* FDropItemSet::GetDropItemTemplate() const
{
	if(ItemTemplateID > 0)
	{
		return MMOFacade::getSingleton().FindItemTemplate(ItemTemplateID);
	}

	return NULL;
}

//------------------------------------------------------------------------------------------------
// MMODropGroupDataManager
//------------------------------------------------------------------------------------------------

#if !SHIPPING_PC_GAME
bool MMODropGroupDataManager_IsItemExists( INT itemTemplateID )
{
	MMODropGroupDataManager::DataTypeMap::const_iterator i = MMODropGroupDataManager::getSingleton().GetDataMap().begin();
	for (; i != MMODropGroupDataManager::getSingleton().GetDataMap().end(); ++i)
	{
		const FDropGroup& dropGroup = *i->second;
		for (INT j = 0; j < dropGroup.DropItemList.Num(); ++j)
		{
			if(dropGroup.DropItemList(j).ItemTemplateID == itemTemplateID)
			{
				return true;
			}
		}
	}

	return false;
}
#endif