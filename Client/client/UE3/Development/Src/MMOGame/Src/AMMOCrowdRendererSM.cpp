#include "stdafx.h"
#include "CrowdParticleData.h"

IMPLEMENT_CLASS(AMMOCrowdRendererSM);

void AMMOCrowdRendererSM::Init()
{
	PSModule = NULL;
	if (!ParticleSystemComponent || !ParticleSystemComponent->EmitterInstances.IsValidIndex(0) ||
		!ParticleSystemComponent->EmitterInstances(0))
		return;

	UParticleSpriteEmitter* pse = ParticleSystemComponent->EmitterInstances(0)->SpriteTemplate;
	if (!pse || !pse->LODLevels.IsValidIndex(0) || !pse->LODLevels(0))
		return;

	for (INT i = 0; i < pse->LODLevels(0)->Modules.Num(); ++i)
	{
		PSModule = Cast<UParticleModuleLocationCrowdSystem>(pse->LODLevels(0)->Modules(i));
		if (PSModule)
			break;
	}
	UpdateCrowdParticleData(PSModule, CrowdSystem);
}