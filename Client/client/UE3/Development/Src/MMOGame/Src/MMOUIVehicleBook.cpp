#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"

// Mount book (1.05, UI_VehicleBook.upk).
//
// What the swf shows:
//   - tab 1: every mount (or only the owned ones), the selected / mounted mount
//   - tab 2: "beast power" - the sum of MountPower (mount.txt) of the owned mounts indexes mount_power.txt, which gives the
//            beast power level (Lap), the milestone nodes of that level and the attributes granted so far.
// The beast power attributes are only displayed here: this server does not apply them (it has no mount_power support).

IMPLEMENT_CLASS(UMMOUIVehicleBook);

// ------------------------------------------------------------------------------------------------------------------------
// mount_power.txt
struct FVBPowerRow
{
	INT		ID;
	INT		Lap;
	INT		SoulSlots;
	INT		Power;
	FString	NewNode;
	TArray<FString>	AttrType;
	TArray<INT>		AttrValue;
};

static TArray<FVBPowerRow>	g_VBRows;
static UBOOL				g_VBLoaded = FALSE;
static INT					g_VBMaxSoulSlots = 0;

static INT g_VBMode = 0;				// 0 = all mounts, 1 = owned only
static INT g_VBSelected = -1;			// mount id selected in the list
static INT g_VBPendingFrames = 0;		// frames left to wait for the swf to run configUI
static UBOOL g_VBDirty = FALSE;			// data changed while the panel was not ready

static void VB_LoadTables()
{
	if(g_VBLoaded)
	{
		return;
	}
	g_VBLoaded = TRUE;

	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("mount_power.txt"), TRUE ) || tabFile == NULL)
	{
		warnf(NAME_Log, TEXT("[VehicleBook] mount_power.txt could not be read"));
		return;
	}

	INT _ID		= tabFile->GetKeyColIdx( TEXT("ID"));
	INT _Lap	= tabFile->GetKeyColIdx( TEXT("Lap"));
	INT _Soul	= tabFile->GetKeyColIdx( TEXT("MaxSoulSlotCount"));
	INT _Power	= tabFile->GetKeyColIdx( TEXT("Power"));
	INT _Node	= tabFile->GetKeyColIdx( TEXT("NewNode"));
	INT _Type[6];
	INT _Value[6];
	for(INT k = 0; k < 6; ++k)
	{
		_Type[k]	= tabFile->GetKeyColIdx( *FString::Printf(TEXT("AttrType%d"), k + 1));
		_Value[k]	= tabFile->GetKeyColIdx( *FString::Printf(TEXT("AttrValue%d"), k + 1));
	}

	g_VBRows.Empty();
	for(INT i = 2; i < tabFile->GetRowCount(); ++i)
	{
		FVBPowerRow Row;
		Row.ID			= appAtoi(tabFile->GetData(_ID, i));
		Row.Lap			= appAtoi(tabFile->GetData(_Lap, i));
		Row.SoulSlots	= (_Soul >= 0) ? appAtoi(tabFile->GetData(_Soul, i)) : 0;
		Row.Power		= (_Power >= 0) ? appAtoi(tabFile->GetData(_Power, i)) : 0;
		Row.NewNode		= (_Node >= 0) ? FString(tabFile->GetData(_Node, i)) : FString();
		for(INT k = 0; k < 6; ++k)
		{
			FString Type = (_Type[k] >= 0) ? FString(tabFile->GetData(_Type[k], i)) : FString();
			INT Value = (_Value[k] >= 0) ? appAtoi(tabFile->GetData(_Value[k], i)) : 0;
			if(Type.Len() > 0 && Value != 0)
			{
				Row.AttrType.AddItem(Type);
				Row.AttrValue.AddItem(Value);
			}
		}
		g_VBMaxSoulSlots = Max(g_VBMaxSoulSlots, Row.SoulSlots);
		g_VBRows.AddItem(Row);
	}
	delete tabFile;
	warnf(NAME_Log, TEXT("[VehicleBook] mount_power.txt: %d rows"), g_VBRows.Num());
}

static FString VB_AttrLabel(const FString& Cat)
{
	if(Cat == TEXT("catMaxHp"))			return TEXT("Max HP");
	if(Cat == TEXT("catMaxMp"))			return TEXT("Max MP");
	if(Cat == TEXT("catPhyDef"))		return TEXT("Defense");
	if(Cat == TEXT("catMaxPhyDef"))		return TEXT("Max Defense");
	if(Cat == TEXT("catMagDef"))		return TEXT("Magic Defense");
	if(Cat == TEXT("catMaxMagDef"))		return TEXT("Max Magic Defense");

	// unknown attribute: "catSomeThing" -> "Some Thing"
	FString Out;
	INT Start = (Cat.Left(3) == TEXT("cat")) ? 3 : 0;
	for(INT i = Start; i < Cat.Len(); ++i)
	{
		TCHAR C = Cat[i];
		if(i > Start && C >= 'A' && C <= 'Z')
		{
			Out += TEXT(" ");
		}
		Out += FString::Printf(TEXT("%c"), C);
	}
	return Out;
}

// ownership: the facade keeps the synced mount list even when it arrives before the main player exists
static UBOOL VB_Owned(INT MountID)
{
	return MMOFacade::getSingleton().IsMountOwned(MountID);
}

// beast power points = sum of MountPower of the owned mounts
static INT VB_OwnedPoints()
{
	INT Points = 0;
	MountDataMap* pMap = MMOFacade::getSingleton().GetMountDataMap();
	for(MountDataMap::iterator it = pMap->begin(); it != pMap->end(); ++it)
	{
		if(it->second.ID > 0 && VB_Owned(it->second.ID))
		{
			Points += it->second.MountPower;
		}
	}
	return Points;
}

