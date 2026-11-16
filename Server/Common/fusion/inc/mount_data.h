#ifndef _MOUNT_DATA_H_
#define _MOUNT_DATA_H_

#include "data_manager.h"
#include "mount_def.h"

class CTabFile;
struct MOUNT_ENTRY
{
	INT32 m_nID;
	INT32 m_nLevelLimit;
	INT32 m_nSpeedModificationRate;
	INT32 m_nHitUnmountRate;

	INT32 m_nAuraID;

	// 1.05 mount book
	INT32 m_nMountPower;		// beast power points this mount adds once it is owned
	INT32 m_nSoulSpellID;		// SoulAuraID of mount.txt: the mount skill (a spell) that can be put into a beast soul slot

	INT32 m_nItemID[mftCount];
	INT32 m_nItemCount[mftCount];

	static BOOL ReadLine(INT32& rnID, const char*& rpcszName, MOUNT_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow);
};

// 1.05 mount_power.txt : beast power. The row ID is the sum of MountPower of the owned mounts; the row gives the beast power
// level (Lap), the number of unlocked beast soul slots and the attributes granted in total at that point.
#define MOUNT_POWER_ATTR_COUNT		6
#define MOUNT_SOUL_MAX_SLOTS		8
// the equipped beast souls (mount ids, 6 bits each) are kept in two role variables: slots 0..4 in A, 5..7 in B
#define ROLE_VAR_MOUNT_SOUL_A		2044
#define ROLE_VAR_MOUNT_SOUL_B		2045

struct MOUNT_POWER_ENTRY
{
	INT32 m_nID;
	INT32 m_nLap;
	INT32 m_nMaxSoulSlotCount;
	INT32 m_nAttributeID[MOUNT_POWER_ATTR_COUNT];
	INT32 m_nAttributeValue[MOUNT_POWER_ATTR_COUNT];
	INT32 m_nPower;

	static BOOL ReadLine(INT32& rnID, const char*& rpcszName, MOUNT_POWER_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow);
};

#endif // _mount_data_h__
