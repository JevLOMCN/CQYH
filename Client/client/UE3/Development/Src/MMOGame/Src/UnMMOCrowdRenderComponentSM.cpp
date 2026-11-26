#include "stdafx.h"
#include "CrowdParticleData.h"
#include "EngineParticleClasses.h"

IMPLEMENT_CLASS(UMMOCrowdRenderComponentSM);

void UMMOCrowdRenderComponentSM::Attach()
{
	Super::Attach();
	PSModule = NULL;

	OwnerSystem = Cast<AMMOCrowdSystem>( GetOwner() );
	if( OwnerSystem && Template )
		Init( OwnerSystem );
}

void UMMOCrowdRenderComponentSM::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("MMOCrowdRenderComponentSM");
	Super::Tick(DeltaTime);
	UpdateCrowdParticleData(PSModule, OwnerSystem);
}

// set particle parameters and fetch crowd system location module
void UMMOCrowdRenderComponentSM::Init(const AMMOCrowdSystem* cs)
{
	PSModule = NULL;
	if (!cs || !EmitterInstances.IsValidIndex(0) || !EmitterInstances(0))
		return;

	UParticleSpriteEmitter* pse = EmitterInstances(0)->SpriteTemplate;
	if (!pse || !pse->LODLevels.IsValidIndex(0) || !pse->LODLevels(0))
		return;

	for (INT i = 0; i < pse->LODLevels(0)->Modules.Num(); ++i)
	{
		UParticleModuleLocationCrowdSystem* module = Cast<UParticleModuleLocationCrowdSystem>(pse->LODLevels(0)->Modules(i));
		if (module)
			PSModule = module;
	}
	UpdateCrowdParticleData(PSModule, cs);
}