// the row that applies for this many points (the biggest ID that is not above it)
static const FVBPowerRow* VB_RowFor(INT Points)
{
	const FVBPowerRow* Best = NULL;
	for(INT i = 0; i < g_VBRows.Num(); ++i)
	{
		if(g_VBRows(i).ID <= Points && (Best == NULL || g_VBRows(i).ID > Best->ID))
		{
			Best = &g_VBRows(i);
		}
	}
	return Best;
}

// ------------------------------------------------------------------------------------------------------------------------
static UBOOL VB_EnsureConfigured(UMMOUIVehicleBook* Panel)
{
	// the CLIK panel runs configUI lazily (first render after loading); data sent before it is lost
	if(Panel->GetBool(TEXT("initialized")))
	{
		return TRUE;
	}
	TArray<struct FASValue> NoArgs;
	Panel->Invoke(TEXT("validateNow"), NoArgs);
	return Panel->GetBool(TEXT("initialized"));
}

static UMMOUIVehicleBook* VB_FindPanel()
{
	UMMOUIManager* UM = MMOUI::GetUIManager();
	return UM ? Cast<UMMOUIVehicleBook>(UM->FindUIPanel(FName(TEXT("VehicleBook")))) : NULL;
}

static INT VB_MountedID()
{
	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer && pMainPlayer->ActorAttributes)
	{
		return pMainPlayer->ActorAttributes->MountID;
	}
	return 0;
}

// ------------------------------------------------------------------------------------------------------------------------
// 3D preview. Same mechanism as MMOUIWingPanel / MMOUIMallEquipRolePanel: a copy of the main player is put into the
// "PawnShowLevel_Mall" show level (its scene capture renders into a render target) and the mount is equipped on that copy.
// The swf has no image for the render target, so the texture is drawn from the HUD (MMOUIVehicleBook_DrawPreview) into the
// transparent "vehicleShow" area of the window. The real player is never touched.
static AMMOPlayer*				g_VBShowPlayer = NULL;
static ULevelStreamingUIShow*	g_VBShowLevel = NULL;
static UWorld*					g_VBShowWorld = NULL;
static UBOOL					g_VBOpen = FALSE;
static INT						g_VBTurn = 0;			// -1 left, 1 right, 0 none
static INT						g_VBPreviewID = 0;
static INT						g_VBShadowPasses = 0;
static UBOOL					g_VBPlaced = FALSE;		// the clone stands where the scene capture looks
static INT						g_VBPlaceTries = 0;
static UBOOL					g_VBTitlePending = FALSE;
static UBOOL					g_VBBound = FALSE;		// render target bound to the swf image "VehicleAvatar"
static INT						g_VBBindDelay = 0;		// frames until the render target is bound (the handbook binds one frame after enabling its show level)

static void VB_CopyEquipment(AMMOPlayer* Show, AMMOPlayer* Main)
{
	if(Show == NULL || Main == NULL || Show->ItemManager == NULL || Main->ItemManager == NULL)
	{
		return;
	}
	for(INT i = 0; i < Main->ItemManager->EquippedItemList_Normal.Num(); i++)
	{
		UEquip* tEquip = Main->ItemManager->EquippedItemList_Normal(i);
		if(tEquip != NULL && tEquip->MeshList.Num() > 0)
		{
			const UItem* tEquipTemplate = MMOFacade::getSingleton().FindItemTemplate(tEquip->ItemId);
			if(tEquipTemplate)
			{
				Show->ItemManager->RequestTakeOn(-1, i, true, tEquipTemplate, false);
			}
		}
	}
	Show->ItemManager->UpdateOwnerData();
}

// the preview actor stands in the real world, so its dynamic shadow would show up on the ground in view
static void VB_NoShadow(AActor* A, INT Depth = 0)
{
	if(A == NULL || Depth > 2)
	{
		return;
	}
	for(INT i = 0; i < A->AllComponents.Num(); ++i)
	{
		UDynamicLightEnvironmentComponent* LE = Cast<UDynamicLightEnvironmentComponent>(A->AllComponents(i));
		if(LE && (LE->bCastShadows || LE->bCompositeShadowsFromDynamicLights))
		{
			LE->bCastShadows = FALSE;
			LE->bCompositeShadowsFromDynamicLights = FALSE;
			LE->BeginDeferredReattach();
		}
		UPrimitiveComponent* C = Cast<UPrimitiveComponent>(A->AllComponents(i));
		if(C && (C->CastShadow || C->bCastDynamicShadow))
		{
			C->CastShadow = FALSE;
			C->bCastDynamicShadow = FALSE;
			C->BeginDeferredReattach();
		}
	}
	for(INT i = 0; i < A->Attached.Num(); ++i)
	{
		VB_NoShadow(A->Attached(i), Depth + 1);
	}
}

