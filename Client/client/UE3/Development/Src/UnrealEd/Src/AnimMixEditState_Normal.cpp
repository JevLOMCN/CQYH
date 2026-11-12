/*=============================================================================
	hemu@snda 120416
	Animation blend parameter table, used for smooth return playback of animations such as combo actions
=============================================================================*/

#include "AnimMixEditState_Normal.h"
#include "UnLinkedObjDrawUtils.h"
#include "AnimMixMatrix.h"

#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

AnimMixEditState_Normal::AnimMixEditState_Normal( AnimMixMatrix* ed )
: AnimMixEditState(ed)
, SelectedObject(0)
, HighlightObject(0)
{

}
//------------------------------------------------------------------------
AnimMixEditState_Normal::~AnimMixEditState_Normal()
{

}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::DrawObjects( FViewport* Viewport, FCanvas* Canvas )
{
	for (INT i = 0; i < EnumObjTarget->NumEnums() - 1; ++i)
	{
		FLinkedObjDrawUtils::DrawTitleBar(Canvas, 
			FIntPoint(i * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE) + DRAW_X_OFFSET,
			DRAW_Y_OFFSET - DRAW_BLOCK_SIZE - DRAW_BLOCK_SPACE),
			FIntPoint(DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE), BorderColor, FColor(255,255,128),BkgColor,
			*GetEnumDisplayName(i,false), TArray<FString>());

	}
	for (INT i = 0; i <EnumObjSource ->NumEnums() - 1; ++i)
	{
		FLinkedObjDrawUtils::DrawTitleBar(Canvas, 
			FIntPoint(DRAW_X_OFFSET - DRAW_BLOCK_SIZE - DRAW_BLOCK_SPACE,
				i * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE) + DRAW_Y_OFFSET),
			FIntPoint(DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE), BorderColor, FColor(255,255,128),BkgColor,
			*GetEnumDisplayName(i), TArray<FString>());
	}

	for (INT x = 0; x < EAMT_MAX  ; ++x)
	{
		for (INT y = 0; y < EAMS_MAX ; ++y)
		{
			//Source ->Target
			UMMOAnimMixEditObject* EditProxyObject = FindEditProxyObject(y, x);
			if(Canvas->IsHitTesting())
			{
				Canvas->SetHitProxy(new HAnyObjectProxy(EditProxyObject, (int)EAO_Normal, 0));
			}

			FColor tBorderColor = BorderColor;
			FColor tBkgColor = BkgColor;

			if( EditProxyObject != NULL /*&&EditProxyObject->AnimMixInfo!=NULL*/ && EditProxyObject->AnimMixInfo.Anim.animation.GetIndex() >0 )
			{
				tBkgColor = GreenColor;
			}
			else 
			{
				//Data not yet filled in
				tBkgColor = WhiteColor ;
			}
			/*else
			{
				tBkgColor = YellowColor;
			}*/
			if( SelectedObject == EditProxyObject )
			{
				tBorderColor = CyanColor;
			}
			else if( HighlightObject == EditProxyObject )
			{
				tBorderColor = YellowColor;
			}
			else
			{
				tBorderColor = BorderColor;
			}

			FLinkedObjDrawUtils::DrawTitleBar(Canvas,
				FIntPoint(DRAW_X_OFFSET + x * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE),
				DRAW_Y_OFFSET + y * (DRAW_BLOCK_SIZE + DRAW_BLOCK_SPACE)),
				FIntPoint(DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE),
				tBorderColor, FColor(255,255,128),tBkgColor, TEXT(""), TArray<FString>());

			if(Canvas->IsHitTesting())
			{
				Canvas->SetHitProxy(0);
			}
		}
	}
	Canvas->PushAbsoluteTransform(FMatrix::Identity);
	
	INT msX = AnimMixEditor->LinkedObjVC->OldMouseX;
	INT msY = AnimMixEditor->LinkedObjVC->OldMouseY;
	UINT szX = AnimMixEditor->LinkedObjVC->Viewport->GetSizeX();
	UINT szY = AnimMixEditor->LinkedObjVC->Viewport->GetSizeY();
	
	DrawLine2D(Canvas, FVector2D(0, msY), FVector2D(szX, msY), FLinearColor(0, 0, 128));
	DrawLine2D(Canvas, FVector2D(msX, 0), FVector2D(msX, szY), FLinearColor(0, 0, 128));

	if(HighlightObject)
	{
		FString tipMsg = FString::Printf( TEXT("%s -> %s"),
			*GetEnumDisplayName(HighlightObject->AnimMixInfo.SourceAnim),
			*GetEnumDisplayName(HighlightObject->AnimMixInfo.TargetAnim,false));
		FLinkedObjDrawInfo drawInfo;
		drawInfo.ToolTips.AddItem(tipMsg);
		FLinkedObjDrawUtils::DrawToolTips(Canvas, drawInfo, FIntPoint(msX, msY), FIntPoint(100, 50));
	}

	Canvas->PopTransform();
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::OnEnter()
{
	_DestroyEditProxyObject();

	for (INT x = 0; x < EAMT_MAX; ++x)
	{
		for (INT y = 0; y < EAMS_MAX; ++y)
		{
			UMMOAnimMixEditObject* obj = Cast<UMMOAnimMixEditObject>(
				UObject::StaticConstructObject(UMMOAnimMixEditObject::StaticClass()));
			//obj->AnimMixGroupRef = 0;
			obj->AnimMixInfo.TargetAnim  = x;
			obj->AnimMixInfo.SourceAnim = y;
			FAnimMixNode* node = FindAnimNodeObject(obj->AnimMixInfo.SourceAnim, obj->AnimMixInfo.TargetAnim);
			if(node)				
			{
				obj->AnimMixInfo = *node;
			}
			EditProxyObjectArray.AddItem(obj);
		}
	}
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::OnExit()
{
	_DestroyEditProxyObject();
	SelectedObject = 0;
	HighlightObject = 0;
}
//Read from ArchiType
FAnimMixNode* AnimMixEditState_Normal::FindAnimNodeObject(BYTE ST, BYTE TT)
{
	UGamePlayDataManager* DataMgr = AnimMixEditor->GetDataManager();
	for (INT j = 0; j < DataMgr->AnimMixList.Num(); ++j)
	{
		FAnimMixNode* node = &DataMgr->AnimMixList(j);
		if((node->SourceAnim == ST) && (node->TargetAnim == TT))
		{
			return node;
		}
	}
	return 0;
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::ClickedAnyObject( struct HAnyObjectProxy* proxy )
{
	if(proxy->ObjectTypeFlag == EAO_Normal)
	{
		SelectedObject = (UMMOAnimMixEditObject*)proxy->AnyObject;
		if( !SelectedObject)
			return;
		FAnimMixNode* node = FindAnimNodeObject(SelectedObject->AnimMixInfo.SourceAnim, SelectedObject->AnimMixInfo.TargetAnim);
		if(!node)				
		{
			//UMMOAnimMixEditObject *NewNode=new UMMOAnimMixEditObject;//(EC_EventParm)
			//EC_EventParm needs to be added here for initialization
			FAnimMixNode NewNode(EC_EventParm);
			NewNode.SourceAnim = SelectedObject->AnimMixInfo.SourceAnim;
			NewNode.TargetAnim = SelectedObject->AnimMixInfo.TargetAnim;
			AnimMixEditor->GetDataManager()->AnimMixList.AddItem( NewNode );
			AnimMixEditor->MakePackageDirty();
		}
		else
		{
			//If the edit node in memory already has data, use the in-memory edit node data to prevent the data from being overwritten
			if( SelectedObject->AnimMixInfo.Anim.animation.GetIndex() <= 0)
			{
				node->Anim=SelectedObject->AnimMixInfo.Anim;
				AnimMixEditor->MakePackageDirty();
			}
			else
				SelectedObject->AnimMixInfo = *node;
		}
		AnimMixEditor->SetObjectToPropertyWindow(SelectedObject);
	}
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::HighlightAnyObject( struct HAnyObjectProxy* proxy )
{
	HighlightObject = 0;

	if(proxy && proxy->ObjectTypeFlag == EAO_Normal)
	{
		HighlightObject = (UMMOAnimMixEditObject*)proxy->AnyObject;
	}
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::_DestroyEditProxyObject()
{
	EditProxyObjectArray.Empty();
}
//------------------------------------------------------------------------
UMMOAnimMixEditObject* AnimMixEditState_Normal::FindEditProxyObject(
	BYTE SrcType, BYTE TargetType )
{
	for (INT j = 0; j < EditProxyObjectArray.Num(); ++j)
	{
		UMMOAnimMixEditObject* obj = EditProxyObjectArray(j);

		if((obj->AnimMixInfo.SourceAnim == SrcType)
			&& (obj->AnimMixInfo.TargetAnim == TargetType))
		{
			return obj;
		}
	}
	return 0;
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::EmptySelection()
{
	SelectedObject = 0;
	HighlightObject = 0;
}
//------------------------------------------------------------------------
void AnimMixEditState_Normal::SaveData()
{
	for (INT i = 0; i < EditProxyObjectArray.Num(); ++i)
	{
		UMMOAnimMixEditObject* obj = EditProxyObjectArray(i);
		FAnimMixNode* node = FindAnimNodeObject(
			obj->AnimMixInfo.SourceAnim, obj->AnimMixInfo.TargetAnim);
		if(node)
		{
			*node = obj->AnimMixInfo;
		}
		else
		{
			//Node can be empty
			//checkSlow(false && TEXT("Sound material type map not found!") );
		}
	}
}
