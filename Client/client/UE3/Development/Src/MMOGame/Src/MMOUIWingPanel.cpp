#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"


// 1.05 wing panel ("UnrealWing", UI_UnrealWing.upk), mirrors MMOUIMountPanel.
//
// UnrealWing.swf calls into this class: ASClickWingList / AsClickWing / ASClickWingUpgrade / ASHasNoWingTip /
// ASRotateWingMesh / ASRotateWingMeshRelease / ASSwitchBackShowType / ASShowTipsByType (declared in MMOUIWingPanel.uc)
// and is fed through UCUpdateWingData / UCSetLocalizationData / UCOnWingUpgrade (CallUpdateWingData ... events).
//
// Data: wings.txt (list), wings_level.txt (upgrade cost / bonus), g2c_wing_info (wing level, owned wings).
// The worn wing is ActorAttributes->WingID of the main player (g2c_char_wing).

IMPLEMENT_CLASS(UMMOUIWingPanel);

// the small wing slots of the panel use container id WING_SLOT_CONTAINER and slot id 0..2 (the swf indexes its slot data with
// slotID); wing list items are created by the swf with container -1 / slot -1 and carry the wing id in itemIndex
#define WING_SLOT_CONTAINER	200
#define WING_SLOT_COUNT		3
#define WING_ID_FIRST		39501

struct FUnrealWingRow
{
	INT		ID;
	INT		Quality;
	INT		Power;
	INT		AttrValue;
	FString	Name;
	FString	Icon;
	FString	AttrName;
	FString	Source;
	FString	Script;
	FUnrealWingRow() : ID(0), Quality(0), Power(0), AttrValue(0) {}
};

struct FUnrealWingLevelRow
{
	INT		Level;
	INT		Power;
	FString	AttrName[2];
	INT		AttrValue[2];
	INT		ItemID[2];
	INT		ItemCnt[2];
	FUnrealWingLevelRow() : Level(0), Power(0)
	{
		for(INT i = 0; i < 2; ++i) { AttrValue[i] = 0; ItemID[i] = 0; ItemCnt[i] = 0; }
	}
};

static TArray<FUnrealWingRow>		g_UnrealWingRows;
static TArray<FUnrealWingLevelRow>	g_UnrealWingLevels;
static UBOOL						g_UnrealWingLoaded = FALSE;
static INT							g_UnrealWingLevel = 0;		// g2c_wing_info
static UINT							g_UnrealWingMask = 0;		// g2c_wing_info, bit N <=> wing id WING_ID_FIRST + N

static FString UW_StripAsset(const FString& Raw)
{
	// Texture2D'MMO_ItemIcon_Supplies.Item_Wing_01_1'  ->  MMO_ItemIcon_Supplies.Item_Wing_01_1
	INT Begin = Raw.InStr(TEXT("'"));
	INT End = Raw.InStr(TEXT("'"), TRUE);
	if(Begin >= 0 && End > Begin)
	{
		return Raw.Mid(Begin + 1, End - Begin - 1);
	}
	return Raw;
}

static void UW_LoadTables()
{
	if(g_UnrealWingLoaded)
	{
		return;
	}
	g_UnrealWingLoaded = TRUE;

	CTabFile* tabFile = NULL;
	if(MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("wings.txt"), TRUE ) && tabFile)
	{
		INT _ID			= tabFile->GetKeyColIdx( TEXT("ID"));
		INT _Attr		= tabFile->GetKeyColIdx( TEXT("AttributeID"));
		INT _AttrValue	= tabFile->GetKeyColIdx( TEXT("AttributeValue"));
		INT _Power		= tabFile->GetKeyColIdx( TEXT("Power"));
		INT _Name		= tabFile->GetKeyColIdx( TEXT("Name"));
		INT _Quality	= tabFile->GetKeyColIdx( TEXT("Quality"));
		INT _Icon		= tabFile->GetKeyColIdx( TEXT("IconPath"));
		INT _Source		= tabFile->GetKeyColIdx( TEXT("Source"));
		INT _Script		= tabFile->GetKeyColIdx( TEXT("Script"));

		g_UnrealWingRows.Empty();
		for (int i = 2; i < tabFile->GetRowCount(); ++i)
		{
			FUnrealWingRow T;
			T.ID = appAtoi(tabFile->GetData( _ID,i));
			if(T.ID <= 0)
			{
				continue;
			}
			T.AttrName	= tabFile->GetData( _Attr,i);
			T.AttrValue	= appAtoi(tabFile->GetData( _AttrValue,i));
			T.Power		= appAtoi(tabFile->GetData( _Power,i));
			T.Name		= tabFile->GetData( _Name,i);
			T.Quality	= appAtoi(tabFile->GetData( _Quality,i));
			T.Icon		= UW_StripAsset(FString(tabFile->GetData( _Icon,i)));
			T.Source	= tabFile->GetData( _Source,i);
			T.Script	= tabFile->GetData( _Script,i);
			g_UnrealWingRows.AddItem(T);
		}
		delete tabFile;
	}

	tabFile = NULL;
	if(MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("wings_level.txt"), TRUE ) && tabFile)
	{
		INT _Level	= tabFile->GetKeyColIdx( TEXT("Level"));
		INT _Power	= tabFile->GetKeyColIdx( TEXT("Power"));
		INT _A1		= tabFile->GetKeyColIdx( TEXT("AttributeID1"));
		INT _V1		= tabFile->GetKeyColIdx( TEXT("AttributeValue1"));
		INT _A2		= tabFile->GetKeyColIdx( TEXT("AttributeID2"));
		INT _V2		= tabFile->GetKeyColIdx( TEXT("AttributeValue2"));
		INT _I1		= tabFile->GetKeyColIdx( TEXT("ConsumeItemID1"));
		INT _N1		= tabFile->GetKeyColIdx( TEXT("ConsumeItemCnt1"));
		INT _I2		= tabFile->GetKeyColIdx( TEXT("ConsumeItemID2"));
		INT _N2		= tabFile->GetKeyColIdx( TEXT("ConsumeItemCnt2"));

		g_UnrealWingLevels.Empty();
		for (int i = 2; i < tabFile->GetRowCount(); ++i)
		{
			FUnrealWingLevelRow T;
			T.Level = appAtoi(tabFile->GetData( _Level,i));
			T.Power = appAtoi(tabFile->GetData( _Power,i));
			T.AttrName[0]	= tabFile->GetData( _A1,i);
			T.AttrValue[0]	= appAtoi(tabFile->GetData( _V1,i));
			T.AttrName[1]	= tabFile->GetData( _A2,i);
			T.AttrValue[1]	= appAtoi(tabFile->GetData( _V2,i));
			T.ItemID[0]		= appAtoi(tabFile->GetData( _I1,i));
			T.ItemCnt[0]	= appAtoi(tabFile->GetData( _N1,i));
			T.ItemID[1]		= appAtoi(tabFile->GetData( _I2,i));
			T.ItemCnt[1]	= appAtoi(tabFile->GetData( _N2,i));
			g_UnrealWingLevels.AddItem(T);
		}
		delete tabFile;
	}
}

static FUnrealWingRow* UW_Find(INT WingID)
{
	for(INT i = 0; i < g_UnrealWingRows.Num(); ++i)
	{
		if(g_UnrealWingRows(i).ID == WingID)
		{
			return &g_UnrealWingRows(i);
		}
	}
	return NULL;
}

static FUnrealWingLevelRow* UW_LevelFind(INT Level)
{
	for(INT i = 0; i < g_UnrealWingLevels.Num(); ++i)
	{
		if(g_UnrealWingLevels(i).Level == Level)
		{
			return &g_UnrealWingLevels(i);
		}
	}
	return NULL;
}

static UBOOL UW_Owned(INT WingID)
{
	INT Bit = WingID - WING_ID_FIRST;
	return (Bit >= 0 && Bit < 31 && ((g_UnrealWingMask >> Bit) & 1)) ? TRUE : FALSE;
}

// attribute categories of wings_level.txt this game knows. The 1.05 only ones (ignore defence damage / resist, burning and
// bleeding resist) do not exist in the server, are not applied there and are therefore not listed either.
static UBOOL UW_AttrSupported(const FString& Cat)
{
	return Cat == TEXT("catMaxHp") || Cat == TEXT("catMaxMp") || Cat == TEXT("catPhyDef") || Cat == TEXT("catMaxPhyDef")
		|| Cat == TEXT("catMagDef") || Cat == TEXT("catMaxMagDef");
}

static FString UW_AttrLabel(const FString& Cat)
{
	if(Cat == TEXT("catMaxHp"))					return TEXT("Max HP");
	if(Cat == TEXT("catMaxMp"))					return TEXT("Max MP");
	if(Cat == TEXT("catPhyDef"))				return TEXT("Defense");
	if(Cat == TEXT("catMaxPhyDef"))				return TEXT("Max Defense");
	if(Cat == TEXT("catMagDef"))				return TEXT("Magic Defense");
	if(Cat == TEXT("catMaxMagDef"))				return TEXT("Max Magic Defense");
	if(Cat == TEXT("catMinIgnoreDefDmg"))		return TEXT("Min Holy Damage");
	if(Cat == TEXT("catMaxIgnoreDefDmg"))		return TEXT("Max Holy Damage");
	if(Cat == TEXT("catMinIgnoreDefResist"))	return TEXT("Min Holy Defense");
	if(Cat == TEXT("catMaxIgnoreDefResist"))	return TEXT("Max Holy Defense");
	if(Cat == TEXT("catBurningResist"))			return TEXT("Burn Resist");
	if(Cat == TEXT("catBleedingResist"))		return TEXT("Bleed Resist");

	// unknown attribute: "catSomeThing" -> "Some Thing"
	FString Out;
	INT Start = (Cat.Left(3) == TEXT("cat")) ? 3 : 0;
	for(INT i = Start; i < Cat.Len(); ++i)
	{
		TCHAR c = Cat[i];
		if(i > Start && c >= TEXT('A') && c <= TEXT('Z'))
		{
			Out += TEXT(" ");
		}
		Out += Cat.Mid(i, 1);
	}
	return Out;
}

