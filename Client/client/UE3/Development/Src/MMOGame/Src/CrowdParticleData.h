#ifndef MMO_CROWD_PARTICLE_DATA_H
#define MMO_CROWD_PARTICLE_DATA_H

#include "EngineParticleClasses.h"
#include "MMOGameClasses.h"

inline void UpdateCrowdParticleData(UParticleModuleLocationCrowdSystem* module, const AMMOCrowdSystem* system)
{
	if (!module)
		return;

	const INT count = system ? system->CrowdInstanceData.Num() : 0;
	if (module->ParticleLocations.Num() != count)
		module->ParticleLocations.SetNum(count);
	if (module->ParticleRotations.Num() != count)
		module->ParticleRotations.SetNum(count);

	for (INT i = 0; i < count; ++i)
	{
		const UMMOCrowdInstance* instance = system->CrowdInstanceData(i);
		module->ParticleLocations(i) = instance ? instance->Location : FVector(0, 0, 0);
		module->ParticleRotations(i) = instance ? instance->Direction : FVector(0, 0, 0);
	}
}

#endif