// put the clone where the scene capture that owns our render target looks
static UBOOL VB_FixPlacement(UBOOL bLog)
{
	AMMOPlayer* D = g_VBShowPlayer;
	if(D == NULL || g_VBShowLevel == NULL || g_VBShowWorld != GWorld || GWorld == NULL)
	{
		return FALSE;
	}
	UObject* RT = (UObject*)g_VBShowLevel->RenderTarget;
	ASceneCapture2DActor* Owner = NULL;
	for(FActorIterator It; It; ++It)
	{
		AActor* A = *It;
		if(A == NULL || A == D || A->Location.Z > -15000.f || A->Location.Z < -30000.f)
		{
			continue;
		}
		ASceneCapture2DActor* SC = Cast<ASceneCapture2DActor>(A);
		if(SC)
		{
			USceneCapture2DComponent* C = Cast<USceneCapture2DComponent>(SC->SceneCapture);
			if(C && (UObject*)C->TextureTarget == RT && (Owner == NULL || C->bEnabled))
			{
				Owner = SC;
			}
		}
	}
	if(Owner == NULL)
	{
		if(bLog)
		{
			warnf(NAME_Log, TEXT("[VehicleBook] placement: no scene capture owns the render target yet"));
		}
		return FALSE;
	}
	APlayerStart* Spot = NULL;
	FLOAT Best = 1e9f;
	for(FActorIterator It2; It2; ++It2)
	{
		APlayerStart* PS = Cast<APlayerStart>(*It2);
		if(PS && Abs(PS->Location.Z - Owner->Location.Z) < 400.f)
		{
			const FLOAT d = (PS->Location - Owner->Location).Size();
			if(d < Best)
			{
				Best = d;
				Spot = PS;
			}
		}
	}
	if(Spot == NULL)
	{
		if(bLog)
		{
			warnf(NAME_Log, TEXT("[VehicleBook] placement: no PlayerStart near the capture at (%.0f,%.0f,%.0f)"), Owner->Location.X, Owner->Location.Y, Owner->Location.Z);
		}
		return FALSE;
	}
	const FVector Want(Spot->Location.X, Spot->Location.Y, Owner->Location.Z);
	if((D->Location - Want).Size() > 300.f)
	{
		D->SetLocation(Want);
	}
	if(bLog)
	{
		warnf(NAME_Log, TEXT("[VehicleBook] placement: capture=(%.0f,%.0f,%.0f) spot=(%.0f,%.0f,%.0f) clone=(%.0f,%.0f,%.0f) dist=%.0f"),
			Owner->Location.X, Owner->Location.Y, Owner->Location.Z, Spot->Location.X, Spot->Location.Y, Spot->Location.Z,
			D->Location.X, D->Location.Y, D->Location.Z, (D->Location - Owner->Location).Size());
	}
	return TRUE;
}

// The capture leaves alpha 0 on every pixel, so the swf (which blends the image by its alpha) would draw it invisible. The
// preview is therefore copied to a dynamic texture whose alpha is made from the colour (the capture background is black) and
// that texture is bound to the swf image instead of the render target.
// (the GFx image wrapper only accepts UTexture2D and render targets, so this is a plain UTexture2D whose RHI texture is rewritten)
static UTexture2D* g_VBAlphaTex = NULL;

struct FVBTexUpdate
{
	FTexture2DResource*	Resource;
	INT					W;
	INT					H;
	TArray<BYTE>		Data;
};

// Layout of the swf image "VehicleAvatar" (UI_VehicleBook.swf): the image is placed at the origin of the model window and cropped by
// a mask of x 3..509, y 42..428 (506 x 386). The engine scales a replacement texture by (original image size / texture size), the
// original counts as 1024 x 1024 (measured with a calibration grid: a 512 texture came out exactly 2x too big), so a 1024 x 1024
// texture is shown 1:1. The preview (square) is scaled to the width of the mask and centred in it.
#define VB_TEX_SIZE		1024
#define VB_MASK_X		3
#define VB_MASK_Y		42
#define VB_MASK_W		506
#define VB_MASK_H		386