// wings.txt Script holds up to two %d (attribute value, illusion power) and %% escapes
static FString UW_FormatScript(const FString& Fmt, INT A, INT B)
{
	FString Out;
	INT Used = 0;
	for(INT i = 0; i < Fmt.Len(); ++i)
	{
		if(Fmt[i] == TEXT('%') && i + 1 < Fmt.Len())
		{
			if(Fmt[i + 1] == TEXT('d'))
			{
				Out += FString::Printf(TEXT("%d"), (Used == 0) ? A : B);
				++Used;
				++i;
				continue;
			}
			if(Fmt[i + 1] == TEXT('%'))
			{
				Out += TEXT("%");
				++i;
				continue;
			}
		}
		Out += Fmt.Mid(i, 1);
	}
	return Out;
}

static INT UW_TotalPower()
{
	INT Total = 0;
	for(INT i = 0; i < g_UnrealWingRows.Num(); ++i)
	{
		if(UW_Owned(g_UnrealWingRows(i).ID))
		{
			Total += g_UnrealWingRows(i).Power;
		}
	}
	for(INT l = 1; l <= g_UnrealWingLevel; ++l)
	{
		FUnrealWingLevelRow* Row = UW_LevelFind(l);
		if(Row)
		{
			Total += Row->Power;
		}
	}
	return Total;
}

// ---- 3D preview: the centre hole of the window shows the game world with the player in it, so the selected wing is
// previewed by equipping it on the main player (client side only) and the rotate buttons turn the player. The real worn
// wing is remembered and restored when the panel closes.
static INT g_UnrealWingPreviewID = 0;	// wing currently shown on the preview actor
static UBOOL g_UnrealWingFirstInfo = FALSE;
static INT g_UnrealWingTurn = 0;		// -1 left, 1 right, 0 none
static INT g_UnrealWingShadowPasses = 0;	// no-shadow passes done since the window opened (reattaching every few frames makes ALL shadows flicker)
static UBOOL g_UnrealWingOwnedOnly = FALSE;	// "Owned" tab selected (the swf's own tab filter reads a cache that is never filled, so the list is filtered here)
static UBOOL g_UnrealWingOpen = FALSE;
static UBOOL g_UnrealWingFed = FALSE;
static INT g_UnrealWingDiagFrames = 0;
static INT g_UnrealWingRTDumpFrames = 0;	// frames until the next render target read-back
static INT g_UnrealWingRTCooldown = 0;
static INT g_UnrealWingWornPkt = 0;			// worn wing id of the last g2c_wing_info (authoritative)
static UBOOL g_UnrealWingWornKnown = FALSE;
static UBOOL g_UWFlagNoTip = FALSE;		// diagnostics: set by the natives called from the swf
static UBOOL g_UWFlagUpgrade = FALSE;

// ---- 3D preview, same mechanism as MMOUIMallEquipRolePanel: a copy of the main player is placed in the "PawnShowLevel_Mall"
// show level whose render target replaces the swf image "UnrealWingAvatar" (the mc_avater placeholder of UnrealWing.swf).
// The real player is never touched.
static AMMOPlayer*				g_UnrealWingShowPlayer = NULL;
static ULevelStreamingUIShow*	g_UnrealWingShowLevel = NULL;
static UWorld*					g_UnrealWingShowWorld = NULL;

static void UW_LogShowPlayer(const TCHAR* Why)
{
	AMMOPlayer* p = g_UnrealWingShowPlayer;
	if(p == NULL)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] %s: no preview actor"), Why);
		return;
	}
	warnf(NAME_Log, TEXT("[UnrealWing] %s: preview actor wing=%d mesh=%d WingInfo=%d wingMesh=%d attached=%d HiddenGame=%d yaw=%d"), Why,
		p->ActorAttributes ? p->ActorAttributes->WingID : -1,
		p->Mesh != NULL && p->Mesh->SkeletalMesh != NULL, p->WingInfo != NULL,
		p->WingInfo != NULL && p->WingInfo->SkelMeshComponent != NULL && p->WingInfo->SkelMeshComponent->SkeletalMesh != NULL,
		p->WingInfo != NULL && p->WingInfo->SkelMeshComponent != NULL && p->WingInfo->SkelMeshComponent->IsAttached(),
		p->WingInfo != NULL && p->WingInfo->SkelMeshComponent != NULL && p->WingInfo->SkelMeshComponent->HiddenGame, p->Rotation.Yaw);
}

static void UW_CopyEquipment(AMMOPlayer* Show, AMMOPlayer* Main)
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

// Place the clone where the scene capture that owns our render target looks: log every actor of the show-level band, find the
// ASceneCapture2DActor whose TextureTarget is the bound render target and the PlayerStart of the same level instance.
static void UW_FixPreviewPlacement(UBOOL bLogAll, UBOOL bSetYaw)
{
	AMMOPlayer* D = g_UnrealWingShowPlayer;
	if(D == NULL || g_UnrealWingShowLevel == NULL || g_UnrealWingShowWorld != GWorld || GWorld == NULL)
	{
		return;
	}
	UObject* RT = (UObject*)g_UnrealWingShowLevel->RenderTarget;
	ASceneCapture2DActor* Owner = NULL;
	if(bLogAll && g_UnrealWingShowLevel->RenderTarget)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] render target %s size=%dx%d"), *g_UnrealWingShowLevel->RenderTarget->GetName(),
			(INT)g_UnrealWingShowLevel->RenderTarget->GetSurfaceWidth(), (INT)g_UnrealWingShowLevel->RenderTarget->GetSurfaceHeight());
	}
	for(FActorIterator It; It; ++It)
	{
		AActor* A = *It;
		if(A == NULL || A == D || A->Location.Z > -15000.f || A->Location.Z < -30000.f)
		{
			continue;
		}
		INT bOwns = 0;
		ASceneCapture2DActor* SC = Cast<ASceneCapture2DActor>(A);
		if(SC)
		{
			USceneCapture2DComponent* C = Cast<USceneCapture2DComponent>(SC->SceneCapture);
			if(C && (UObject*)C->TextureTarget == RT && (Owner == NULL || C->bEnabled))
			{
				bOwns = C->bEnabled ? 2 : 1;
				Owner = SC;
			}
		}
		if(bLogAll)
		{
			warnf(NAME_Log, TEXT("[UnrealWing] show-level actor %s loc=(%.0f,%.0f,%.0f) rot=(p%d y%d) dist=%.0f ownsRT=%d"), *A->GetClass()->GetName(),
				A->Location.X, A->Location.Y, A->Location.Z, A->Rotation.Pitch, A->Rotation.Yaw, (A->Location - D->Location).Size(), bOwns);
		}
	}
	if(Owner == NULL)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] no scene capture owns the render target"));
		return;
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
		warnf(NAME_Log, TEXT("[UnrealWing] no PlayerStart near the owning capture"));
		return;
	}
	const FVector Want(Spot->Location.X, Spot->Location.Y, Owner->Location.Z);
	if((D->Location - Want).Size() > 300.f)
	{
		D->SetLocation(Want);
	}
	if(bSetYaw)
	{
		FRotator Face = D->Rotation;
		Face.Yaw = Owner->Rotation.Yaw;		// same direction as the camera: the back (wings) faces the camera
		D->SetRotation(Face);
	}
	warnf(NAME_Log, TEXT("[UnrealWing] preview placement: capture=(%.0f,%.0f,%.0f) rot(p%d y%d) spot=(%.0f,%.0f,%.0f) clone=(%.0f,%.0f,%.0f) yaw=%d cameraDist=%.0f"),
		Owner->Location.X, Owner->Location.Y, Owner->Location.Z, Owner->Rotation.Pitch, Owner->Rotation.Yaw,
		Spot->Location.X, Spot->Location.Y, Spot->Location.Z, D->Location.X, D->Location.Y, D->Location.Z, D->Rotation.Yaw, (D->Location - Owner->Location).Size());
}

static void UW_DumpParts(AMMOActor* A, const TCHAR* Label)
{
	if(A == NULL)
	{
		return;
	}
	warnf(NAME_Log, TEXT("[UnrealWing] parts of %s: actorHidden=%d components=%d"), Label, A->bHidden, A->Components.Num());
	for(INT i = 0; i < A->Components.Num(); ++i)
	{
		USkeletalMeshComponent* SK = Cast<USkeletalMeshComponent>(A->Components(i));
		if(SK)
		{
			warnf(NAME_Log, TEXT("[UnrealWing]   skel comp %s mesh='%s' elements=%d hiddenGame=%d ownerNoSee=%d onlyOwnerSee=%d attached=%d lightCh=%08x dpg=%d acceptsLights=%d lightEnv=%d castShadow=%d"),
				*SK->GetName(), SK->SkeletalMesh ? *SK->SkeletalMesh->GetName() : TEXT("(none)"), SK->GetNumElements(), SK->HiddenGame,
				SK->bOwnerNoSee, SK->bOnlyOwnerSee, SK->IsAttached(), *(DWORD*)&SK->LightingChannels, (INT)SK->DepthPriorityGroup,
				SK->bAcceptsLights, SK->LightEnvironment != NULL, SK->CastShadow);
		}
	}
}

