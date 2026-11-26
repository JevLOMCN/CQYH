#include "stdafx.h"
#include "ReplayManager.h"
#include "..\..\Common\define\version.h" // oscar@snda
#if !M_ENABLE_RPMGR

void RecordInputKey(INT ControllerId,FName Key,EInputEvent EventType,FLOAT AmountDepressed,UBOOL bGamepad)
{
}

void RecordInputAxis(INT ControllerId,FName Key,FLOAT Delta,FLOAT DeltaTime, UBOOL bGamepad)
{
}

void RecordInputChar(INT ControllerId,TCHAR Character)
{
}
#else

bool g_replayMode = false;

void RecordInputKey(INT ControllerId,FName Key,EInputEvent EventType,FLOAT AmountDepressed,UBOOL bGamepad)
{
#if M_ENABLE_RPMGR
	if(g_replayMode || GIsEditor)
	{
		return;
	}

	FViewport* vp = GEngine->GameViewport->Viewport;

	FReplayDataInput* dat = new FReplayDataInput;

	vp->GetMousePos(dat->MousePoint);
	dat->GameTime = appSeconds() - GStartTime;
	dat->DataType = EIDT_Key;
	dat->ControllerId = ControllerId;
	dat->Key = Key;
	dat->EventType = EventType;
	dat->AmountDepressed = AmountDepressed;
	dat->GamePad = bGamepad;
	ReplayManager::getSingleton().Recode(dat);
#endif
}

void RecordInputAxis(INT ControllerId,FName Key,FLOAT Delta,FLOAT DeltaTime, UBOOL bGamepad)
{
#if M_ENABLE_RPMGR
	if(g_replayMode || GIsEditor)
	{
		return;
	}

	FViewport* vp = GEngine->GameViewport->Viewport;

	FReplayDataInput* dat = new FReplayDataInput;

	vp->GetMousePos(dat->MousePoint);
	dat->GameTime = appSeconds() - GStartTime;
	dat->DataType = EIDT_Axis;
	dat->ControllerId = ControllerId;
	dat->Key = Key;
	dat->Delta = Delta;
	dat->DeltaTime = DeltaTime;
	dat->GamePad = bGamepad;
	ReplayManager::getSingleton().Recode(dat);
#endif
}

void RecordInputChar(INT ControllerId,TCHAR Character)
{
#if M_ENABLE_RPMGR
	if(g_replayMode || GIsEditor)
	{
		return;
	}

	FViewport* vp = GEngine->GameViewport->Viewport;

	FReplayDataInput* dat = new FReplayDataInput;

	vp->GetMousePos(dat->MousePoint);
	dat->GameTime = appSeconds() - GStartTime;
	dat->DataType = EIDT_Char;
	dat->ControllerId = ControllerId;
	dat->Character = Character;
	ReplayManager::getSingleton().Recode(dat);
#endif
}

void ReplayManager::Recode( FReplayData* newData )
{
#if M_ENABLE_RPMGR
	if(g_replayMode || GIsEditor)
	{
		newData->Destroy();
		return;
	}

	static UINT64 AutoID = 0;
	newData->Index = AutoID;
	++AutoID;

#if defined(_DEBUG) && LOG_KEY_MESSAGE
	if(!newData->IsHiFrequencyData())
	{
		SET_WARN_COLOR(COLOR_GREEN);
		warnf(TEXT("==========%s"), *newData->ToString());
		CLEAR_WARN_COLOR();
	}
#endif

	if(_archive)
	{
		EReplayDataType dataType = newData->GetType();
		_archive->Serialize(&dataType, sizeof(EReplayDataType));
		newData->Serialize(*_archive);
		_archive->Flush();
	}

	// no need to save in memory
	//_replayDataList.push_back(newData);
#endif

	if(newData)
	{
		newData->Destroy();
	}
}

ReplayManager::ReplayManager()
	: _archive(0)
{
	
}

ReplayManager::~ReplayManager()
{	
}