static UBOOL VB_UpdateAlphaTex()
{
	if(g_VBShowLevel == NULL || g_VBShowLevel->RenderTarget == NULL)
	{
		return FALSE;
	}
	UTextureRenderTarget2D* RTex = g_VBShowLevel->RenderTarget;
	FTextureRenderTargetResource* Res = RTex->GameThread_GetRenderTargetResource();
	TArray<FColor> Pix;
	if(Res == NULL || !Res->ReadPixels(Pix) || Pix.Num() <= 0)
	{
		return FALSE;
	}
	const INT W = (INT)RTex->GetSurfaceWidth();
	const INT H = (INT)RTex->GetSurfaceHeight();
	if(W <= 0 || H <= 0 || W * H != Pix.Num())
	{
		return FALSE;
	}
	if(g_VBAlphaTex == NULL)
	{
		g_VBAlphaTex = ConstructObject<UTexture2D>(UTexture2D::StaticClass(), UObject::GetTransientPackage());
		if(g_VBAlphaTex == NULL)
		{
			return FALSE;
		}
		g_VBAlphaTex->AddToRoot();
		g_VBAlphaTex->SRGB = FALSE;
		g_VBAlphaTex->NeverStream = TRUE;
		g_VBAlphaTex->CompressionNone = TRUE;
		g_VBAlphaTex->Init(VB_TEX_SIZE, VB_TEX_SIZE, PF_A8R8G8B8);
		g_VBAlphaTex->UpdateResource();
		g_VBBound = FALSE;		// bind the new texture
	}
	if(g_VBAlphaTex->Resource == NULL)
	{
		return FALSE;
	}

	// the preview (square), scaled to the mask width so the whole width of the capture is used, centred vertically in the mask
	// (bilinear), alpha made from the colour. Rows outside the mask are not needed.
	const INT Side = VB_MASK_W;
	const INT DestX0 = VB_MASK_X;
	const INT DestY0 = VB_MASK_Y + VB_MASK_H / 2 - Side / 2;
	TArray<BYTE> Data;
	Data.AddZeroed(VB_TEX_SIZE * VB_TEX_SIZE * 4);
	for(INT dy = 0; dy < Side; ++dy)
	{
		const INT ty = DestY0 + dy;
		if(ty < VB_MASK_Y || ty >= VB_MASK_Y + VB_MASK_H)
		{
			continue;
		}
		const FLOAT sy = ((FLOAT)dy + 0.5f) * (FLOAT)H / (FLOAT)Side - 0.5f;
		const INT y0 = Clamp((INT)appFloor(sy), 0, H - 1);
		const INT y1 = Min(y0 + 1, H - 1);
		const FLOAT fy = Clamp(sy - (FLOAT)y0, 0.f, 1.f);
		for(INT dx = 0; dx < Side; ++dx)
		{
			const FLOAT sx = ((FLOAT)dx + 0.5f) * (FLOAT)W / (FLOAT)Side - 0.5f;
			const INT x0 = Clamp((INT)appFloor(sx), 0, W - 1);
			const INT x1 = Min(x0 + 1, W - 1);
			const FLOAT fx = Clamp(sx - (FLOAT)x0, 0.f, 1.f);
			const FColor& C00 = Pix(y0 * W + x0);
			const FColor& C10 = Pix(y0 * W + x1);
			const FColor& C01 = Pix(y1 * W + x0);
			const FColor& C11 = Pix(y1 * W + x1);
			const FLOAT w00 = (1.f - fx) * (1.f - fy), w10 = fx * (1.f - fy), w01 = (1.f - fx) * fy, w11 = fx * fy;
			const FLOAT R = C00.R * w00 + C10.R * w10 + C01.R * w01 + C11.R * w11;
			const FLOAT G = C00.G * w00 + C10.G * w10 + C01.G * w01 + C11.G * w11;
			const FLOAT B = C00.B * w00 + C10.B * w10 + C01.B * w01 + C11.B * w11;
			const FLOAT M = Max(Max(R, G), B);
			BYTE* Dst = &Data(((DestY0 + dy) * VB_TEX_SIZE + (DestX0 + dx)) * 4);
			Dst[0] = (BYTE)Clamp(appTrunc(B + 0.5f), 0, 255);
			Dst[1] = (BYTE)Clamp(appTrunc(G + 0.5f), 0, 255);
			Dst[2] = (BYTE)Clamp(appTrunc(R + 0.5f), 0, 255);
			Dst[3] = (BYTE)Clamp(appTrunc(M * 6.f), 0, 255);
		}
	}

	FVBTexUpdate* Upd = new FVBTexUpdate();
	Upd->Resource = (FTexture2DResource*)g_VBAlphaTex->Resource;
	Upd->W = VB_TEX_SIZE;
	Upd->H = VB_TEX_SIZE;
	Upd->Data = Data;
	// rewrite mip 0 of the texture on the render thread
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		FVBUpdateTex,
		FVBTexUpdate*, Upd, Upd,
		{
			UINT Pitch = 0;
			void* Locked = RHILockTexture2D(Upd->Resource->GetTexture2DRHI(), 0, TRUE, Pitch, FALSE);
			if(Locked != NULL)
			{
				const INT RowBytes = Upd->W * 4;
				for(INT y = 0; y < Upd->H; ++y)
				{
					appMemcpy((BYTE*)Locked + y * (INT)Pitch, &Upd->Data(y * RowBytes), RowBytes);
				}
				RHIUnlockTexture2D(Upd->Resource->GetTexture2DRHI(), 0, FALSE);
			}
			delete Upd;
		});
	return TRUE;
}

static void VB_StopShow()
{
	g_VBOpen = FALSE;
	g_VBTurn = 0;
	g_VBPreviewID = 0;
	g_VBShadowPasses = 0;
	if(g_VBShowLevel)
	{
		g_VBShowLevel->SetEnabled(FALSE);
	}
	if(g_VBShowPlayer && g_VBShowWorld == GWorld && GWorld != NULL)
	{
		GWorld->DestroyActor(g_VBShowPlayer);
	}
	g_VBShowPlayer = NULL;
	g_VBShowLevel = NULL;
}

static void VB_StartShow(UMMOUIVehicleBook* Panel)
{
	if(Panel == NULL || Panel->GFXPlayer == NULL)
	{
		return;
	}
	AMMOPlayer* Main = MMOFacade::getSingleton().GetMainPlayer();
	if(Main == NULL || Main->ActorAttributes == NULL)
	{
		return;
	}
	if(g_VBShowWorld != GWorld)
	{
		g_VBShowPlayer = NULL;		// the old world (and the actor) is gone
		g_VBShowLevel = NULL;
		g_VBShowWorld = GWorld;
	}
	if(g_VBShowLevel == NULL)
	{
		g_VBShowLevel = Panel->GFXPlayer->eventGetShowLevel(TEXT("PawnShowLevel_Mall"));
	}
	if(g_VBShowLevel == NULL || g_VBShowLevel->RenderTarget == NULL)
	{
		warnf(NAME_Log, TEXT("[VehicleBook] preview: show level PawnShowLevel_Mall=%d renderTarget=%d"), g_VBShowLevel != NULL,
			g_VBShowLevel != NULL && g_VBShowLevel->RenderTarget != NULL);
		g_VBShowLevel = NULL;
		return;
	}
	g_VBShowLevel->SetEnabled(TRUE);

	// the swf has an image "VehicleAvatar" for the model (the handbook binds "HandbookAvatar" the same way, a frame later)
	g_VBBound = FALSE;
	g_VBBindDelay = 3;

	if(g_VBShowPlayer == NULL)
	{
		g_VBShowPlayer = Panel->eventClonePlayer(Main);
		if(g_VBShowPlayer)
		{
			g_VBShowPlayer->PawnAnimInfo = Main->PawnAnimInfo;
		}
	}
	AMMOPlayer* Show = g_VBShowPlayer;
	if(Show == NULL)
	{
		return;
	}
	Show->NativeSetMesh(Main->GetClassId(), Main->GetSex());
	Show->UnEquipMount();
	VB_CopyEquipment(Show, Main);
	Show->DrawScale = 0.8f;

	FName tag(TEXT("C_01"), FNAME_Find);
	if(Show->UseingAvatar)
	{
		tag = Show->UseingAvatar->PlacementNodeTag;
	}
	if(Show->LightEnvironment)
	{
		Show->LightEnvironment->SetEnabled(FALSE);
	}
	if(Show->Mesh)
	{
		Show->Mesh->bUpdateSkelWhenNotRendered = TRUE;
		Show->Mesh->bIgnoreControllersWhenNotRendered = FALSE;
	}
	g_VBShowLevel->SetShowActor(Show, tag);
	// the show level streams in asynchronously, so the scene capture may not exist yet: placing is retried from the tick
	g_VBPlaced = FALSE;
	g_VBPlaceTries = 0;
	g_VBPreviewID = 0;
	g_VBShadowPasses = 0;
	g_VBOpen = TRUE;
}

