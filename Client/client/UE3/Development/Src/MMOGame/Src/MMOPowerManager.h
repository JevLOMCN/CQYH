
#ifndef _MMOPowerManager_H_
#define _MMOPowerManager_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"

enum EPowerPart
{
	PowerPart_SuitPow,
	PowerPart_HeavenPow,
	PowerPart_TitlePow,
	PowerPart_EquipSlotEnhancePow,
	PowerPart_AllEquippedPow,
	PowerPart_CharacterBasePow,
	PowerPart_SkillPow,
};

class MMOPowerPart;

typedef TMap< INT , MMOPowerPart* > PowerPartMap;

class MMOPowerManager: public SingletonMeyers<MMOPowerManager>
{
public:

	MMOPowerManager();
	virtual ~MMOPowerManager();

	void Init();
	void UnInit();

	void Reset();

	void UpdatePartPower( EPowerPart type );

	//combat power for a single weapon only
	void UpdateEquipPower( FClientItemInfo* item );

	INT GetPower();

protected:

	PowerPartMap _powerPartMap;

	void _NotifyUpdateUIPower();
public:
	static const INT EQUIP_LV_UP_POWER_FACTER = 5; 
};

#endif