#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOJigsawManager.h"
#include "MMOFacade.h"
#include "MMOMsgButtonManager.h"
#include "MMOChatManager.h"
#include "MMOMsgBoxManager.h"

//=======================================================================================
MMOJigsawManager::MMOJigsawManager()
	: _OpenJigsawPanelDelegate(EC_EventParm)
{
	Init();
}

void MMOJigsawManager::Init()
{
	isBuildingTexture = FALSE;
	isBuildDone = FALSE;
	appMemzero(_TargetTextureData, sizeof(_TargetTextureData));
	appMemzero(_AnswerTextureData, sizeof(_AnswerTextureData));

	pTargetTexture = NULL;

	_HintJigsawMsgButtonIdx = -1;

	int i;
	for (i=0;i<JIGSAW_ANSWER_COUNT;i++)
	{
		pAnsTexture[i] = NULL;
	}

	_DeadLineTime = 0;
	_BlockX = 0;
	_BlockY = 0;
}
//=======================================================================================
void MMOJigsawManager::Shutdown()
{
	FScriptDelegate nullDelgate(EC_EventParm);
	SetOpenJigsawPanelDelegate( nullDelgate );

	if (pTargetTexture)
	{
		pTargetTexture->RemoveFromRoot();
	}
	
	INT i;
	for(i=0;i<JIGSAW_ANSWER_COUNT;i++)
	{
		if (pAnsTexture[i])
		{
			pAnsTexture[i]->RemoveFromRoot();
		}		
	}
}

void MMOJigsawManager::Update(float deltaTime)
{
	if (isBuildDone)
	{
		isBuildDone = FALSE;
		if( _HintJigsawMsgButtonIdx >= 0 )
		{
			RemoveMessageButtionBar();
		}
		MMOMsgButtonManager_Parms Parms(EC_EventParm);
		_HintJigsawMsgButtonIdx = MMOMsgButtonManager::getSingleton().AddNewButton( _OpenJigsawPanelDelegate, Parms, E_MsgBtnShow_Jigsaw );
	}

	INT nForceOpenTime = 30;
	GConfig->GetInt(TEXT("AntiCheat"),TEXT("AutoOpenDeadLine"),nForceOpenTime,GDefaultGameIni);
	if (MMOFacade::getSingleton().GetRealClientUTCTime() + nForceOpenTime > _DeadLineTime)
	{
		if (_HintJigsawMsgButtonIdx >= 0)
		{
			MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Jigsaw , TRUE, "" );
		}
	}
}

UTexture2D* MMOJigsawManager::CreateJigsawTexture( INT sizeX, INT sizeY )
{
	UTexture2D* NewTexture = Cast<UTexture2D>(UObject::StaticConstructObject(
		UTexture2D::StaticClass(), UObject::GetTransientPackage(), NAME_None, RF_Transient));
	if (NewTexture != NULL)
	{
		NewTexture->CompressionNone			= FALSE;
		NewTexture->CompressionSettings		= TC_Default;
		NewTexture->CompressionNoAlpha		= TRUE;
		NewTexture->DeferCompression		= FALSE;
		NewTexture->bNoTiling				= TRUE;
		NewTexture->SRGB					= FALSE;
		NewTexture->Init(
			sizeX, 
			sizeY, 
			PF_DXT1);

		NewTexture->UpdateResource();

		NewTexture->AddToRoot();
	}
	return NewTexture;
}

BOOL MMOJigsawManager::IsBuildingTextrue()
{
	return isBuildingTexture;
}

void MMOJigsawManager::SetOpenJigsawPanelDelegate(FScriptDelegate fun)
{
	_OpenJigsawPanelDelegate = fun;
}

void MMOJigsawManager::CopyTargetTextureData(char *src,INT copySize /* = JIGSAW_DATA_SIZE */)
{
	if (isBuildingTexture == TRUE)
	{
		return ;
	}
	if (!src || copySize < 0 || (UINT)copySize > sizeof(_TargetTextureData))
		return;
	appMemzero(_TargetTextureData, sizeof(_TargetTextureData));
	appMemcpy(_TargetTextureData , src , copySize);
}

void MMOJigsawManager::CopyAnswerTextureData(char *src,INT nIndex , INT copySize /* = JIGSAW_ANSWER_DATA_SIZE */)
{
	if (isBuildingTexture)
	{
		return ;
	}
	if (!src || nIndex < 0 || nIndex >= JIGSAW_ANSWER_COUNT ||
		copySize < 0 || (UINT)copySize > sizeof(_AnswerTextureData[nIndex]))
		return;
	appMemzero(_AnswerTextureData[nIndex], sizeof(_AnswerTextureData[nIndex]));
	appMemcpy(_AnswerTextureData[nIndex] , src , copySize);
}