// mount selected in the list: show it on the preview actor
static void VB_ApplyPreview(INT MountID)
{
	if(g_VBShowPlayer == NULL || g_VBShowWorld != GWorld || MountID <= 0 || g_VBPreviewID == MountID)
	{
		return;
	}
	g_VBPreviewID = MountID;
	g_VBShowPlayer->EquipMount(MountID);
	g_VBShowPlayer->HideWeapon(TRUE);
	g_VBShowPlayer->DrawScale = 0.8f;
	g_VBShadowPasses = 0;
}

// The swf's "Use" button is the ride skill button of the 1.05 client. This server has no ride skills, so the button rides /
// dismounts the selected mount instead.
static void VB_UpdateUseButton(UMMOUIVehicleBook* Panel)
{
	MountData* pMount = (g_VBSelected > 0) ? MMOFacade::getSingleton().GetMountData(g_VBSelected) : NULL;
	const UBOOL bOwned = pMount != NULL && VB_Owned(pMount->ID);
	Panel->eventCallSetRideSkillLearned(bOwned);
	if(UGFxObject* Av = Panel->GetObject(TEXT("mw_vehicleAvatar")))
	{
		if(UGFxObject* Btn = Av->GetObject(TEXT("btn_useRideSkill")))
		{
			Btn->SetString(TEXT("label"), (bOwned && VB_MountedID() == g_VBSelected) ? TEXT("Dismount") : TEXT("Ride"));
		}
	}
}

// drawn from AMMOHud::DrawHUD (before the GFx movies are drawn): the render target goes into the transparent preview area
void MMOUIVehicleBook_DrawPreview(UCanvas* Canvas)
{
	if(Canvas == NULL || !g_VBOpen || g_VBShowLevel == NULL || g_VBShowLevel->RenderTarget == NULL || g_VBShowWorld != GWorld)
	{
		return;
	}
	if(g_VBBound)
	{
		return;
	}
	UMMOUIVehicleBook* P = VB_FindPanel();
	if(P == NULL || !P->GetBool(TEXT("visible")))
	{
		return;
	}
	UGFxObject* Area = P->GetObject(TEXT("vehicleShow"));
	if(Area == NULL || !Area->GetBool(TEXT("visible")))
	{
		return;		// the beast power tab is open
	}
	// the dark model area of UI_VehicleBook.swf in panel pixels (the swf clip itself reports a bogus size)
	const FLOAT HoleX = P->GetFloat(TEXT("x")) + 34.f;
	const FLOAT HoleY = P->GetFloat(TEXT("y")) + 74.f;
	const FLOAT HoleW = 508.f;
	const FLOAT HoleH = 390.f;
	const FLOAT Side = Min(HoleW, HoleH);		// the render target is square: keep the aspect, centred
	const FLOAT X0 = HoleX + (HoleW - Side) * 0.5f;
	const FLOAT Y0 = HoleY + (HoleH - Side) * 0.5f;
	UTexture* Tex = g_VBShowLevel->RenderTarget;
	static INT s_Logged = 0;
	if(s_Logged++ % 900 == 0)
	{
		warnf(NAME_Log, TEXT("[VehicleBook] DrawPreview: panel=(%.0f,%.0f) area=(%.0f,%.0f %.0fx%.0f) tile=(%.0f,%.0f %.0f) tex=%.0fx%.0f"),
			P->GetFloat(TEXT("x")), P->GetFloat(TEXT("y")), HoleX, HoleY, HoleW, HoleH, X0, Y0, Side, (FLOAT)Tex->GetSurfaceWidth(), (FLOAT)Tex->GetSurfaceHeight());
	}

	// drag with the left mouse button inside the area to turn the preview actor
	{
		static UBOOL s_Dragging = FALSE;
		static FLOAT s_LastX = 0.f;
		ULocalPlayer* LP = (GEngine && GEngine->GamePlayers.Num() > 0) ? GEngine->GamePlayers(0) : NULL;
		if(LP && LP->ViewportClient && g_VBShowPlayer != NULL)
		{
			const FVector2D Mouse = LP->ViewportClient->GetMousePosition();
			const UBOOL bDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
			if(!bDown)
			{
				s_Dragging = FALSE;
			}
			else if(!s_Dragging)
			{
				s_Dragging = Mouse.X >= HoleX && Mouse.X <= HoleX + HoleW && Mouse.Y >= HoleY && Mouse.Y <= HoleY + HoleH;
			}
			else
			{
				FRotator R = g_VBShowPlayer->Rotation;
				R.Yaw += appTrunc((Mouse.X - s_LastX) * 60.f);
				g_VBShowPlayer->SetRotation(R);
			}
			s_LastX = Mouse.X;
		}
	}

	// dark backdrop over the whole area; the render target (black background) goes on top so only the model shows.
	// Z must be >= 1 (a tile with Z = 0 is not drawn)
	if(Canvas->DefaultTexture)
	{
		Canvas->DrawTile(Canvas->DefaultTexture, HoleX, HoleY, 1.f, HoleW, HoleH, 0.f, 0.f, 1.f, 1.f, FLinearColor(0.f, 0.f, 0.f, 1.f), BLEND_Opaque);
	}
	Canvas->DrawTile(Tex, X0, Y0, 1.f, Side, Side, 0.f, 0.f, Tex->GetSurfaceWidth(), Tex->GetSurfaceHeight(), FLinearColor(1.f, 1.f, 1.f, 1.f), BLEND_Opaque);
}