// The swf's "UnrealWingAvatar" image never shows the render target, so the preview texture is drawn here with the canvas, in the
// transparent centre region of the window (called from AMMOHud::DrawHUD, i.e. before the GFx movies are drawn).
void MMOUIWingPanel_DrawPreview(UCanvas* Canvas)
{
	if(Canvas == NULL || !g_UnrealWingOpen || g_UnrealWingShowLevel == NULL || g_UnrealWingShowLevel->RenderTarget == NULL || g_UnrealWingShowWorld != GWorld)
	{
		return;
	}
	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIWingPanel* P = UM ? Cast<UMMOUIWingPanel>(UM->FindUIPanel(FName(TEXT("UnrealWing")))) : NULL;
	if(P == NULL || !P->GetBool(TEXT("visible")))
	{
		return;
	}
	// panel origin in movie pixels (the UI uses 1:1 pixels, see MoveCenterToScreenCenter); the centre region of UnrealWing.swf
	// (WindowNormalLargest frame: between the title and the "Wing Level" band, left of the list) in panel-local pixels
	const FLOAT PX = P->GetFloat(TEXT("x"));
	const FLOAT PY = P->GetFloat(TEXT("y"));
	const FLOAT HoleX = PX + 40.f, HoleY = PY + 67.f, HoleW = 686.f, HoleH = 269.f;
	const FLOAT Side = HoleH;					// the render target is square: keep the aspect, centred
	const FLOAT X0 = HoleX + (HoleW - Side) * 0.5f;
	UTexture* Tex = g_UnrealWingShowLevel->RenderTarget;
	const FLOAT TW = Tex->GetSurfaceWidth();
	const FLOAT TH = Tex->GetSurfaceHeight();
	static INT s_Logged = 0;
	if(s_Logged++ % 600 == 0)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] DrawPreview: canvas=%dx%d panel=(%.0f,%.0f) tile=(%.0f,%.0f %.0fx%.0f) tex=%.0fx%.0f"), (INT)Canvas->SizeX, (INT)Canvas->SizeY, PX, PY, X0, HoleY, Side, Side, TW, TH);
	}
	// drag with the left mouse button inside the hole to turn the preview actor
	{
		static UBOOL s_Dragging = FALSE;
		static FLOAT s_LastX = 0.f;
		ULocalPlayer* LP = (GEngine && GEngine->GamePlayers.Num() > 0) ? GEngine->GamePlayers(0) : NULL;
		if(LP && LP->ViewportClient && g_UnrealWingShowPlayer != NULL)
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
				FRotator R = g_UnrealWingShowPlayer->Rotation;
				R.Yaw += appTrunc((Mouse.X - s_LastX) * 60.f);
				g_UnrealWingShowPlayer->SetRotation(R);
			}
			s_LastX = Mouse.X;
		}
	}
	// dark backdrop over the whole hole; the render target (black background) is added on top so only the model shows
	// Z must be >= 1: with Z=0 the tile is not drawn at all (verified with marker tiles)
	if(Canvas->DefaultTexture)
	{
		Canvas->DrawTile(Canvas->DefaultTexture, HoleX, HoleY, 1.f, HoleW, HoleH, 0.f, 0.f, 1.f, 1.f, FLinearColor(0.f, 0.f, 0.f, 1.f), BLEND_Opaque);
	}
	// opaque (the first version, whose colours looked right): additive blending made the model pale. The capture leaves alpha ~0
	// on solid pixels so it can't be alpha blended; the black hole backdrop matches the render target's black background.
	Canvas->DrawTile(Tex, X0, HoleY, 1.f, Side, Side, 0.f, 0.f, TW, TH, FLinearColor(1.f, 1.f, 1.f, 1.f), BLEND_Opaque);
}

static void UW_DumpCaptureInfo()
{
	AMMOPlayer* D = g_UnrealWingShowPlayer;
	if(D == NULL || g_UnrealWingShowLevel == NULL || GWorld == NULL || g_UnrealWingShowWorld != GWorld)
	{
		return;
	}
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
			if(C)
			{
				const FVector Delta = D->Location - SC->Location;
				const FVector Local = FRotationMatrix(SC->Rotation).InverseTransformNormal(Delta);
				const FLOAT HAng = (Local.X > 0.f) ? appAtan2(Local.Y, Local.X) * 180.f / PI : 999.f;
				const FLOAT VAng = (Local.X > 0.f) ? appAtan2(Local.Z, Local.X) * 180.f / PI : 999.f;
				const EShowFlags SF = C->GetSceneShowFlags();
				warnf(NAME_Log, TEXT("[UnrealWing] capture '%s' tag=%s enabled=%d fov=%.1f near=%.1f far=%.1f clear=(%d,%d,%d,%d) viewMode=%d frameRate=%.1f postProcess=%d enablePP=%d fog=%d skipOccluded=%d showFlags=n/a(%d,%d) ownsRT=%d targetSize=%dx%d clone camera-space=(fwd %.0f right %.0f up %.0f) angle h=%.1f v=%.1f"),
					*SC->GetName(), *SC->Tag.ToString(), C->bEnabled, C->FieldOfView, C->NearPlane, C->FarPlane, C->ClearColor.R, C->ClearColor.G, C->ClearColor.B, C->ClearColor.A,
					(INT)C->ViewMode, C->FrameRate, C->PostProcess != NULL, C->bEnablePostProcess, C->bEnableFog, C->bSkipUpdateIfOwnerOccluded,
					0, 0, (UObject*)C->TextureTarget == (UObject*)g_UnrealWingShowLevel->RenderTarget,
					C->TextureTarget ? (INT)C->TextureTarget->GetSurfaceWidth() : 0, C->TextureTarget ? (INT)C->TextureTarget->GetSurfaceHeight() : 0,
					Local.X, Local.Y, Local.Z, HAng, VAng);
			}
			continue;
		}
		ALight* L = Cast<ALight>(A);
		if(L && L->LightComponent)
		{
			warnf(NAME_Log, TEXT("[UnrealWing] light %s '%s' loc=(%.0f,%.0f,%.0f) enabled=%d brightness=%.2f lightCh=%08x dist=%.0f"), *L->GetClass()->GetName(), *L->GetName(),
				L->Location.X, L->Location.Y, L->Location.Z, L->bEnabled, L->LightComponent->Brightness, *(DWORD*)&L->LightComponent->LightingChannels,
				(L->Location - D->Location).Size());
			continue;
		}
		AMMOPlayer* OtherPlayer = Cast<AMMOPlayer>(A);
		if(OtherPlayer && OtherPlayer != MMOFacade::getSingleton().GetMainPlayer())
		{
			UW_DumpParts(OtherPlayer, TEXT("show-level MMOPlayer (other instance)"));
		}
	}
	warnf(NAME_Log, TEXT("[UnrealWing] clone actor: lightEnv=%d lightChBody=%08x bounds origin=(%.0f,%.0f,%.0f) radius=%.0f"), D->LightEnvironment != NULL,
		D->Mesh ? *(DWORD*)&D->Mesh->LightingChannels : 0, D->Mesh ? D->Mesh->Bounds.Origin.X : 0.f, D->Mesh ? D->Mesh->Bounds.Origin.Y : 0.f, D->Mesh ? D->Mesh->Bounds.Origin.Z : 0.f,
		D->Mesh ? D->Mesh->Bounds.SphereRadius : 0.f);
}

// read the render target back, log statistics and save it as a 24-bit BMP
static void UW_DumpRenderTarget(const TCHAR* Why)
{
	if(g_UnrealWingShowLevel == NULL || g_UnrealWingShowLevel->RenderTarget == NULL)
	{
		return;
	}
	UTextureRenderTarget2D* RTex = g_UnrealWingShowLevel->RenderTarget;
	FTextureRenderTargetResource* Res = RTex->GameThread_GetRenderTargetResource();
	if(Res == NULL)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] RT dump (%s): no render target resource"), Why);
		return;
	}
	TArray<FColor> Pix;
	if(!Res->ReadPixels(Pix) || Pix.Num() <= 0)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] RT dump (%s): ReadPixels failed"), Why);
		return;
	}
	INT W = (INT)RTex->GetSurfaceWidth();
	INT H = (INT)RTex->GetSurfaceHeight();
	if(W <= 0 || H <= 0 || W * H != Pix.Num())
	{
		W = H = (INT)appSqrt((FLOAT)Pix.Num());
	}
	DOUBLE SumR = 0, SumG = 0, SumB = 0, SumA = 0;
	INT AlphaPix = 0, NonBlack = 0, MinX = W, MinY = H, MaxX = -1, MaxY = -1;
	DOUBLE GridR[8][8], GridG[8][8], GridB[8][8], GridA[8][8];
	INT GridN[8][8];
	appMemzero(GridR, sizeof(GridR)); appMemzero(GridG, sizeof(GridG)); appMemzero(GridB, sizeof(GridB)); appMemzero(GridA, sizeof(GridA)); appMemzero(GridN, sizeof(GridN));
	for(INT y = 0; y < H; ++y)
	{
		for(INT x = 0; x < W; ++x)
		{
			const FColor& C = Pix(y * W + x);
			SumR += C.R; SumG += C.G; SumB += C.B; SumA += C.A;
			const INT gx = Min(7, x * 8 / W), gy = Min(7, y * 8 / H);
			GridR[gy][gx] += C.R; GridG[gy][gx] += C.G; GridB[gy][gx] += C.B; GridA[gy][gx] += C.A; GridN[gy][gx]++;
			if(C.A > 0)
			{
				++AlphaPix;
			}
			if(C.R > 8 || C.G > 8 || C.B > 8)
			{
				++NonBlack;
			}
			if(C.A > 0 || C.R > 8 || C.G > 8 || C.B > 8)
			{
				MinX = Min(MinX, x); MinY = Min(MinY, y); MaxX = Max(MaxX, x); MaxY = Max(MaxY, y);
			}
		}
	}
	const DOUBLE N = (DOUBLE)Pix.Num();
	warnf(NAME_Log, TEXT("[UnrealWing] RT dump (%s): %dx%d avgRGBA=(%.1f,%.1f,%.1f,%.1f) alpha>0=%d nonBlack=%d bbox=(%d,%d)-(%d,%d)"), Why, W, H,
		SumR / N, SumG / N, SumB / N, SumA / N, AlphaPix, NonBlack, MinX, MinY, MaxX, MaxY);
	for(INT gy = 0; gy < 8; ++gy)
	{
		FString Row;
		for(INT gx = 0; gx < 8; ++gx)
		{
			const DOUBLE n = Max(1, GridN[gy][gx]);
			Row += FString::Printf(TEXT("(%d,%d,%d,%d) "), (INT)(GridR[gy][gx] / n), (INT)(GridG[gy][gx] / n), (INT)(GridB[gy][gx] / n), (INT)(GridA[gy][gx] / n));
		}
		warnf(NAME_Log, TEXT("[UnrealWing] RT grid row %d: %s"), gy, *Row);
	}
	// 24-bit BMP (bottom-up rows, BGR, rows padded to 4 bytes)
	const INT RowBytes = (W * 3 + 3) & ~3;
	const INT DataBytes = RowBytes * H;
	TArray<BYTE> Bmp;
	Bmp.AddZeroed(54 + DataBytes);
	BYTE* B = &Bmp(0);
	B[0] = 'B'; B[1] = 'M';
	*(INT*)(B + 2) = 54 + DataBytes;
	*(INT*)(B + 10) = 54;
	*(INT*)(B + 14) = 40;
	*(INT*)(B + 18) = W;
	*(INT*)(B + 22) = H;
	*(WORD*)(B + 26) = 1;
	*(WORD*)(B + 28) = 24;
	*(INT*)(B + 34) = DataBytes;
	for(INT y = 0; y < H; ++y)
	{
		BYTE* Dst = B + 54 + (H - 1 - y) * RowBytes;
		for(INT x = 0; x < W; ++x)
		{
			const FColor& C = Pix(y * W + x);
			// alpha is blended over mid grey so that an empty (alpha 0) target is distinguishable from black
			const INT A = C.A;
			Dst[x * 3 + 0] = (BYTE)((C.B * A + 128 * (255 - A)) / 255);
			Dst[x * 3 + 1] = (BYTE)((C.G * A + 128 * (255 - A)) / 255);
			Dst[x * 3 + 2] = (BYTE)((C.R * A + 128 * (255 - A)) / 255);
		}
	}
	FString Path = TEXT("C:\\Users\\johne\\.claude\\jobs\\4b3f839e\\tmp\\wing_preview_rt.bmp");
	UBOOL bSaved = appSaveArrayToFile(Bmp, *Path);
	if(!bSaved)
	{
		Path = appGameLogDir() + TEXT("wing_preview_rt.bmp");
		bSaved = appSaveArrayToFile(Bmp, *Path);
	}
	warnf(NAME_Log, TEXT("[UnrealWing] RT dump saved=%d path=%s"), bSaved, *Path);
}