void MMOJigsawManager::OnRecieveJigsawMsg()
{
	if (isBuildingTexture)
	{
		return ;
	}

	if (pTargetTexture == NULL)
	{
		pTargetTexture = CreateJigsawTexture( JIGSAW_PIC_WIDTH, JIGSAW_PIC_WIDTH );
		if (pTargetTexture == NULL)
		{
			return ;
		}
	}

	int i;
	for (i=0;i<JIGSAW_ANSWER_COUNT;i++)
	{
		if (pAnsTexture[i] == NULL)
		{
			pAnsTexture[i] = CreateJigsawTexture( JIGSAW_ANSWER_WIDTH, JIGSAW_ANSWER_WIDTH );
			if (pAnsTexture[i] == NULL)
			{
				return ;
			}
		}		
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		BuildJigsawTexture,
		MMOJigsawManager*, classPtr, this,
	{
		classPtr->BuilJigsawTexture();
	} );

	return ;
}

static BOOL UploadJigsawTexture(UTexture2D* texture, const char* source, UINT width, size_t sourceSize)
{
	if (!texture || !source || !width || width % JIGSAW_BLOCK_WIDTH || !texture->Resource ||
		texture->SizeX <= 0 || (UINT)texture->SizeX != width || (UINT)texture->SizeY != width)
		return FALSE;
	const size_t blockRows = width / JIGSAW_BLOCK_WIDTH;
	if (blockRows > (size_t)-1 / sizeof(UINT64)) return FALSE;
	const size_t rowBytes = blockRows * sizeof(UINT64);
	if (blockRows > (size_t)-1 / rowBytes || sourceSize != rowBytes * blockRows)
		return FALSE;
	FTexture2DResource* resource = (FTexture2DResource*)texture->Resource;
	UINT stride = 0;
	BYTE* destination = (BYTE*)RHILockTexture2D(resource->GetTexture2DRHI(), 0, TRUE, stride, FALSE);
	if (!destination)
		return FALSE;
	BOOL copied = FALSE;
	if (stride >= rowBytes && blockRows - 1 <= ((size_t)-1 - rowBytes) / stride)
	{
		for (size_t row = 0; row < blockRows; ++row)
			appMemcpy(destination + row * stride, source + row * rowBytes, rowBytes);
		copied = TRUE;
	}
	RHIUnlockTexture2D(resource->GetTexture2DRHI(), 0, FALSE);
	return copied;
}

void MMOJigsawManager::BuilJigsawTexture()
{
	isBuildingTexture = TRUE;
	isBuildDone = FALSE;
	if (!UploadJigsawTexture(pTargetTexture, _TargetTextureData, JIGSAW_PIC_WIDTH, sizeof(_TargetTextureData)))
	{
		isBuildingTexture = FALSE;
		return;
	}
	for (INT index = 0; index < JIGSAW_ANSWER_COUNT; ++index)
	{
		if (!UploadJigsawTexture(pAnsTexture[index], _AnswerTextureData[index], JIGSAW_ANSWER_WIDTH,
			sizeof(_AnswerTextureData[index])))
		{
			isBuildingTexture = FALSE;
			return;
		}
	}
	isBuildingTexture = FALSE;
	isBuildDone = TRUE;
}

void MMOJigsawManager::OnSelectAnswer(INT answer)
{
	if(answer >= 0 && answer < JIGSAW_ANSWER_COUNT)
	{
		CMessageHandler::GetInstance().Send_JigsawAnswer(answer);
	}
}

UTexture2D* MMOJigsawManager::GetJigsawTexture()
{
	return pTargetTexture;
}

UTexture2D* MMOJigsawManager::GetAnswerTexture(INT nIndex)
{
	if (nIndex >= 0 && nIndex < JIGSAW_ANSWER_COUNT)
	{
		return pAnsTexture[nIndex];
	}
	return NULL;
}

void MMOJigsawManager::RemoveMessageButtionBar()
{
	if(IsMessageButtonBarShowing())
	{
		MMOMsgButtonManager::getSingleton().RemoveOneTypeButtons( E_MsgBtnShow_Jigsaw );
		_HintJigsawMsgButtonIdx = -1;
	}
}

void MMOJigsawManager::OnRetCode(INT32 nErrCode,INT32 nParam1)
{
	if (nErrCode == errAntiCheatSuccess)
	{
		FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAntiCheatSuccess") );
		FString txt = FString::Printf( *temp, nParam1 );
		MMOChatManager::getSingleton().AddSystemMessage( txt );
	}
	else if(nErrCode == errAntiCheatError)
	{
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAntiCheatError")));
	}
	else if(nErrCode == errAntiCheatErrorKick)
	{
		FString localVal = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAntiCheatErrorKick"));
		MMOUI::GetUIManager()->delegateAddScreenMessage( localVal ,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAntiCheatErrorKick")); 
		MMOChatManager::getSingleton().AddSystemMessage( localVal );
	}
	else if(nErrCode == errAntiCheatErrorTimeoutReturn)
	{
		FString localVal = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAntiCheatErrorTimeoutReturn"));
		MMOUI::GetUIManager()->delegateAddScreenMessage( localVal ,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAntiCheatErrorTimeoutReturn")); 
		MMOChatManager::getSingleton().AddSystemMessage( localVal );
	}
}