static void VB_FeedLocalization(UMMOUIVehicleBook* Panel)
{
	if(Panel->GFXPlayer == NULL)
	{
		return;
	}
	UGFxObject* Loc = Panel->GFXPlayer->CreateObject(TEXT("Object"));
	if(Loc == NULL)
	{
		return;
	}

	UGFxObject* Titles = Panel->GFXPlayer->CreateArray();
	if(Titles)
	{
		Titles->SetElementString(0, TEXT("Mounts"));
		Titles->SetElementString(1, TEXT("Power"));
		Loc->SetObject(TEXT("titleTextArray"), Titles);
	}
	Loc->SetString(TEXT("vehicle"),				TEXT(""));
	Loc->SetString(TEXT("speedUp"),				TEXT(""));
	Loc->SetString(TEXT("vehicleList"),			TEXT("My Mounts"));
	Loc->SetString(TEXT("curSelVehicle"),		TEXT("%s"));
	Loc->SetString(TEXT("notOwnVehicle"),		TEXT("Not Obtained"));
	Loc->SetString(TEXT("showAll"),				TEXT("All"));
	Loc->SetString(TEXT("showOwn"),				TEXT("Owned"));
	Loc->SetString(TEXT("vehiclePowerLevel"),	TEXT("Beast Power Lv.: "));
	Loc->SetString(TEXT("vehiclePowerTitle"),	TEXT("Beast Stats"));
	Loc->SetString(TEXT("vehiclePowerTips"),	TEXT("Beast Power: "));
	Loc->SetString(TEXT("itemSource"),			TEXT(""));
	Loc->SetString(TEXT("notLearned"),			TEXT("Not owned"));
	Loc->SetString(TEXT("useRideSkill"),		TEXT("Ride"));
	Panel->eventCallSetLocalizationData(Loc);

	if(g_VBTitlePending)
	{
		g_VBTitlePending = FALSE;
		TArray<struct FASValue> TitleArgs;
		FASValue TitleValue(EC_EventParm);
		TitleValue.Type = AS_String;
		TitleValue.S = TEXT("Mounts");
		TitleArgs.AddItem(TitleValue);
		Panel->Invoke(TEXT("SetWindowTitle"), TitleArgs);
	}
}

// ------------------------------------------------------------------------------------------------------------------------
static void VB_FeedPower(UMMOUIVehicleBook* Panel)
{
	if(Panel->GFXPlayer == NULL)
	{
		return;
	}
	VB_LoadTables();
	if(g_VBRows.Num() == 0)
	{
		return;
	}

	const INT Points = VB_OwnedPoints();
	const FVBPowerRow* Row = VB_RowFor(Points);
	if(Row == NULL)
	{
		return;
	}

	// beast power level (Lap) shown: level 1 until the first points are collected
	const INT Lap = Max(Row->Lap, 1);
	INT LapFirst = MAXINT, LapLast = -1;
	for(INT i = 0; i < g_VBRows.Num(); ++i)
	{
		if(g_VBRows(i).Lap == Lap)
		{
			LapFirst = Min(LapFirst, g_VBRows(i).ID);
			LapLast = Max(LapLast, g_VBRows(i).ID);
		}
	}
	if(LapLast < LapFirst)
	{
		return;
	}
	const INT IndexMax = LapLast - LapFirst + 1;
	const INT IndexCur = Clamp(Min(Points, LapLast) - LapFirst + 1, 0, IndexMax);

	// milestone nodes of this level
	UGFxObject* Nodes = Panel->GFXPlayer->CreateArray();
	INT NodeCount = 0;
	for(INT i = 0; i < g_VBRows.Num(); ++i)
	{
		const FVBPowerRow& R = g_VBRows(i);
		if(R.Lap == Lap && R.NewNode.Len() > 0 && Nodes)
		{
			UGFxObject* N = Panel->GFXPlayer->CreateObject(TEXT("Object"));
			if(N)
			{
				N->SetInt(TEXT("IndexReward"), R.ID - LapFirst + 1);
				N->SetString(TEXT("AttrStr"), R.NewNode);
				Nodes->SetElementObject(NodeCount++, N);
			}
		}
	}

	UGFxObject* Data = Panel->GFXPlayer->CreateObject(TEXT("Object"));
	if(Data)
	{
		Data->SetInt(TEXT("Lap"), Lap);
		Data->SetInt(TEXT("IndexMax"), IndexMax);
		Data->SetInt(TEXT("IndexCur"), IndexCur);
		if(Nodes)
		{
			Data->SetObject(TEXT("RewardDataList"), Nodes);
		}
		Panel->eventCallUpdateMountPowerData(Data);
	}

	// attributes granted so far
	UGFxObject* Attrs = Panel->GFXPlayer->CreateArray();
	if(Attrs)
	{
		for(INT i = 0; i < Row->AttrType.Num(); ++i)
		{
			UGFxObject* A = Panel->GFXPlayer->CreateObject(TEXT("Object"));
			if(A)
			{
				A->SetString(TEXT("AttrName"), VB_AttrLabel(Row->AttrType(i)));
				A->SetInt(TEXT("AttrValue"), Row->AttrValue(i));
				Attrs->SetElementObject(i, A);
			}
		}
		Panel->eventCallUpdateMountPowerAttrTotal(Attrs);
	}

	// beast (soul) slots that are unlocked at this level
	UGFxObject* Slots = Panel->GFXPlayer->CreateArray();
	if(Slots)
	{
		for(INT i = 0; i < g_VBMaxSoulSlots; ++i)
		{
			UGFxObject* S = Panel->GFXPlayer->CreateObject(TEXT("Object"));
			if(S)
			{
				S->SetBool(TEXT("isOpened"), i < Row->SoulSlots);
				S->SetBool(TEXT("visible"), TRUE);
				S->SetString(TEXT("iconName"), TEXT(""));
				S->SetInt(TEXT("slotType"), ESPType_Mount);
				S->SetInt(TEXT("itemIndex"), 0);
				S->SetInt(TEXT("slotID"), i);
				Slots->SetElementObject(i, S);
			}
		}
		Panel->eventCallUpdateMountSoul(Slots);
	}
}