// window closed
static void UW_StopShow()
{
	g_UnrealWingTurn = 0;
	g_UnrealWingPreviewID = 0;
	g_UnrealWingShadowPasses = 0;
	if(g_UnrealWingShowLevel)
	{
		g_UnrealWingShowLevel->SetEnabled(FALSE);
	}
	if(g_UnrealWingShowPlayer && g_UnrealWingShowWorld == GWorld && GWorld != NULL)
	{
		GWorld->DestroyActor(g_UnrealWingShowPlayer);
	}
	g_UnrealWingShowPlayer = NULL;
	g_UnrealWingShowLevel = NULL;
}

// window opened
static void UW_StartShow(UMMOUIWingPanel* Panel)
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
	if(g_UnrealWingShowWorld != GWorld)
	{
		g_UnrealWingShowPlayer = NULL;		// the old world (and the actor) is gone
		g_UnrealWingShowLevel = NULL;
		g_UnrealWingShowWorld = GWorld;
	}
	if(g_UnrealWingShowLevel == NULL)
	{
		g_UnrealWingShowLevel = Panel->GFXPlayer->eventGetShowLevel(TEXT("PawnShowLevel_Mall"));
	}
	if(g_UnrealWingShowLevel == NULL || g_UnrealWingShowLevel->RenderTarget == NULL)
	{
		warnf(NAME_Log, TEXT("[UnrealWing] preview: show level PawnShowLevel_Mall=%d renderTarget=%d"), g_UnrealWingShowLevel != NULL,
			g_UnrealWingShowLevel != NULL && g_UnrealWingShowLevel->RenderTarget != NULL);
		g_UnrealWingShowLevel = NULL;
		return;
	}
	g_UnrealWingShowLevel->SetEnabled(TRUE);
	// SetExternalTexture('UnrealWingAvatar') is not used any more: the swf showed the render target as a dark rectangle at the stage
	// origin. The texture is drawn by MMOUIWingPanel_DrawPreview instead.
	const UBOOL bBound = FALSE;
	if(UGFxObject* Av = Panel->GetObject(TEXT("mc_avater")))
	{
		Av->SetVisible(FALSE);
	}

	if(g_UnrealWingShowPlayer == NULL)
	{
		g_UnrealWingShowPlayer = Panel->eventClonePlayer(Main);
		if(g_UnrealWingShowPlayer)
		{
			g_UnrealWingShowPlayer->PawnAnimInfo = Main->PawnAnimInfo;
		}
	}
	AMMOPlayer* Show = g_UnrealWingShowPlayer;
	warnf(NAME_Log, TEXT("[UnrealWing] preview: SetExternalTexture('UnrealWingAvatar')=%d previewActor=%d"), bBound, Show != NULL);
	if(Show == NULL)
	{
		return;
	}
	Show->NativeSetMesh(Main->GetClassId(), Main->GetSex());
	Show->UnEquipMount();
	UW_CopyEquipment(Show, Main);
	FLOAT Scale = 1.2f;
	if(Main->ActorAttributes->GenderType == EGenderType::EGT_Female)
	{
		Scale = 1.4f;
	}
	Show->DrawScale = Scale;

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
	g_UnrealWingShowLevel->SetShowActor(Show, tag);
	g_UnrealWingDiagFrames = 90;
	g_UnrealWingRTDumpFrames = 120;
	g_UnrealWingRTCooldown = 0;
	// wings are mounted on the back: show the back
	FRotator Flip = Show->Rotation;
	Flip.Yaw += 32768;
	Show->SetRotation(Flip);
	UW_FixPreviewPlacement(FALSE, TRUE);
	g_UnrealWingPreviewID = -1;
}

// wing selected in the list (or the worn wing at open time): show it on the preview actor
static void UW_ApplyPreview(INT WingID)
{
	if(g_UnrealWingShowPlayer == NULL || g_UnrealWingShowWorld != GWorld || UW_Find(WingID) == NULL)
	{
		return;
	}
	if(g_UnrealWingPreviewID == WingID)
	{
		return;
	}
	g_UnrealWingPreviewID = WingID;
	if(g_UnrealWingRTCooldown <= 0)
	{
		g_UnrealWingRTDumpFrames = 60;		// read the render target back a second after the wing changed
		g_UnrealWingRTCooldown = 600;
	}
	g_UnrealWingShowPlayer->EquipWing(WingID);
	UW_LogShowPlayer(TEXT("preview EquipWing"));
}

static INT UW_MainPlayerWornID()
{
	if(g_UnrealWingWornKnown)
	{
		return g_UnrealWingWornPkt;		// g2c_wing_info is authoritative (the actor may not have been refreshed yet)
	}
	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer && pMainPlayer->ActorAttributes)
	{
		return pMainPlayer->ActorAttributes->WingID;
	}
	return 0;
}

// ------------------------------------------------------------------------------------------------------------------------
// Delivery helpers.
// The CLIK panel builds itself lazily: UIComponent.validateNow() -> configUI() runs on the first RENDER event after the swf
// was loaded. UCUpdateWingData / UCSetLocalizationData use arrays that configUI fills (wm_slotWingItems, propertyItems ...)
// and configUI resets what was set before it ran, so data fed before configUI is lost and the swf stays at its design time
// placeholders. Force configUI (validateNow is public) before feeding, and feed once more from the frame tick.
static INT g_UnrealWingPendingFrames = 0;

static UBOOL UW_EnsureConfigured(UMMOUIWingPanel* Panel)
{
	if(Panel->GetBool(TEXT("initialized")))
	{
		return TRUE;
	}
	TArray<struct FASValue> NoArgs;
	Panel->Invoke(TEXT("validateNow"), NoArgs);
	return Panel->GetBool(TEXT("initialized"));
}

static void UW_FeedLocalization(UMMOUIWingPanel* Panel)
{
	if(Panel->GFXPlayer == NULL)
	{
		return;
	}
	UGFxObject* Loc = Panel->GFXPlayer->CreateObject(TEXT("Object"));
	if(Loc)
	{
		Loc->SetString(TEXT("title"),			TEXT("Illusion Wings"));
		Loc->SetString(TEXT("wingProperty"),	TEXT("Wing Attributes"));
		Loc->SetString(TEXT("wingPower"),		TEXT("Illusion Power"));
		Loc->SetString(TEXT("upgrade"),			TEXT("Upgrade"));
		Loc->SetString(TEXT("totalWingPower"),	TEXT("Total Illusion Power"));
		Loc->SetString(TEXT("showSetting"),		TEXT("Display"));
		Loc->SetString(TEXT("wing"),			TEXT("Wing"));
		Loc->SetString(TEXT("showAll"),			TEXT("All"));
		Loc->SetString(TEXT("showOwner"),		TEXT("Owned"));
		Panel->eventCallSetWingLocalization(Loc);
	}
}

// the preview actor stands in the real world, so its dynamic shadow shows up on the ground in view (a dark wing-shaped shadow
// that turns with the preview): switch shadow casting off on all of its components and attached actors
static void UW_NoShadow(AActor* A, INT& Changed, INT Depth = 0)
{
	if(A == NULL || Depth > 2)
	{
		return;
	}
	for(INT i = 0; i < A->AllComponents.Num(); ++i)
	{
		// the light environment builds its own (composite) dynamic shadow for the primitives it lights
		UDynamicLightEnvironmentComponent* LE = Cast<UDynamicLightEnvironmentComponent>(A->AllComponents(i));
		if(LE && (LE->bCastShadows || LE->bCompositeShadowsFromDynamicLights))
		{
			LE->bCastShadows = FALSE;
			LE->bCompositeShadowsFromDynamicLights = FALSE;
			LE->BeginDeferredReattach();
			++Changed;
		}
		UPrimitiveComponent* C = Cast<UPrimitiveComponent>(A->AllComponents(i));
		if(C && (C->CastShadow || C->bCastDynamicShadow))
		{
			C->CastShadow = FALSE;
			C->bCastDynamicShadow = FALSE;
			C->BeginDeferredReattach();
			++Changed;
		}
	}
	for(INT i = 0; i < A->Attached.Num(); ++i)
	{
		UW_NoShadow(A->Attached(i), Changed, Depth + 1);
	}
}