void ReplayManager::Init()
{
#if M_ENABLE_RPMGR
	if(GIsEditor)
	{
		return;
	}

	FString rplyFile;
	if(Parse(appCmdLine(), TEXT("-REPLY="), rplyFile))
	{
		g_replayMode = true;
	}

	if(!g_replayMode)
	{
		FString replayFile = FString::Printf(TEXT("%sReplay_%d_%s.dat"),
			*appGameLogDir(), BUILD_NUMBER, *appSystemTimeString());
		
		_archive = GFileManager->CreateFileWriter(*replayFile);

		FReplayDataScreenRes* scrDat = new FReplayDataScreenRes;
		scrDat->ResX = GEngine->GetAViewport()->GetSizeX();
		scrDat->ResY = GEngine->GetAViewport()->GetSizeY();

		RHIGetSupportedResolution(scrDat->ResX, scrDat->ResY);
		scrDat->IsFullScreen = GSystemSettings.bFullscreen;
		Recode(scrDat);

// 		if(_archive)
// 		{
// 			_archive->Serialize(&_initMousePt, sizeof(FIntPoint));
// 		}
	}
	else
	{
		SET_WARN_COLOR(COLOR_PURPLE);
		warnf(TEXT("Start Replay==========%s"), *rplyFile);
		CLEAR_WARN_COLOR();

		_ClearReplayData();
		FString replayFile = appGameLogDir() + rplyFile;
		FArchive* ar = GFileManager->CreateFileReader(*replayFile);
		if(ar)
		{
			while (!ar->AtEnd() && !ar->IsError())
			{
				EReplayDataType dataType = ERDT_Network;
				ar->Serialize(&dataType, sizeof(dataType));
				if (ar->IsError())
					break;
				FReplayData* dat = NULL;
				switch (dataType)
				{
				case ERDT_Network: dat = new FReplayDataNetwork; break;
				case ERDT_Input: dat = new FReplayDataInput; break;
				case ERDT_ScreenRes: dat = new FReplayDataScreenRes; break;
				}
				if (!dat)
					break;
				if (!dat->Serialize(*ar))
				{
					dat->Destroy();
					break;
				}
				_replayDataList.push_back(dat);
			}

			delete ar;
		}
		else
		{
			appMsgf(AMT_OK, TEXT("Can't find replay file:%s!"), *rplyFile);
		}
	}
#endif
}

void ReplayManager::ShutDown()
{
	if(GIsEditor)
	{
		return;
	}

	_ClearReplayData();

	if(_archive)
	{
		_archive->Close();
		delete _archive;
		_archive = 0;
	}
}

void ReplayManager::Tick()
{
#if M_ENABLE_RPMGR
	if(GIsEditor)
	{
		return;
	}

	if(!g_replayMode)
	{
		if(GEngine->GameViewport)
		{
			FViewport* vp = GEngine->GameViewport->Viewport;

			FReplayDataInput* dat = new FReplayDataInput;

			vp->GetMousePos(dat->MousePoint);
			dat->GameTime = appSeconds() - GStartTime;
			dat->DataType = EIDT_MousePosition;
			ReplayManager::getSingleton().Recode(dat);
		}
		return;
	}

	DOUBLE gameTime = appSeconds() - GStartTime;
	ReplayDataList::iterator i = _replayDataList.begin();
	while(i != _replayDataList.end() && ((*i)->GameTime <= gameTime))
	{
		(*i)->DispatchReplayData();

		if(!(*i)->IsHiFrequencyData())
		{
			SET_WARN_COLOR(COLOR_PURPLE);
			warnf(TEXT("Replay==========%s"), *(*i)->ToString());
			CLEAR_WARN_COLOR();
		}

		(*i)->Destroy();
		++i;
	}

	_replayDataList.erase(_replayDataList.begin(), i);
#endif
}

void ReplayManager::_ClearReplayData()
{
	ReplayDataList::iterator i = _replayDataList.begin();
	for (; i != _replayDataList.end(); ++i)
	{
		//delete[] (*i)->DataBuffer;
		(*i)->Destroy();
	}
	_replayDataList.clear();
}

//#endif

void FReplayDataNetwork::DispatchReplayData()
{
	if(DataType == ENDT_Recv)
	{
		CMessageHandler::GetInstance().DebugParseMsg(DataBuffer, DataSize);
	}
}

UBOOL FReplayDataNetwork::Serialize(FArchive& archive)
{
	if (!FReplayData::Serialize(archive))
		return FALSE;
	INT dataType = (INT)DataType;
	INT dataSize = DataSize;
	archive << dataType;
	archive << dataSize;
	if (archive.IsError() || (dataType != ENDT_Recv && dataType != ENDT_Send) ||
		dataSize < (INT)sizeof(MESSAGE_HEADER) || dataSize > 0xFFFF)
		return FALSE;
	if (archive.IsSaving())
	{
		if (!DataBuffer)
			return FALSE;
		archive.Serialize(DataBuffer, dataSize);
		return !archive.IsError();
	}
	if (archive.Tell() < 0 || archive.Tell() > archive.TotalSize() ||
		dataSize > archive.TotalSize() - archive.Tell())
		return FALSE;
	char* buffer = new char[dataSize];
	archive.Serialize(buffer, dataSize);
	if (archive.IsError())
	{
		delete[] buffer;
		return FALSE;
	}
	delete[] DataBuffer;
	DataBuffer = buffer;
	DataSize = dataSize;
	DataType = (ENetworkDataType)dataType;
	return TRUE;
}