static void VB_FeedList(UMMOUIVehicleBook* Panel)
{
	if(Panel->GFXPlayer == NULL)
	{
		return;
	}

	UGFxObject* DataProvider = Panel->GFXPlayer->CreateArray();
	if(DataProvider == NULL)
	{
		return;
	}

	INT Count = 0;
	INT FirstID = -1;
	UBOOL bSelectedShown = FALSE;
	MountDataMap* pMountMap = MMOFacade::getSingleton().GetMountDataMap();
	for(MountDataMap::iterator it = pMountMap->begin(); it != pMountMap->end(); ++it)
	{
		MountData* pMount = &(it->second);
		if(pMount->ID <= 0 || pMount->Name.Len() == 0 || !pMount->DataVisible)
		{
			continue;
		}
		if(g_VBMode == 1 && !VB_Owned(pMount->ID))
		{
			continue;
		}

		// the slot renderer casts its data to ActionScript3.Mount.MountSlotData, it has to be that class
		UGFxObject* SlotObj = Panel->GFXPlayer->CreateObject(TEXT("ActionScript3.Mount.MountSlotData"));
		if(SlotObj == NULL)
		{
			continue;
		}
		SlotObj->SetBool(TEXT("HaveData"), TRUE);
		SlotObj->SetInt(TEXT("EquipID"), ESBarSlotTp_Mount);
		SlotObj->SetInt(TEXT("SlotType"), ESPType_Mount);
		SlotObj->SetInt(TEXT("slotType"), ESPType_Mount);
		SlotObj->SetInt(TEXT("slotIndex"), Count);
		SlotObj->SetInt(TEXT("itemTemplateID"), pMount->ID);
		SlotObj->SetInt(TEXT("slotID"), pMount->ID);
		SlotObj->SetInt(TEXT("containerID"), E_CONSLOT_MAX);
		const UBOOL bHave = VB_Owned(pMount->ID);
		SlotObj->SetInt(TEXT("Have"), bHave ? 1 : 0);
		SlotObj->SetBool(TEXT("isLocked"), !bHave);
		SlotObj->SetInt(TEXT("IsEnable"), 1);
		SlotObj->SetInt(TEXT("classesLevel"), pMount->LevelLimit);
		SlotObj->SetInt(TEXT("classesQuality"), pMount->Quality);
		SlotObj->SetInt(TEXT("itemColor"), pMount->Quality);
		SlotObj->SetInt(TEXT("speedUp"), pMount->InUiSpeed);
		SlotObj->SetString(TEXT("slotName"), pMount->Name);
		SlotObj->SetString(TEXT("iconName"), FString::Printf(TEXT("img://%s"), *pMount->IconName));
		SlotObj->SetString(TEXT("tipsHtml"), pMount->Script);

		if(FirstID < 0)
		{
			FirstID = pMount->ID;
		}
		if(pMount->ID == g_VBSelected)
		{
			bSelectedShown = TRUE;
		}
		DataProvider->SetElementObject(Count++, SlotObj);
	}

	if(!bSelectedShown)
	{
		g_VBSelected = FirstID;
	}

	Panel->eventCallSetVehicleInfoShowMode(g_VBMode);
	Panel->eventCallUpdateVehicleList(DataProvider);
	Panel->eventCallSetMountedVehicle(VB_MountedID());
	if(g_VBSelected > 0)
	{
		Panel->eventCallSetCurSelectedVehicle(g_VBSelected);
	}
	VB_ApplyPreview(g_VBSelected);
	VB_UpdateUseButton(Panel);
}

// ------------------------------------------------------------------------------------------------------------------------
void UMMOUIVehicleBook::Init()
{
	Super::Init();
}

void UMMOUIVehicleBook::UnInit()
{
	Super::UnInit();
}

void UMMOUIVehicleBook::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		VB_LoadTables();
		g_VBMode = 0;
		g_VBSelected = VB_MountedID() > 0 ? VB_MountedID() : -1;
		g_VBPendingFrames = 240;		// feed again from the tick once the swf has run configUI
		g_VBDirty = TRUE;
		VB_StartShow(this);
		g_VBTitlePending = TRUE;

		if(VB_EnsureConfigured(this))
		{
			RefreshVehicleBook();
		}
	}
	else
	{
		g_VBPendingFrames = 0;
		VB_StopShow();
	}
}

