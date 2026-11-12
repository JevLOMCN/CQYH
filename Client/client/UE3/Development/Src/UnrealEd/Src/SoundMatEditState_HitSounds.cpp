#include "SoundMatEditState_HitSounds.h"
#include "UnLinkedObjDrawUtils.h"
#include "SoundMaterialMatrix.h"

#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

SoundMatEditState_HitSounds::SoundMatEditState_HitSounds( SoundMaterialMatrix* ed )
: SoundMaterialEditState(ed)
, SelectedObject(0)
, HighlightObject(0)
{
	WeaponHitEnumObj = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EWeaponHitType"), TRUE);
	MaterialTypeEnumObj = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EItemMaterialType"), TRUE);

}
//------------------------------------------------------------------------
SoundMatEditState_HitSounds::~SoundMatEditState_HitSounds()
{

}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::DrawObjects( FViewport* Viewport, FCanvas* Canvas )
{
	for (INT i = 0; i < MaterialTypeEnumObj->NumEnums() - 1; ++i)
	{
		//FRotator rot( C_DegToRad * 45, C_DegToRad * 45, C_DegToRad * 45);
		//FRotationMatrix mat(rot);
		//Canvas->PushAbsoluteTransform(mat);

		FLinkedObjDrawUtils::DrawTitleBar(Canvas, 
			FIntPoint(i * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE) + DRAW_X_OFFSET,
			DRAW_Y_OFFSET - DRAW_BLOCK_SIZE - DRAW_BLOCK_SPACE),
			FIntPoint(DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE), BorderColor, FColor(255,255,128),BkgColor,
			*GetEnumEnglishName(MaterialTypeEnumObj, i), TArray<FString>());

		//Canvas->PopTransform();
	}

	for (INT i = 0; i < WeaponHitEnumObj->NumEnums() - 1; ++i)
	{
		FLinkedObjDrawUtils::DrawTitleBar(Canvas, 
			FIntPoint(DRAW_X_OFFSET - DRAW_BLOCK_SIZE - DRAW_BLOCK_SPACE,
				i * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE) + DRAW_Y_OFFSET),
			FIntPoint(DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE), BorderColor, FColor(255,255,128),BkgColor,
			*GetEnumEnglishName(WeaponHitEnumObj, i), TArray<FString>());
	}

	for (INT x = EWHT_HitType0; x < EWHT_MAX; ++x)
	{
		for (INT y = EIMAT_Default; y < EIMAT_MAX; ++y)
		{
			UMMOSoundMaterialEditObject* EditProxyObject = FindEditProxyObject(x, y);
			if( EditProxyObject == NULL)
				continue;
			if(Canvas->IsHitTesting())
			{
				Canvas->SetHitProxy(new HAnyObjectProxy(EditProxyObject, (int)ETO_HitSounds, 0));
			}

			FColor tBorderColor = BorderColor;
			FColor tBkgColor = BkgColor;

			if(!EditProxyObject->ToPlaySound)
			{
				tBkgColor = WhiteColor;
			}
			else
			{
				tBkgColor = GreenColor;
			}


			if((SelectedObject == EditProxyObject) || (HighlightObject == EditProxyObject))
			{
				tBorderColor = RedColor;
			}
			else
			{
				tBorderColor = BorderColor;
			}

			FLinkedObjDrawUtils::DrawTitleBar(Canvas,
				FIntPoint(DRAW_X_OFFSET + y * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE),
				DRAW_Y_OFFSET + x * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE)),
				FIntPoint(DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE),
				tBorderColor,FColor(255,255,128), tBkgColor, TEXT(""), TArray<FString>());

			if(Canvas->IsHitTesting())
			{
				Canvas->SetHitProxy(0);
			}
		}
	}


	Canvas->PushAbsoluteTransform(FMatrix::Identity);
	
	INT msX = SoundMaterialEditor->LinkedObjVC->OldMouseX;
	INT msY = SoundMaterialEditor->LinkedObjVC->OldMouseY;
	UINT szX = SoundMaterialEditor->LinkedObjVC->Viewport->GetSizeX();
	UINT szY = SoundMaterialEditor->LinkedObjVC->Viewport->GetSizeY();
	
	DrawLine2D(Canvas, FVector2D(0, msY), FVector2D(szX, msY), FLinearColor(0, 0, 128));
	DrawLine2D(Canvas, FVector2D(msX, 0), FVector2D(msX, szY), FLinearColor(0, 0, 128));

	if(HighlightObject)
	{
		FString tipMsg = FString::Printf( TEXT("%s -> %s"),
			*GetEnumEnglishName(WeaponHitEnumObj, HighlightObject->HitType),
			*GetEnumEnglishName(MaterialTypeEnumObj, HighlightObject->TargetArmorMatType));

		FLinkedObjDrawInfo drawInfo;
		drawInfo.ToolTips.AddItem(tipMsg);
		FLinkedObjDrawUtils::DrawToolTips(Canvas, drawInfo, FIntPoint(msX, msY), FIntPoint(100, 50));
	}

	Canvas->PopTransform();
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::OnEnter()
{
	_DestroyEditProxyObject();

	for (INT x = EIMAT_Default; x < EIMAT_MAX; ++x)
	{
		for (INT y = EIMAT_Default; y < EIMAT_MAX; ++y)
		{
			UMMOSoundMaterialEditObject* obj = Cast<UMMOSoundMaterialEditObject>(
				UObject::StaticConstructObject(UMMOSoundMaterialEditObject::StaticClass()));

			//obj->SoundMaterialGroupRef = 0;
			obj->HitType = x;
			obj->TargetArmorMatType = y;

			FHitSoundNode* sndGrp = FindSoundMaterialGroupObject(x, y);
			if(sndGrp)
			{
				obj->HitType = sndGrp->HitType;
				obj->TargetArmorMatType = sndGrp->TargetArmorMatType;
				obj->ToPlaySound = sndGrp->ToPlaySound;
			}

			EditProxyObjectArray.AddItem(obj);
		}
	}
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::OnExit()
{
	_DestroyEditProxyObject();
	SelectedObject = 0;
	HighlightObject = 0;
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::ClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	if(proxy->ObjectTypeFlag == ETO_HitSounds)
	{
		SelectedObject = (UMMOSoundMaterialEditObject*)proxy->AnyObject;
		if(SelectedObject)
		{
			SoundMaterialEditor->SetObjectToPropertyWindow(SelectedObject);

			FHitSoundNode* sndGrp = FindSoundMaterialGroupObject(
				SelectedObject->HitType, SelectedObject->TargetArmorMatType);

			// add new one if not found
			if(!sndGrp)				
			{
				FHitSoundNode hitCfg(EC_EventParm);
				hitCfg.HitType = SelectedObject->HitType;
				hitCfg.TargetArmorMatType = SelectedObject->TargetArmorMatType;

				SoundMaterialEditor->GetDataManager()->HitEffectList.AddItem(hitCfg);
				SoundMaterialEditor->MakePackageDirty();
			}
		}
	}
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::HighlightAnyObject( struct HAnyObjectProxy* proxy )
{
	HighlightObject = 0;

	if(proxy && proxy->ObjectTypeFlag == ETO_HitSounds)
	{
		HighlightObject = (UMMOSoundMaterialEditObject*)proxy->AnyObject;
	}
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::_DestroyEditProxyObject()
{
	for (INT i = 0; i < EditProxyObjectArray.Num(); ++i)
	{
		//UMMOSoundMaterialEditObject* obj = EditProxyObjectArray(i);
		//*obj->SoundMaterialGroupRef = obj->SoundMaterialGroup;
		//EditProxyObjectArray(i);
	}

	EditProxyObjectArray.Empty();
}
//------------------------------------------------------------------------
UMMOSoundMaterialEditObject* SoundMatEditState_HitSounds::FindEditProxyObject(
	BYTE SrcType, BYTE TargetType )
{
	for (INT j = 0; j < EditProxyObjectArray.Num(); ++j)
	{
		UMMOSoundMaterialEditObject* obj = EditProxyObjectArray(j);
		
		if((obj->HitType == SrcType)
			&& (obj->TargetArmorMatType == TargetType))
		{
			return obj;
		}
	}

	return 0;
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::EmptySelection()
{
	SelectedObject = 0;
	HighlightObject = 0;
}
//------------------------------------------------------------------------
void SoundMatEditState_HitSounds::SaveData()
{
	SoundMaterialEditor->MakePackageDirty();

	for (INT i = 0; i < EditProxyObjectArray.Num(); ++i)
	{
		UMMOSoundMaterialEditObject* obj = EditProxyObjectArray(i);
		FHitSoundNode* sndGrp = FindSoundMaterialGroupObject(
			obj->HitType, obj->TargetArmorMatType);
		if(sndGrp)
		{
			//sndGrp = obj->ToPlaySound;
			sndGrp->HitType = obj->HitType;
			sndGrp->TargetArmorMatType = obj->TargetArmorMatType;
			sndGrp->ToPlaySound = obj->ToPlaySound;
		}
		else
		{
			checkSlow(false && TEXT("Sound material type map not found!") );
		}
	}
}
//------------------------------------------------------------------------
FHitSoundNode* SoundMatEditState_HitSounds::FindSoundMaterialGroupObject(
	BYTE SrcType, BYTE TargetType )
{
	UGamePlayDataManager* DataMgr = SoundMaterialEditor->GetDataManager();
	for (INT j = 0; j < DataMgr->HitEffectList.Num(); ++j)
	{
		FHitSoundNode& hitCfg = DataMgr->HitEffectList(j);
		if((hitCfg.HitType == SrcType) && (hitCfg.TargetArmorMatType == TargetType))
		{
			return &hitCfg;
		}
	}

	return 0;
}
//------------------------------------------------------------------------

/*
HitType0=no type
HitType1=slash
HitType2=stab
HitType3=shoot
HitType4=bash
HitType5=brawl
HitType6=element
HitType7=Swing Smash


[EItemMaterialType]
EIMAT_Default=Default
EIMAT_Mat1=Metal
EIMAT_Mat2=Stone
EIMAT_Mat3=Crystal
EIMAT_Mat4=Wood
EIMAT_Mat5=Bone
EIMAT_Mat6=Flesh
EIMAT_Mat7=Debris
EIMAT_Mat8=Fluid
EIMAT_Mat9=Illusory
*/

FString SoundMatEditState_HitSounds::GetEnumEnglishName(UEnum* InObject, INT Idx)
{
	FString Ret;
	if (InObject == WeaponHitEnumObj)
	{
		switch(Idx)
		{
		case 0:
			Ret = FString(TEXT("UnDefine"));
			break;
		case 1:
			Ret = FString(TEXT("Chop"));
			break;
		case 2:
			Ret = FString(TEXT("Stab"));
			break;
		case 3:
			Ret = FString(TEXT("Shoot"));
			break;
		case 4:
			Ret = FString(TEXT("Strike"));
			break;
		case 5:
			Ret = FString(TEXT("Punch"));
			break;
		case 6:
			Ret = FString(TEXT("Magic"));
			break;
		case 7:
			Ret = FString(TEXT("Crash"));
			break;
		default:
			Ret = FString(TEXT("Error"));
		}
	}
	else if (InObject == MaterialTypeEnumObj)
	{
		switch(Idx)
		{
		case 0:
			Ret = FString("Undefine");
			break;
		case 1:
			Ret = FString("Metal");
			break;
		case 2:
			Ret = FString("Stone");
			break;
		case 3:
			Ret = FString("Crystal");
			break;
		case 4:
			Ret = FString("Wood");
			break;
		case 5:
			Ret = FString("Bone");
			break;
		case 6:
			Ret = FString("Flesh");
			break;
		case 7:
			Ret = FString("Smash");
			break;
		case 8:
			Ret = FString("Fluid");
			break;
		case 9:
			Ret = FString("Ghost");
			break;
		}
	}

	return Ret;
}