// called every frame from UMMOGFXPlayer_RootBase::PostAdvance
void MMOUIWingPanel_Tick()
{
	if(g_UnrealWingOpen && g_UnrealWingShowPlayer != NULL && g_UnrealWingShowWorld == GWorld)
	{
		static INT s_ShadowCounter = 0;
		if(g_UnrealWingShadowPasses < 4 && ++s_ShadowCounter % 40 == 0)
		{
			INT Changed = 0;
			UW_NoShadow(g_UnrealWingShowPlayer, Changed);
			warnf(NAME_Log, TEXT("[UnrealWing] no-shadow pass %d: %d components changed"), g_UnrealWingShadowPasses, Changed);
			++g_UnrealWingShadowPasses;
		}
	}
	if(g_UnrealWingTurn != 0 && g_UnrealWingShowPlayer != NULL && g_UnrealWingShowWorld == GWorld)
	{
		AMMOPlayer* p = g_UnrealWingShowPlayer;
		{
			FRotator R = p->Rotation;
			R.Yaw += g_UnrealWingTurn * 350;
			p->SetRotation(R);
		}
	}
	if(g_UnrealWingOpen && g_UnrealWingFed)
	{
		static INT s_PollCounter = 0;
		if(++s_PollCounter % 6 == 0)
		{
			UMMOUIManager* UM0 = MMOUI::GetUIManager();
			UMMOUIWingPanel* P0 = UM0 ? Cast<UMMOUIWingPanel>(UM0->FindUIPanel(FName(TEXT("UnrealWing")))) : NULL;
			UGFxObject* Owned = P0 ? P0->GetObject(TEXT("btn_showOwner")) : NULL;
			if(Owned)
			{
				const UBOOL bOwned = Owned->GetBool(TEXT("selected"));
				if(bOwned != g_UnrealWingOwnedOnly)
				{
					g_UnrealWingOwnedOnly = bOwned;
					warnf(NAME_Log, TEXT("[UnrealWing] tab changed: ownedOnly=%d"), bOwned);
					P0->UpdateWing();
				}
			}
		}
	}
	if(g_UnrealWingRTCooldown > 0)
	{
		--g_UnrealWingRTCooldown;
	}
	if(g_UnrealWingRTDumpFrames > 0 && --g_UnrealWingRTDumpFrames == 0 && g_UnrealWingShowPlayer != NULL && g_UnrealWingShowWorld == GWorld)
	{
		UW_DumpCaptureInfo();
		UW_DumpRenderTarget(TEXT("preview"));
	}
	if(g_UnrealWingDiagFrames > 0 && --g_UnrealWingDiagFrames == 0)
	{
		AMMOPlayer* D = g_UnrealWingShowPlayer;
		if(D && D->Mesh && g_UnrealWingShowWorld == GWorld && GWorld != NULL)
		{
			warnf(NAME_Log, TEXT("[UnrealWing] preview diag: loc=(%.0f,%.0f,%.0f) yaw=%d scale=%.2f meshHidden=%d lastRender=%.2f now=%.2f boundsR=%.0f origin=(%.0f,%.0f,%.0f) lightEnv=%d"),
				D->Location.X, D->Location.Y, D->Location.Z, D->Rotation.Yaw, D->DrawScale, D->Mesh->HiddenGame,
				D->Mesh->LastRenderTime, GWorld->GetTimeSeconds(), D->Mesh->Bounds.SphereRadius,
				D->Mesh->Bounds.Origin.X, D->Mesh->Bounds.Origin.Y, D->Mesh->Bounds.Origin.Z, D->LightEnvironment != NULL);
			UW_LogShowPlayer(TEXT("preview diag"));
			UW_DumpParts(D, TEXT("preview clone"));
			UW_DumpParts(MMOFacade::getSingleton().GetMainPlayer(), TEXT("main player"));
			warnf(NAME_Log, TEXT("[UnrealWing] preview mesh='%s' materials=%d"),
				D->Mesh->SkeletalMesh ? *D->Mesh->SkeletalMesh->GetName() : TEXT("(none)"), D->Mesh->Materials.Num());
			UW_FixPreviewPlacement(TRUE, FALSE);
		}
	}
	if(g_UnrealWingPendingFrames <= 0)
	{
		return;
	}
	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIWingPanel* Panel = UM ? Cast<UMMOUIWingPanel>(UM->FindUIPanel(FName(TEXT("UnrealWing")))) : NULL;
	if(Panel == NULL || !Panel->GetVisible())
	{
		g_UnrealWingPendingFrames = 0;
		return;
	}
	--g_UnrealWingPendingFrames;
	if(!UW_EnsureConfigured(Panel))
	{
		if(g_UnrealWingPendingFrames == 0)
		{
			warnf(NAME_Log, TEXT("[UnrealWing] panel never became initialized (UIComponent.initialized == false)"));
		}
		return;
	}
	UW_FeedLocalization(Panel);
	Panel->UpdateWing();
	g_UnrealWingPendingFrames = 0;
	warnf(NAME_Log, TEXT("[UnrealWing] tick feed done, swf cacheWingProperty='%s' curSelectWingId='%s'"), *Panel->GetString(TEXT("cacheWingProperty")), *Panel->GetString(TEXT("curSelectWingId")));
}

// called from the network layer (g2c_wing_info)
void MMOUIWingPanel_OnWingInfo(INT Level, UINT Mask, INT CurrentWing, UBOOL bUpgraded)
{
	g_UnrealWingLevel = Level;
	g_UnrealWingMask = Mask;
	g_UnrealWingWornPkt = CurrentWing;
	g_UnrealWingWornKnown = TRUE;
	{
		// the player never receives its own appear data: apply the worn wing to the main player here (login, scene change, wear)
		AMMOPlayer* MainP = MMOFacade::getSingleton().GetMainPlayer();
		if(MainP && MainP->ActorAttributes && MainP->ActorAttributes->WingID != CurrentWing)
		{
			if(CurrentWing > 0)
			{
				MainP->EquipWing(CurrentWing);
			}
			else
			{
				MainP->UnEquipWing();
			}
			warnf(NAME_Log, TEXT("[UnrealWing] main player wing -> %d (from g2c_wing_info)"), CurrentWing);
		}
	}
	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIWingPanel* Panel = UM ? Cast<UMMOUIWingPanel>(UM->FindUIPanel(FName(TEXT("UnrealWing")))) : NULL;
	if(g_UnrealWingFirstInfo)
	{
		// first reply after opening: select the worn wing (the open-time guess ran before the worn wing was known)
		g_UnrealWingFirstInfo = FALSE;
		if(Panel && CurrentWing > 0)
		{
			Panel->SelectWingID = CurrentWing;
		}
	}
	warnf(NAME_Log, TEXT("[UnrealWing] g2c_wing_info level=%d mask=%u worn=%d panel=%d"), Level, Mask, CurrentWing, Panel != NULL);
	if(Panel && Panel->GetVisible())
	{
		UW_EnsureConfigured(Panel);
		Panel->UpdateWing();
		if(bUpgraded)
		{
			Panel->eventCallOnWingUpgrade();
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------
void UMMOUIWingPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		UW_LoadTables();
		UW_StartShow(this);
		g_UnrealWingOpen = TRUE;
		g_UnrealWingFed = FALSE;
		g_UnrealWingOwnedOnly = FALSE;
		g_UnrealWingFirstInfo = TRUE;
		SelectWingID = -1;		// pick the worn wing again on every open

		// make sure the swf ran configUI before data is fed (see UW_EnsureConfigured), then feed again from the tick
		const UBOOL bConfigured = UW_EnsureConfigured(this);
		UW_FeedLocalization(this);

		// ask the server for the wing level / owned wings, then show what is known right now
		CMessageHandler::GetInstance().Send_WingOp(0, 0);
		UpdateWing();

		g_UnrealWingPendingFrames = 240;
		// probe which AS object this widget really is
		warnf(NAME_Log, TEXT("[UnrealWing] probe name='%s' PanelName='%s' visible=%d x=%d list_wing=%d MyWindowsFrame_mc=%d btn_upgrade=%d child UnrealWing=%d cacheWingProperty(null?)=%d"),
			*GetString(TEXT("name")), *GetString(TEXT("PanelName")), GetBool(TEXT("visible")), (INT)GetFloat(TEXT("x")),
			GetObject(TEXT("list_wing")) != NULL, GetObject(TEXT("MyWindowsFrame_mc")) != NULL, GetObject(TEXT("btn_upgrade")) != NULL,
			GetObject(TEXT("UnrealWing")) != NULL, GetObject(TEXT("cacheWingProperty")) != NULL);
		warnf(NAME_Log, TEXT("[UnrealWing] PostOnShow GFXPlayer=%d configured=%d wings=%d cacheWingProperty='%s'"),
			GFXPlayer != NULL, bConfigured, g_UnrealWingRows.Num(), *GetString(TEXT("cacheWingProperty")));
	}
	else
	{
		g_UnrealWingOpen = FALSE;
		UW_StopShow();
	}
}

INT UMMOUIWingPanel::GetSlotType()
{
	return 0;
}

//------------------------------------------------------------------------------------------------------------

// wing id carried by a wing list item / panel slot (SetData(.., itemIndex = wing id, ..))
static INT UW_IdFromSlot(UGFxObject* mc, INT SlotID)
{
	if(mc)
	{
		INT id = mc->GetInt(TEXT("itemIndex"));
		if(UW_Find(id))
		{
			return id;
		}
		id = mc->GetInt(TEXT("itemid"));
		if(UW_Find(id))
		{
			return id;
		}
	}
	if(UW_Find(SlotID))
	{
		return SlotID;
	}
	return 0;
}

static void UW_ToggleWear(INT WingID)
{
	if(UW_Find(WingID) == NULL)
	{
		return;
	}
	if(!UW_Owned(WingID))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("You do not own this wing yet."), MsgInfoType_Warning);
		return;
	}
	if(UW_MainPlayerWornID() == WingID)
	{
		CMessageHandler::GetInstance().Send_WingOp(2, 0);
	}
	else
	{
		CMessageHandler::GetInstance().Send_WingOp(1, WingID);
	}
}

UBOOL UMMOUIWingPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

UBOOL UMMOUIWingPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

// right click / double click on a wing: wear it, or take it off when it is the worn one
UBOOL UMMOUIWingPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return OnDoubleClickSlot(mc, SlotType, ContainerID, SlotID, bDefault);
}

UBOOL UMMOUIWingPanel::OnDoubleClickSlot(class UGFxObject* SrcSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,UBOOL bDefault)
{
	if(bDefault || !GetVisible() || SrcSlotType != ESPType_Mount)
	{
		return FALSE;
	}

	if(SrcContainerID == WING_SLOT_CONTAINER)
	{
		// panel slot: take the worn wing off
		if(UW_MainPlayerWornID() != 0)
		{
			CMessageHandler::GetInstance().Send_WingOp(2, 0);
		}
		return TRUE;
	}

	INT WingID = UW_IdFromSlot(SrcSlotMC, SrcSlotID);
	if(WingID == 0)
	{
		return FALSE;
	}
	UW_ToggleWear(WingID);
	return TRUE;
}