INT UMMOUIVehicleBook::GetSlotType()
{
	return 0;
}

UBOOL UMMOUIVehicleBook::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

UBOOL UMMOUIVehicleBook::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

UBOOL UMMOUIVehicleBook::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

UBOOL UMMOUIVehicleBook::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}

void UMMOUIVehicleBook::RefreshVehicleBook()
{
	if(GFXPlayer == NULL)
	{
		return;
	}
	VB_FeedLocalization(this);
	VB_FeedList(this);
	VB_FeedPower(this);
	g_VBDirty = FALSE;
}

// ------------------------------------------------------------------------------------------------------------------------
// swf -> game
void UMMOUIVehicleBook::ASClickMountSlot(INT MountID)
{
	g_VBSelected = MountID;
	VB_ApplyPreview(MountID);
	VB_UpdateUseButton(this);
}

void UMMOUIVehicleBook::ASSetInfoShowMode(INT Mode)
{
	g_VBMode = (Mode == 1) ? 1 : 0;
	VB_FeedList(this);
}

void UMMOUIVehicleBook::ASMountVehicle(INT MountID)
{
	MountData* pMount = MMOFacade::getSingleton().GetMountData(MountID);
	if(pMount && VB_Owned(MountID))
	{
		CMessageHandler::GetInstance().Send_EquipMount(MountID);
	}
}

void UMMOUIVehicleBook::ASUnMountVehicle()
{
	CMessageHandler::GetInstance().Send_EquipMount(0);
}

void UMMOUIVehicleBook::ASModelRotate(INT Direction)
{
	g_VBTurn = (Direction == 0) ? -1 : 1;
}

void UMMOUIVehicleBook::ASReleaseModelRotate()
{
	g_VBTurn = 0;
}

void UMMOUIVehicleBook::ASShowMountGuide()
{
}

void UMMOUIVehicleBook::AsUseRideSkill()
{
	// ride / dismount the selected mount (see VB_UpdateUseButton)
	if(g_VBSelected > 0 && VB_MountedID() == g_VBSelected)
	{
		ASUnMountVehicle();
	}
	else if(g_VBSelected > 0)
	{
		ASMountVehicle(g_VBSelected);
	}
}

void UMMOUIVehicleBook::ASRideSkillGetPath()
{
}

// ------------------------------------------------------------------------------------------------------------------------
// network / facade: ownership or the mounted mount changed
void MMOUIVehicleBook_Refresh()
{
	UMMOUIVehicleBook* Panel = VB_FindPanel();
	if(Panel && Panel->GetVisible() && Panel->GetBool(TEXT("initialized")))
	{
		Panel->RefreshVehicleBook();
	}
}

// called every frame from UMMOGFXPlayer_RootBase::PostAdvance
void MMOUIVehicleBook_Tick()
{
	if(g_VBOpen && g_VBBindDelay > 0 && --g_VBBindDelay == 0)
	{
		UMMOUIVehicleBook* BindPanel = VB_FindPanel();
		if(BindPanel && BindPanel->GFXPlayer && g_VBShowLevel && g_VBShowLevel->RenderTarget && g_VBShowWorld == GWorld)
		{
			if(VB_UpdateAlphaTex() && g_VBAlphaTex)
			{
				g_VBBound = BindPanel->GFXPlayer->SetExternalTexture(TEXT("VehicleAvatar"), g_VBAlphaTex);
				warnf(NAME_Log, TEXT("[VehicleBook] preview: SetExternalTexture('VehicleAvatar', alpha copy)=%d"), g_VBBound);
			}
			else
			{
				g_VBBindDelay = 3;		// retry
			}
		}
	}
	if(g_VBOpen && g_VBBound && g_VBShowWorld == GWorld)
	{
		static INT s_AlphaCounter = 0;
		if(++s_AlphaCounter % 3 == 0)
		{
			VB_UpdateAlphaTex();
		}
	}
	if(g_VBOpen && g_VBShowPlayer != NULL && g_VBShowWorld == GWorld && !g_VBPlaced && g_VBPlaceTries < 900)
	{
		if(++g_VBPlaceTries % 10 == 1)
		{
			if(VB_FixPlacement(g_VBPlaceTries % 100 == 1))
			{
				g_VBPlaced = TRUE;
				// three quarter view, like the mall's equip preview
				FRotator Rot = g_VBShowPlayer->Rotation;
				Rot.Yaw -= 10000;
				g_VBShowPlayer->SetRotation(Rot);
				warnf(NAME_Log, TEXT("[VehicleBook] preview placed after %d frames"), g_VBPlaceTries);
			}
		}
	}
	if(g_VBOpen && g_VBShowPlayer != NULL && g_VBShowWorld == GWorld)
	{
		static INT s_ShadowCounter = 0;
		if(g_VBShadowPasses < 4 && ++s_ShadowCounter % 40 == 0)
		{
			VB_NoShadow(g_VBShowPlayer);
			++g_VBShadowPasses;
		}
		if(g_VBTurn != 0)
		{
			FRotator R = g_VBShowPlayer->Rotation;
			R.Yaw += g_VBTurn * 350;
			g_VBShowPlayer->SetRotation(R);
		}
	}
	if(g_VBPendingFrames <= 0)
	{
		return;
	}
	UMMOUIVehicleBook* Panel = VB_FindPanel();
	if(Panel == NULL || !Panel->GetVisible())
	{
		g_VBPendingFrames = 0;
		return;
	}
	--g_VBPendingFrames;
	if(!VB_EnsureConfigured(Panel))
	{
		return;
	}
	g_VBTitlePending = TRUE;
	Panel->RefreshVehicleBook();
	g_VBPendingFrames = 0;
}
