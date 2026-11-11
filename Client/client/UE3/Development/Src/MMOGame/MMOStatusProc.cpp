#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOSkillManager.h"
#include "MMOChatManager.h"
#include "FSMIO.h"

IMPLEMENT_CLASS(UIMMOStatusProc);
IMPLEMENT_CLASS(UMMOStatusProc_AttachParticle);
IMPLEMENT_CLASS(UMMOStatusProc_AttachLight);
IMPLEMENT_CLASS(UMMOStatusProc_EnabledCollision);
IMPLEMENT_CLASS(UMMOStatusProc_ExplodeSkelAnim);
IMPLEMENT_CLASS(UMMOStatusProc_HideBody);
IMPLEMENT_CLASS(UMMOStatusProc_PlayAnimation);
IMPLEMENT_CLASS(UMMOStatusProc_PlayMatinee);
IMPLEMENT_CLASS(UMMOStatusProc_Ragdoll);
IMPLEMENT_CLASS(UMMOStatusProc_SetAvatarAnim);
IMPLEMENT_CLASS(UMMOStatusProc_SetMaterial);
IMPLEMENT_CLASS(UMMOStatusProc_SetMaterialAphla);
IMPLEMENT_CLASS(UMMOStatusProc_SetMMOAttach);
IMPLEMENT_CLASS(UMMOStatusProc_SetObstacelEnable);
IMPLEMENT_CLASS(UMMOStatusProc_ShowBody);
IMPLEMENT_CLASS(UMMOStatusProc_ShowSceneHint);
IMPLEMENT_CLASS(UMMOStatusProc_SpawnActor);
IMPLEMENT_CLASS(UMMOStatusProc_SpawnDecal);
IMPLEMENT_CLASS(UMMOStatusProc_SpawnParticle);
IMPLEMENT_CLASS(UMMOStatusProc_UseAbility);
IMPLEMENT_CLASS(UMMOStatusProc_SetWeaponState);
IMPLEMENT_CLASS(UMMOStatusProc_SetPickable);
IMPLEMENT_CLASS(UMMOStatusProc_ShowWeapon);
IMPLEMENT_CLASS(UMMOStatusProc_ShowNameHint);
IMPLEMENT_CLASS(UMMOStatusProc_LoopSkillSlot);
IMPLEMENT_CLASS(UMMOStatusProc_SetSkillSlotIcon);
IMPLEMENT_CLASS(UMMOStatusProc_ExchangeSkill);
IMPLEMENT_CLASS(UMMOStatusProc_AddMessage);
IMPLEMENT_CLASS(UMMOStatusProc_SetNameHint);
IMPLEMENT_CLASS(UMMOStatusProc_SetAlwaysWalk);

void UMMOStatusProc_AttachParticle::Update(AMMOActor* actor)
{
	//local ParticleSystemComponent sfx;
	//local array<SkeletalMeshComponent> wpMesh;
	//local int i;

	//if(BindToWeapon)	
	//{
	//	wpMesh = actor.GetClientWeaponSkMesh();
	//}
	//else	
	//{
	//	wpMesh.AddItem(actor.Mesh);
	//}

	//if(EffectTemplate != none)
	//{
	//	for(i = 0; i < wpMesh.Length; ++i)
	//	{
	//		sfx = new( self ) class'ParticleSystemComponent';
	//		sfx.SetTemplate(EffectTemplate);
	//		sfx.SetScale(EffectScale);			
	//		wpMesh[i].AttachComponentToSocket(sfx, SocketName);			
	//		sfx.SetSkeletalMeshComponentParameter(VertSurfaceActorParameterName, wpMesh[i]);
	//		sfx.ActivateSystem();
	//	}
	//}	
}

void UMMOStatusProc_AttachLight::Update(AMMOActor* actor){}
void UMMOStatusProc_EnabledCollision::Update(AMMOActor* actor){}
void UMMOStatusProc_ExplodeSkelAnim::Update(AMMOActor* actor){}
void UMMOStatusProc_HideBody::Update(AMMOActor* actor){}
void UMMOStatusProc_PlayAnimation::Update(AMMOActor* actor){}
void UMMOStatusProc_PlayMatinee::Update(AMMOActor* actor){}
void UMMOStatusProc_PlayMatinee::NativeOnExecute(AMMOActor* actor)
{
	//MMOFacade::getSingleton().PauseBackSound(TRUE);
}