UBOOL UMMOUIWingPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if(!GetVisible() || SrcSlotType != ESPType_Mount)
	{
		return FALSE;
	}

	const UBOOL bSrcPanelSlot = (SrcContainerID == WING_SLOT_CONTAINER);
	const UBOOL bDestPanelSlot = (DestSlotType == ESPType_Mount && DestContainerID == WING_SLOT_CONTAINER);

	if(!bSrcPanelSlot && bDestPanelSlot)
	{
		// wing list -> wing slot : wear
		if(DestSlotID == 0)
		{
			INT WingID = UW_IdFromSlot(SrcSlotMC, SrcSlotID);
			if(WingID != 0 && UW_MainPlayerWornID() != WingID)
			{
				UW_ToggleWear(WingID);
			}
		}
		return TRUE;
	}
	if(bSrcPanelSlot && !bDestPanelSlot)
	{
		// wing slot -> anywhere else : take off
		if(SrcSlotID == 0 && UW_MainPlayerWornID() != 0)
		{
			CMessageHandler::GetInstance().Send_WingOp(2, 0);
		}
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------
static void UW_UpdateTitle(UMMOUIWingPanel* P)
{
	UGFxObject* Frame = P->GetObject(TEXT("MyWindowsFrame_mc"));
	UGFxObject* Tf = Frame ? Frame->GetObject(TEXT("tF_windowTitle")) : NULL;
	if(Tf)
	{
		FUnrealWingRow* Row = UW_Find(P->SelectWingID);
		Tf->SetString(TEXT("text"), Row ? FString::Printf(TEXT("Illusion Wings - %s"), *Row->Name) : FString(TEXT("Illusion Wings")));
	}
}

void UMMOUIWingPanel::UpdateWing()
{
	if(GFXPlayer == NULL)
	{
		return;
	}
	UW_LoadTables();

	const INT Worn = UW_MainPlayerWornID();

	// selection: keep the current one, else the worn wing, else the first owned wing, else the first wing
	if(UW_Find(SelectWingID) == NULL)
	{
		SelectWingID = -1;
		if(Worn > 0 && UW_Find(Worn))
		{
			SelectWingID = Worn;
		}
		for(INT i = 0; SelectWingID < 0 && i < g_UnrealWingRows.Num(); ++i)
		{
			if(UW_Owned(g_UnrealWingRows(i).ID))
			{
				SelectWingID = g_UnrealWingRows(i).ID;
			}
		}
		if(SelectWingID < 0 && g_UnrealWingRows.Num() > 0)
		{
			SelectWingID = g_UnrealWingRows(0).ID;
		}
	}

	UGFxObject* Data = GFXPlayer->CreateObject(TEXT("Object"));
	UGFxObject* WingArray = GFXPlayer->CreateArray();
	UGFxObject* SlotArray = GFXPlayer->CreateArray();
	UGFxObject* PropArray = GFXPlayer->CreateArray();
	if(Data == NULL || WingArray == NULL || SlotArray == NULL || PropArray == NULL)
	{
		return;
	}

	// ---- wing list
	FString SelectedScript;
	for(INT i = 0; i < g_UnrealWingRows.Num(); ++i)
	{
		FUnrealWingRow& Row = g_UnrealWingRows(i);
		const UBOOL bOwner = UW_Owned(Row.ID);

		UGFxObject* Obj = GFXPlayer->CreateObject(TEXT("Object"));
		if(Obj == NULL)
		{
			continue;
		}
		FString Script = UW_FormatScript(Row.Script, Row.AttrValue, Row.Power);
		FString Desc = Row.Source;
		if(!bOwner)
		{
			Desc = FString(TEXT("<font color='#FF4040'>Not owned yet</font><br>")) + Desc;
		}

		Obj->SetInt(TEXT("wingId"),					Row.ID);
		Obj->SetBool(TEXT("bOwner"),				bOwner);
		Obj->SetBool(TEXT("isGray"),				!bOwner);
		Obj->SetInt(TEXT("wingType"),				ESPType_Mount);
		Obj->SetInt(TEXT("curSelectedWingId"),		SelectWingID);
		Obj->SetString(TEXT("iconPath"),			Row.Icon);	// ItemSlot adds "img://" itself
		Obj->SetString(TEXT("title"),				FString::Printf(TEXT("Selected: %s"), *Row.Name));
		Obj->SetString(TEXT("wingName"),			Row.Name);
		Obj->SetString(TEXT("desc"),				Desc);
		Obj->SetString(TEXT("scriptDesc"),			Script);
		WingArray->SetElementObject(i, Obj);

		if(Row.ID == SelectWingID)
		{
			SelectedScript = Script;
		}
	}

	// ---- the three wing slots: slot 0 holds the worn wing, the others are reserved
	for(INT s = 0; s < WING_SLOT_COUNT; ++s)
	{
		UGFxObject* Obj = GFXPlayer->CreateObject(TEXT("Object"));
		if(Obj == NULL)
		{
			continue;
		}
		FUnrealWingRow* WornRow = (s == 0) ? UW_Find(Worn) : NULL;
		Obj->SetInt(TEXT("slotType"),		ESPType_Mount);
		Obj->SetInt(TEXT("itemIndex"),		WornRow ? WornRow->ID : 0);
		Obj->SetInt(TEXT("containerID"),	WING_SLOT_CONTAINER);
		Obj->SetInt(TEXT("slotID"),			s);
		Obj->SetBool(TEXT("isLock"),		s != 0);
		Obj->SetString(TEXT("iconName"),	WornRow ? WornRow->Icon : FString(TEXT("MMO_UIIcon.icon.blank")));
		Obj->SetString(TEXT("lockTips"),	TEXT("Reserved slot (only one wing can be worn)"));
		SlotArray->SetElementObject(s, Obj);
	}

	// ---- attribute list: worn wing first, then the sum of the level bonuses
	TArray<FString> PropNames;
	TArray<INT> PropValues;
	FUnrealWingRow* WornRow = UW_Find(Worn);
	if(WornRow && WornRow->AttrValue != 0)
	{
		PropNames.AddItem(UW_AttrLabel(WornRow->AttrName));
		PropValues.AddItem(WornRow->AttrValue);
	}
	for(INT l = 1; l <= g_UnrealWingLevel; ++l)
	{
		FUnrealWingLevelRow* Lv = UW_LevelFind(l);
		if(Lv == NULL)
		{
			continue;
		}
		for(INT k = 0; k < 2; ++k)
		{
			if(Lv->AttrName[k].Len() == 0 || Lv->AttrValue[k] == 0 || !UW_AttrSupported(Lv->AttrName[k]))
			{
				continue;
			}
			FString Label = UW_AttrLabel(Lv->AttrName[k]);
			INT Found = -1;
			for(INT n = 0; n < PropNames.Num(); ++n)
			{
				if(PropNames(n) == Label)
				{
					Found = n;
					break;
				}
			}
			if(Found >= 0)
			{
				PropValues(Found) += Lv->AttrValue[k];
			}
			else
			{
				PropNames.AddItem(Label);
				PropValues.AddItem(Lv->AttrValue[k]);
			}
		}
	}
	for(INT r = 0; r < 4; ++r)
	{
		if(r * 3 >= PropNames.Num())
		{
			break;
		}
		UGFxObject* Obj = GFXPlayer->CreateObject(TEXT("Object"));
		if(Obj == NULL)
		{
			continue;
		}
		for(INT c = 0; c < 3 && (r * 3 + c) < PropNames.Num(); ++c)
		{
			Obj->SetString(FString::Printf(TEXT("property%d"), c + 1),		PropNames(r * 3 + c));
			Obj->SetString(FString::Printf(TEXT("propertyValue%d"), c + 1),	FString::Printf(TEXT("+%d"), PropValues(r * 3 + c)));
		}
		PropArray->SetElementObject(r, Obj);
	}

	// ---- upgrade panel: what the next level costs / gives (nothing at the highest level)
	FUnrealWingLevelRow* Next = UW_LevelFind(g_UnrealWingLevel + 1);
	if(Next)
	{
		UGFxObject* Up = GFXPlayer->CreateObject(TEXT("Object"));
		UGFxObject* UpProps = GFXPlayer->CreateArray();
		UGFxObject* UpItems = GFXPlayer->CreateArray();
		if(Up && UpProps && UpItems)
		{
			INT p = 0;
			for(INT k = 0; k < 2; ++k)
			{
				if(Next->AttrName[k].Len() == 0 || Next->AttrValue[k] == 0 || !UW_AttrSupported(Next->AttrName[k]))
				{
					continue;
				}
				UGFxObject* Obj = GFXPlayer->CreateObject(TEXT("Object"));
				if(Obj)
				{
					Obj->SetString(TEXT("propName"),	UW_AttrLabel(Next->AttrName[k]));
					Obj->SetString(TEXT("propValue"),	FString::Printf(TEXT("+%d"), Next->AttrValue[k]));
					UpProps->SetElementObject(p++, Obj);
				}
			}
			if(Next->Power != 0)
			{
				UGFxObject* Obj = GFXPlayer->CreateObject(TEXT("Object"));
				if(Obj)
				{
					Obj->SetString(TEXT("propName"),	TEXT("Illusion Power"));
					Obj->SetString(TEXT("propValue"),	FString::Printf(TEXT("+%d"), Next->Power));
					UpProps->SetElementObject(p++, Obj);
				}
			}

			INT q = 0;
			for(INT k = 0; k < 2; ++k)
			{
				if(Next->ItemID[k] <= 0 || Next->ItemCnt[k] <= 0)
				{
					continue;
				}
				UGFxObject* Obj = GFXPlayer->CreateObject(TEXT("Object"));
				if(Obj == NULL)
				{
					continue;
				}
				FString Icon = TEXT("MMO_UIIcon.icon.blank");
				UItem* Tmpl = MMOFacade::getSingleton().FindItemTemplate(Next->ItemID[k]);
				if(Tmpl)
				{
					Icon = MMOFacade::getSingleton().GetItemIconNamePersonalized(Tmpl);
				}
				const INT Have = MMOFacade::getSingleton().GetItemCount(Next->ItemID[k]);
				Obj->SetString(TEXT("iconName"),	Icon);
				Obj->SetInt(TEXT("itemId"),			Next->ItemID[k]);
				Obj->SetString(TEXT("numTxt"),		(Have >= Next->ItemCnt[k])
					? FString::Printf(TEXT("%d/%d"), Have, Next->ItemCnt[k])
					: FString::Printf(TEXT("<font color='#FF4040'>%d</font>/%d"), Have, Next->ItemCnt[k]));
				UpItems->SetElementObject(q++, Obj);
			}

			Up->SetString(TEXT("level_txt"),	FString::Printf(TEXT("Lv.%d > Lv.%d"), g_UnrealWingLevel, g_UnrealWingLevel + 1));
			Up->SetObject(TEXT("propList"),		UpProps);
			Up->SetObject(TEXT("itemList"),		UpItems);
			Data->SetObject(TEXT("wingUpgradeData"), Up);
		}
	}

	Data->SetObject(TEXT("wingData"),			WingArray);
	Data->SetObject(TEXT("slotWingData"),		SlotArray);
	Data->SetObject(TEXT("propData"),			PropArray);
	Data->SetString(TEXT("expTxt"),				FString::Printf(TEXT("%d"), UW_TotalPower()));
	Data->SetString(TEXT("levelTxt"),			FString::Printf(TEXT("Wing Level: %d"), g_UnrealWingLevel));
	Data->SetString(TEXT("descTxt"),			SelectedScript);
	Data->SetInt(TEXT("settingType"),			BackShowType);
	Data->SetInt(TEXT("curSelectedWingId"),		SelectWingID);

	// Deliver piece by piece (UCUpdateWingData does the same in one call, but one failing step would abort the rest).
	eventCallUpdateWingList(WingArray);
	{
		// CLIK ScrollingList with rowHeight 0 measures the first item renderer; a renderer height of 0/NaN gives 0 rows
		UGFxObject* ListMC = GetObject(TEXT("list_wing"));
		if(ListMC)
		{
			const FLOAT RowH = ListMC->GetFloat(TEXT("rowHeight"));
			warnf(NAME_Log, TEXT("[UnrealWing] list_wing rowHeight=%f width=%f height=%f visible=%d showAll.selected=%d showOwner.selected=%d mc_avater=%d tf_dec=%d"),
				RowH, ListMC->GetFloat(TEXT("width")), ListMC->GetFloat(TEXT("height")), ListMC->GetBool(TEXT("visible")),
				GetObject(TEXT("btn_showAll")) ? GetObject(TEXT("btn_showAll"))->GetBool(TEXT("selected")) : -1,
				GetObject(TEXT("btn_showOwner")) ? GetObject(TEXT("btn_showOwner"))->GetBool(TEXT("selected")) : -1,
				GetObject(TEXT("mc_avater")) != NULL, GetObject(TEXT("tf_dec")) != NULL);
			if(!(RowH >= 1.0f))
			{
				ListMC->SetFloat(TEXT("rowHeight"), 64.0f);
				eventCallUpdateWingList(WingArray);
			}
		}
	}
	eventCallUpdateWingProps(PropArray);
	eventCallUpdateWingSlots(SlotArray);
	UGFxObject* UpObj = Data->GetObject(TEXT("wingUpgradeData"));
	if(UpObj)
	{
		eventCallUpdateWingUpgrade(UpObj);
	}
	// original one-shot call: stores cacheWingData / cacheSlotData / curSelectWingId inside the swf (see above)
	eventCallUpdateWingData(Data);
	UGFxObject* tfTotal = GetObject(TEXT("tf_totalWingPowerValue"));
	if(tfTotal)
	{
		tfTotal->SetString(TEXT("htmlText"), FString::Printf(TEXT("%d"), UW_TotalPower()));
	}
	UGFxObject* tfLevel = GetObject(TEXT("tf_wingPower"));
	if(tfLevel)
	{
		tfLevel->SetString(TEXT("htmlText"), FString::Printf(TEXT("Wing Level: %d"), g_UnrealWingLevel));
	}
	eventCallSelectWing(SelectWingID);
	UW_UpdateTitle(this);
	UW_ApplyPreview(SelectWingID);

	// ---- C++ list fallback: build the DataProvider here and assign it, independent of UpdateWingList / the swf caches
	{
		UGFxObject* ListFB = GetObject(TEXT("list_wing"));
		UGFxObject* E0 = (g_UnrealWingRows.Num() > 0) ? WingArray->GetElementObject(0) : NULL;
		warnf(NAME_Log, TEXT("[UnrealWing] wingData readback: array.length=%d element0=%d wingId=%d iconPath='%s' bOwner=%d wingType=%d"),
			WingArray->GetInt(TEXT("length")), E0 != NULL, E0 ? E0->GetInt(TEXT("wingId")) : -1,
			E0 ? *E0->GetString(TEXT("iconPath")) : TEXT(""), E0 ? E0->GetBool(TEXT("bOwner")) : -1, E0 ? E0->GetInt(TEXT("wingType")) : -1);
		if(ListFB)
		{
			ListFB->SetString(TEXT("itemRendererName"), TEXT("UnrealWing_itemRender"));
			UGFxObject* NewDP = GFXPlayer->CreateObject(TEXT("scaleform.clik.data.DataProvider"));
			if(NewDP)
			{
				INT SelIdx = 0;
				INT Pushed = 0;
				for(INT i = 0; i < g_UnrealWingRows.Num(); ++i)
				{
					if(g_UnrealWingOwnedOnly && !UW_Owned(g_UnrealWingRows(i).ID))
					{
						continue;
					}
					UGFxObject* Item = WingArray->GetElementObject(i);
					if(Item)
					{
						NewDP->SetElementObject(Pushed, Item);
						if(g_UnrealWingRows(i).ID == SelectWingID)
						{
							SelIdx = Pushed;
						}
						++Pushed;
					}
				}
				ListFB->SetObject(TEXT("dataProvider"), NewDP);
				TArray<struct FASValue> NoArgs;
				ListFB->Invoke(TEXT("invalidateData"), NoArgs);
				ListFB->Invoke(TEXT("validateNow"), NoArgs);
				ListFB->SetInt(TEXT("selectedIndex"), SelIdx);
				// the swf stores these itself inside UCUpdateWingData; assign them so its own handlers (tabs, selection) have data
				SetObject(TEXT("cacheWingData"), WingArray);
				SetObject(TEXT("cacheShowData"), NewDP);
				SetObject(TEXT("cacheSlotData"), SlotArray);
				warnf(NAME_Log, TEXT("[UnrealWing] fallback DataProvider built: length=%d selectedIndex=%d"), NewDP->GetInt(TEXT("length")), SelIdx);
			}
			else
			{
				warnf(NAME_Log, TEXT("[UnrealWing] fallback: CreateObject(scaleform.clik.data.DataProvider) failed"));
			}
		}
		UGFxObject* Av = GetObject(TEXT("mc_avater"));
		if(Av)
		{
			// the preview is drawn by MMOUIWingPanel_DrawPreview; the swf placeholder only shows as a dark rectangle at the stage origin
			Av->SetVisible(FALSE);
		}
		// The CLIK children (ItemSlot) run configUI lazily on a RENDER event that does not reach them here; until it runs their
		// design-time overlays (trade lock) stay visible. Force it for the wing slots and ask the stage for a RENDER pass.
		{
			TArray<struct FASValue> NoArgs2;
			for(INT k = 0; k < 3; ++k)
			{
				UGFxObject* SlotMC = GetObject(FString::Printf(TEXT("wm_wingPowerItem%d"), k));
				if(SlotMC)
				{
					SlotMC->Invoke(TEXT("validateNow"), NoArgs2);
					SlotMC->SetBool(TEXT("MarkLockedTrade"), TRUE);
					SlotMC->SetBool(TEXT("MarkLockedTrade"), FALSE);
				}
			}
			UGFxObject* StageMC = GetObject(TEXT("stage"));
			if(StageMC)
			{
				StageMC->Invoke(TEXT("invalidate"), NoArgs2);
			}
			warnf(NAME_Log, TEXT("[UnrealWing] children configured, stage=%d ownedOnly=%d"), StageMC != NULL, g_UnrealWingOwnedOnly);
		}
		g_UnrealWingFed = TRUE;
	}
	UGFxObject* LW = GetObject(TEXT("list_wing"));
	UGFxObject* DP = LW ? LW->GetObject(TEXT("dataProvider")) : NULL;
	{
		UGFxObject* Cont = LW ? LW->GetObject(TEXT("container")) : NULL;
		UGFxObject* CacheWing = GetObject(TEXT("cacheWingData"));
		UGFxObject* CacheShow = GetObject(TEXT("cacheShowData"));
		UGFxObject* R0 = LW ? LW->GetObject(TEXT("itemRenderer")) : NULL;
		warnf(NAME_Log, TEXT("[UnrealWing] list detail: itemRendererName='%s' itemRenderer=%d selectedIndex=%d scrollPosition=%d numChildren=%d container=%d container.numChildren=%d cacheWingData=%d(len %d) cacheShowData=%d(len %d) dp.length=%d"),
			LW ? *LW->GetString(TEXT("itemRendererName")) : TEXT("(null)"), R0 != NULL,
			LW ? LW->GetInt(TEXT("selectedIndex")) : -9, LW ? LW->GetInt(TEXT("scrollPosition")) : -9, LW ? LW->GetInt(TEXT("numChildren")) : -9,
			Cont != NULL, Cont ? Cont->GetInt(TEXT("numChildren")) : -9,
			CacheWing != NULL, CacheWing ? CacheWing->GetInt(TEXT("length")) : -9,
			CacheShow != NULL, CacheShow ? CacheShow->GetInt(TEXT("length")) : -9, DP ? DP->GetInt(TEXT("length")) : -9);
		if(LW && DP == NULL || (DP && DP->GetInt(TEXT("length")) == 0 && g_UnrealWingRows.Num() > 0))
		{
			// fallback: ask the list to rebuild itself from whatever it holds now
			TArray<struct FASValue> NoArgs;
			LW->Invoke(TEXT("invalidateData"), NoArgs);
			LW->Invoke(TEXT("validateNow"), NoArgs);
			LW->SetInt(TEXT("selectedIndex"), 0);
		}
	}
	warnf(NAME_Log, TEXT("[UnrealWing] UpdateWing pushed wings=%d slots=%d props=%d first='%d %s %s' selected=%d worn=%d list_wing=%d dataProvider=%d dpLength=%d totalValue='%s' preview=%d"),
		g_UnrealWingRows.Num(), WING_SLOT_COUNT, PropNames.Num(),
		g_UnrealWingRows.Num() ? g_UnrealWingRows(0).ID : 0,
		g_UnrealWingRows.Num() ? *g_UnrealWingRows(0).Name : TEXT(""),
		g_UnrealWingRows.Num() ? *g_UnrealWingRows(0).Icon : TEXT(""),
		SelectWingID, Worn, LW != NULL, DP != NULL, DP ? DP->GetInt(TEXT("length")) : -1,
		tfTotal ? *tfTotal->GetString(TEXT("htmlText")) : TEXT("(null)"), g_UnrealWingPreviewID);
}

//---------------------------------------------------------------------
// called by UnrealWing.swf

// called from UMMOGFXPlayer::ASRollOverItemSlot: > 0 wing item id to show the tip for, -1 = wing slot without wing (no tip), 0 = not ours
INT MMOUIWingPanel_ResolveHoverWing(UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID)
{
	if(mc == NULL || !g_UnrealWingOpen || SlotType != ESPType_Mount)
	{
		return 0;
	}
	INT Id = mc->GetInt(TEXT("itemIndex"));
	if(UW_Find(Id) == NULL)
	{
		Id = mc->GetInt(TEXT("ItemTemplateID"));
	}
	if(UW_Find(Id) == NULL)
	{
		Id = 0;
		UGFxObject* Par = mc->GetObject(TEXT("parent"));
		if(Par != NULL)
		{
			Id = Par->GetInt(TEXT("wingId"));
			if(UW_Find(Id) == NULL)
			{
				Id = 0;
			}
		}
	}
	warnf(NAME_Log, TEXT("[UnrealWing] slot hover: SlotType=%d ContainerID=%d SlotID=%d -> wing %d"), (INT)SlotType, (INT)ContainerID, SlotID, Id);
	if(Id > 0)
	{
		return Id;
	}
	return (ContainerID == WING_SLOT_CONTAINER) ? -1 : 0;
}

static INT UW_IndexToWingID(INT Index)
{
	INT n = 0;
	for(INT i = 0; i < g_UnrealWingRows.Num(); ++i)
	{
		if(g_UnrealWingOwnedOnly && !UW_Owned(g_UnrealWingRows(i).ID))
		{
			continue;
		}
		if(n == Index)
		{
			return g_UnrealWingRows(i).ID;
		}
		++n;
	}
	return 0;
}

void UMMOUIWingPanel::ASClickWingList(INT WingID)
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASClickWingList(%d)"), WingID);
	if(WingID <= 0)
	{
		UGFxObject* LW = GetObject(TEXT("list_wing"));
		const INT Idx = LW ? LW->GetInt(TEXT("selectedIndex")) : -1;
		WingID = UW_IndexToWingID(Idx);
		warnf(NAME_Log, TEXT("[UnrealWing] ASClickWingList: swf sent no id, list selectedIndex=%d -> wing %d (ownedOnly=%d)"), Idx, WingID, g_UnrealWingOwnedOnly);
		if(WingID > 0)
		{
			SetInt(TEXT("curSelectWingId"), WingID);
			eventCallSelectWing(WingID);
		}
	}
	if(UW_Find(WingID))
	{
		SelectWingID = WingID;
		UW_ApplyPreview(WingID);
	}
}

