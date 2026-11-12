/**
 * jimZhang@sdna.com 2013.12.4
 *
 */

#ifndef __MMOFULLSCREENEFFECTMANAGER_H__
#define __MMOFULLSCREENEFFECTMANAGER_H__

#include <list>
#include <vector>
#include <map>

#include "MMOUtil.h"

enum FullScreenEffectType
{
	FSET_LowHP=0,
	FSET_Total
};

class MMOFullScreenEffectManager : public SingletonMeyers<MMOFullScreenEffectManager>
{
public:
	MMOFullScreenEffectManager();
	virtual ~MMOFullScreenEffectManager();

	void Init();
	void Shutdown();
	void Clear();

	void StartEffect(FullScreenEffectType EffectType);	// start effect
	void StopEffect(FullScreenEffectType EffectType);	// stop effect
	void StopAllEffect();	

protected:
	UBOOL     _FullScreenEffectTypeOpen[FSET_Total];
	FString   _FullScreenEffectTypeName[FSET_Total];


};

#endif