void UMMOStatusProc_Ragdoll::Update(AMMOActor* actor){}
void UMMOStatusProc_SetAvatarAnim::Update(AMMOActor* actor){}
void UMMOStatusProc_SetMaterial::Update(AMMOActor* actor){}
void UMMOStatusProc_SetMaterialAphla::Update(AMMOActor* actor)
{
	int nNum;
	//float value = 0;
	UMaterialInterface* mi;
	UMaterialInstanceTimeVarying* mitv;
	UMaterialInstanceConstant* mic;

	Value = Value + 0.1;
	if(Value > 10)
		Value = Value - 20;
	nNum = actor->Mesh->GetNumElements();
	for (int i = 0; i < nNum; i++)
	{
		mi = actor->Mesh->GetMaterial(i);
		mitv = Cast<UMaterialInstanceTimeVarying>(mi);
		mic = Cast<UMaterialInstanceConstant>(mi);
		if(mic)
		{
			mic->SetScalarParameterValue(FName("HPPercent"), Value);
			//`log("________________ SetColor MaterialInstanceConstant" @mi @clrl.A @clrl.R @clrl.G @clrl.B);

		}
		else if(mitv)
		{
			//`log("________________ SetColor MaterialInstanceTimeVarying" @mi @mitv.VectorParameterValues.Length);
			mitv->SetScalarParameterValue(FName("HPPercent"), Value);
		}
		else
		{
			//`log("________________ SetColor " @mi);
		}
	}
}
void UMMOStatusProc_SetMMOAttach::Update(AMMOActor* actor){}
void UMMOStatusProc_SetObstacelEnable::Update(AMMOActor* actor){}
void UMMOStatusProc_ShowBody::Update(AMMOActor* actor){}
void UMMOStatusProc_ShowSceneHint::Update(AMMOActor* actor){}
void UMMOStatusProc_SpawnActor::Update(AMMOActor* actor){}
void UMMOStatusProc_SpawnDecal::Update(AMMOActor* actor){}
void UMMOStatusProc_SpawnParticle::Update(AMMOActor* actor){}
void UMMOStatusProc_UseAbility::Update(AMMOActor* actor){}
void UMMOStatusProc_SetWeaponState::Update(AMMOActor* actor){}
void UMMOStatusProc_SetPickable::Update(AMMOActor* actor){}
void UMMOStatusProc_ShowWeapon::Update(AMMOActor* actor){}
void UMMOStatusProc_ShowNameHint::Update(AMMOActor* actor){}
void UMMOStatusProc_LoopSkillSlot::Update(AMMOActor* actor){}
void UMMOStatusProc_SetSkillSlotIcon::Update(AMMOActor* actor){}
// 1.05 backport: ExchangeSkill is a script-only no-op for now (see MMOStatusProc_ExchangeSkill.uc)
void UMMOStatusProc_ExchangeSkill::Update(AMMOActor* actor){}
void UMMOStatusProc_AddMessage::Update(AMMOActor* actor){}
void UMMOStatusProc_SetNameHint::Update(AMMOActor* actor){}
void UMMOStatusProc_SetAlwaysWalk::Update(AMMOActor* actor){}

void UMMOStatusProc_LoopSkillSlot::NativeOnExecute(AMMOActor* actor)
{
	if(actor && actor->IsMainPlayer())
	{

		if(nType==0){
			MMOSkillManager::getSingleton().SetSkillSlotActive(AbilityId,bActive);
		}
		else if(nType==1)
		{
			MMOSkillManager::getSingleton().SetSkillSlotFlag1(AbilityId,bActive);
		}
		MMOGameEvent_OneInt evt( GameEvent_Refresh_SkillSlotLoop, AbilityId );
		MMOUI::PostGameEvent(&evt);
	}
}

void UMMOStatusProc_SetSkillSlotIcon::NativeOnExecute(AMMOActor* actor)
{
	if(actor && actor->IsMainPlayer())
	{
		MMOSkillManager::getSingleton().SetSkillSlotIcon(AbilityId,IconName);

		MMOGameEvent_Any evt(GameEvent_Refresh_SkillSlotIcon);
		MMOUI::PostGameEvent(&evt);
	}
}

void UMMOStatusProc_AddMessage::NativeOnExecute(AMMOActor* actor)
{
	if(actor && actor->IsMainPlayer())
	{
		if (MMO::GetCurrentGameState() == EGameState_Login || MMO::GetCurrentGameState() == EGameState_Selecting || MMO::GetCurrentGameState() == EGameState_Loading)
		{
			return;
		}

		if(ScreenMessage.Len() )
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( ScreenMessage , MsgInfoType_Normal, ScreenMessage );
		}
		if(SystemMessage.Len() )
		{
			MMOChatManager::getSingleton().AddSystemMessage( SystemMessage  );
		}
	}
}

void UMMOStatusProc_SetNameHint::NativeOnExecute(AMMOActor* actor)
{
	if(actor&&actor->ActorAttributes)
	{
		if(DeleteFlag==FALSE){
			if(actor->ActorAttributes->FrameEx1.Len()==0 && actor->ActorAttributes->ColorEx1.Len()==0)
			{
				actor->ActorAttributes->FrameEx1= NameHintFrame;
				actor->ActorAttributes->ColorEx1= NameHintColor;
			}
			else if(actor->ActorAttributes->FrameEx1==NameHintFrame && actor->ActorAttributes->ColorEx1==NameHintColor)
			{
			}
			else
			{
				actor->ActorAttributes->FrameEx2= NameHintFrame;
				actor->ActorAttributes->ColorEx2= NameHintColor;
			}
		}
		else
		{
			if(actor->ActorAttributes->FrameEx1==NameHintFrame && actor->ActorAttributes->ColorEx1==NameHintColor)
			{
				actor->ActorAttributes->FrameEx1= actor->ActorAttributes->FrameEx2;
				actor->ActorAttributes->ColorEx1= actor->ActorAttributes->ColorEx2;
				actor->ActorAttributes->FrameEx2= TEXT("");
				actor->ActorAttributes->ColorEx2= TEXT("");
			}
			else if(actor->ActorAttributes->FrameEx2==NameHintFrame && actor->ActorAttributes->ColorEx2==NameHintColor)
			{
				actor->ActorAttributes->FrameEx2= TEXT("");
				actor->ActorAttributes->ColorEx2= TEXT("");
			}
		}
		MMOUI::GetUIManager()->delegateUpdateHintText(actor);
	}
}

void UMMOStatusProc_SetAlwaysWalk::NativeOnExecute(AMMOActor* actor)
{

	FSMIO::getSingleton().Ctrl.SetAlwaysWalk(bAlwaysWalk);

}