void UMMOUIWingPanel::AsClickWing(INT WingID)
{
	warnf(NAME_Log, TEXT("[UnrealWing] AsClickWing(%d)"), WingID);
	if(UW_Find(WingID))
	{
		SelectWingID = WingID;
		UW_UpdateTitle(this);
		UW_ApplyPreview(WingID);
	}
}

void UMMOUIWingPanel::ASClickWingUpgrade()
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASClickWingUpgrade()"));
	g_UWFlagUpgrade = TRUE;
	CMessageHandler::GetInstance().Send_WingOp(3, 0);
}

void UMMOUIWingPanel::ASHasNoWingTip()
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASHasNoWingTip()"));
	g_UWFlagNoTip = TRUE;
	MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Get a wing first (use a wing item)."), MsgInfoType_Warning);
}

void UMMOUIWingPanel::ASRotateWingMesh(INT Direction)
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASRotateWingMesh(%d)"), Direction);
	// Direction 0 = left, 1 = right; the preview actor is turned from the frame tick while the button is held
	g_UnrealWingTurn = (Direction == 0) ? -1 : 1;
}

void UMMOUIWingPanel::ASRotateWingMeshRelease()
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASRotateWingMeshRelease()"));
	g_UnrealWingTurn = 0;
}

void UMMOUIWingPanel::ASSwitchBackShowType(INT ShowType)
{
	warnf(NAME_Log, TEXT("[UnrealWing] ASSwitchBackShowType(%d)"), ShowType);
	// display options (cloak / cloak+chest effect / wing) are not implemented, only remembered by the panel
	BackShowType = ShowType;
}