FString FReplayDataNetwork::ToString() const
{
	if (!DataBuffer || DataSize < (INT)sizeof(MESSAGE_HEADER))
		return FString(TEXT("Invalid network replay"));
	MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)DataBuffer;

	FString str = FString("[") + appItoa(Index) + FString(TEXT("]FReplayDataNetwork:["))
		+ appItoa(DataSize) + FString(TEXT("]"));

#ifdef _DEBUG
		if (pHeader->wMsg < ExternalMessage::max_message_count)
			str += CMessageHandler::GetInstance().m_MessageDebugString[pHeader->wMsg];
#endif

	return str;
}

void FReplayDataInput::DispatchReplayData()
{	
	UGameViewportClient* GameViewportClient = GEngine->GameViewport;
	FViewport* vp = GameViewportClient->Viewport;
	vp->SetMouse(MousePoint.X, MousePoint.Y);

	switch(DataType)
	{
	case EIDT_Key:
		{
			GameViewportClient->InputKey(vp, ControllerId, Key, EventType, AmountDepressed, GamePad);
		}
		break;

	case EIDT_Axis:
		{
			GameViewportClient->InputAxis(vp, ControllerId, Key, Delta, DeltaTime, GamePad);
		}
		break;

	case EIDT_Char:
		{
			GameViewportClient->InputChar(vp, ControllerId, Character);
		}
		break;

	case EIDT_MousePosition:
		{
			// have done
		}
		break;
	}
}

UBOOL FReplayDataInput::Serialize( FArchive& archive )
{
	if (!FReplayData::Serialize(archive))
		return FALSE;
	INT tmpDataType = (INT)DataType;
	archive << tmpDataType;
	if (archive.IsError() || tmpDataType < EIDT_Key || tmpDataType > EIDT_MousePosition)
		return FALSE;
	DataType = (EInputDataType)tmpDataType;

	archive.Serialize(&MousePoint, sizeof(FIntPoint));

	archive << ControllerId;

	if(DataType == EIDT_Key)
	{
		if(archive.IsSaving())
		{
			NAME_INDEX Name = Key.GetIndex();
			INT Number = Key.GetNumber();
			archive.ByteOrderSerialize(&Name, sizeof(Name));
			archive.ByteOrderSerialize(&Number, sizeof(Number));
		}
		else
		{
			NAME_INDEX Name = 0;
			INT Number = 0;
			archive.ByteOrderSerialize(&Name, sizeof(Name));
			archive.ByteOrderSerialize(&Number, sizeof(Number));
			// copy over the name with a name made from the name index and number
			if (archive.IsError() || Name < 0 || Name >= FName::GetMaxNames() || !FName::GetEntry(Name) || Number < 0)
				return FALSE;
			Key = FName((EName)Name, Number);
		}
	}

	INT tmpEventType = (INT)EventType;
	archive << tmpEventType;
	if (archive.IsError() || (DataType == EIDT_Key && (tmpEventType < IE_Pressed || tmpEventType >= IE_MAX)))
		return FALSE;
	EventType = (EInputEvent)tmpEventType;
	archive << AmountDepressed;
	archive << GamePad;

	archive << Delta;
	archive << DeltaTime;

	archive.Serialize(&Character, sizeof(TCHAR));
	return !archive.IsError();
}

FString FReplayDataInput::ToString() const
{
	const TCHAR* dataTypeStr = DataType >= EIDT_Key && DataType <= EIDT_MousePosition ? g_inputDataTypeStr[DataType] : TEXT("Unknown");
	FString str = FString("[") + appItoa(Index) + FString(TEXT("]FReplayDataInput:["))
		+ dataTypeStr + FString(TEXT("] ")) + appItoa(MousePoint.X) + TEXT(",") + appItoa(MousePoint.Y) + TEXT(" ");

	if(DataType == EIDT_Key)
	{
		static TCHAR* evtTypeStr[] =
		{
			TEXT("IE_Pressed"),
			TEXT("IE_Released"),
			TEXT("IE_Repeat"),
			TEXT("IE_DoubleClick"),
			TEXT("IE_Axis"),
			TEXT("IE_MAX")
		};

		if (EventType >= IE_Pressed && EventType < IE_MAX)
			str += evtTypeStr[EventType];
	}

	return str;
}

UBOOL FReplayDataScreenRes::Serialize( FArchive& archive )
{
	archive << ResX;
	archive << ResY;
	archive << IsFullScreen;
	return !archive.IsError();
}

void FReplayDataScreenRes::DispatchReplayData()
{
	//GSystemSettings.SetResolution(ResX, ResY, IsFullScreen);
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{
		ctrl->ConsoleCommand(FString::Printf(TEXT("setres %dx%d %s"), ResX, ResY,
			IsFullScreen ? TEXT("F") : TEXT("W")));
	}
}

FString FReplayDataScreenRes::ToString() const
{
	return FString::Printf(TEXT("%dx%d-%d"), ResX, ResY, IsFullScreen);
}

#endif // SHIPPING_PC_GAME