// tooltip of a wing list item (tips type 36 in the 1.05 client)
void UMMOUIWingPanel::ASShowTipsByType(INT TipsType, class UGFxObject* mc)
{
	// logged first: the argument comes straight from the swf; the tooltip itself is not shown (the wing description is in the panel)
	warnf(NAME_Log, TEXT("[UnrealWing] ASShowTipsByType(%d, %p)"), TipsType, mc);
	if(mc != NULL)
	{
		UGFxObject* ItemSlotMC = mc->GetObject(TEXT("mw_itemSlot"));
		warnf(NAME_Log, TEXT("[UnrealWing] hovered renderer: wingId=%d mw_itemSlot=%d"), mc->GetInt(TEXT("wingId")), ItemSlotMC != NULL);
		if(ItemSlotMC != NULL)
		{
			TArray<struct FASValue> NoArgs;
			ItemSlotMC->Invoke(TEXT("validateNow"), NoArgs);
			ItemSlotMC->SetBool(TEXT("MarkLockedTrade"), TRUE);
			ItemSlotMC->SetBool(TEXT("MarkLockedTrade"), FALSE);
		}
		// standard item tooltip (same formatter as the award icons): the wing id is the wing item template id
		const INT HoverWing = mc->GetInt(TEXT("wingId"));
		if(UW_Find(HoverWing) != NULL && MMOFacade::getSingleton().FindItemTemplate(HoverWing) != NULL)
		{
			UMMOUIManager* TipMgr = MMOUI::GetUIManager();
			if(TipMgr)
			{
				mc->SetInt(TEXT("ItemTemplateID"), HoverWing);
				TipMgr->delegateShowTipsHTML(TFT_AwardIcon, mc);
			}
		}
		else
		{
			warnf(NAME_Log, TEXT("[UnrealWing] tooltip: wing %d has no item template"), HoverWing);
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------
// console diagnostics: TestShowPanelEx UWDiag 1 (UWDiag 0 also clicks the button inside the upgrade panel = sends the upgrade op)
static void UW_LogListState(UMMOUIWingPanel* P, const TCHAR* Why)
{
	UGFxObject* LW = P->GetObject(TEXT("list_wing"));
	UGFxObject* DP = LW ? LW->GetObject(TEXT("dataProvider")) : NULL;
	UGFxObject* CW = P->GetObject(TEXT("cacheWingData"));
	UGFxObject* CS = P->GetObject(TEXT("cacheShowData"));
	UGFxObject* BA = P->GetObject(TEXT("btn_showAll"));
	UGFxObject* BO = P->GetObject(TEXT("btn_showOwner"));
	warnf(NAME_Log, TEXT("[UnrealWing] diag %s: dp.length=%d cacheWing.len=%d cacheShow.len=%d showAll=%d showOwner=%d selectedIndex=%d curSelectWingId='%s' ownedOnly=%d"), Why,
		DP ? DP->GetInt(TEXT("length")) : -1, CW ? CW->GetInt(TEXT("length")) : -1, CS ? CS->GetInt(TEXT("length")) : -1,
		BA ? BA->GetBool(TEXT("selected")) : -1, BO ? BO->GetBool(TEXT("selected")) : -1, LW ? LW->GetInt(TEXT("selectedIndex")) : -1,
		*P->GetString(TEXT("curSelectWingId")), g_UnrealWingOwnedOnly);
}

void MMOUIWingPanel_Diag(UBOOL bFull)
{
	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUIWingPanel* P = UM ? Cast<UMMOUIWingPanel>(UM->FindUIPanel(FName(TEXT("UnrealWing")))) : NULL;
	if(P == NULL || !P->GetVisible())
	{
		warnf(NAME_Log, TEXT("[UnrealWing] diag: open the wing window first"));
		return;
	}
	TArray<struct FASValue> NoArgs;
	TArray<struct FASValue> OneArg;
	FASValue NullArg(EC_EventParm);
	OneArg.AddItem(NullArg);

	FASValue Has = P->Invoke(TEXT("IsPlayerHasWing"), NoArgs);
	warnf(NAME_Log, TEXT("[UnrealWing] diag IsPlayerHasWing -> type=%d b=%d i=%d"), (INT)Has.Type, (INT)Has.B, Has.I);
	UW_LogListState(P, TEXT("start"));

	UGFxObject* BA = P->GetObject(TEXT("btn_showAll"));
	UGFxObject* BO = P->GetObject(TEXT("btn_showOwner"));
	if(BA && BO)
	{
		BO->SetBool(TEXT("selected"), TRUE);
		P->Invoke(TEXT("HandleClickShowWingType"), OneArg);
		UW_LogListState(P, TEXT("after Owned + swf handler"));
		BA->SetBool(TEXT("selected"), TRUE);
		P->Invoke(TEXT("HandleClickShowWingType"), OneArg);
		UW_LogListState(P, TEXT("after All + swf handler"));
	}

	g_UWFlagNoTip = FALSE;
	g_UWFlagUpgrade = FALSE;
	P->Invoke(TEXT("HandleClickUpgradeWing"), OneArg);
	warnf(NAME_Log, TEXT("[UnrealWing] diag after HandleClickUpgradeWing: ASHasNoWingTip called=%d ASClickWingUpgrade called=%d"), g_UWFlagNoTip, g_UWFlagUpgrade);
	if(bFull)
	{
		UGFxObject* Up = P->GetObject(TEXT("upgradeMc"));
		if(Up)
		{
			Up->Invoke(TEXT("OnUpgradeClick"), OneArg);
			warnf(NAME_Log, TEXT("[UnrealWing] diag after upgradeMc.OnUpgradeClick: ASClickWingUpgrade called=%d (op 3 sent to the server)"), g_UWFlagUpgrade);
		}
	}
}
