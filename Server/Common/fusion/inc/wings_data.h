#ifndef _WINGS_DATA_H_
#define _WINGS_DATA_H_

#include "data_manager.h"

class CTabFile;
// 1.05 wings.txt (item ID of the wing item is the wing ID)
struct WINGS_ENTRY
{
	INT32 m_nID;
	INT32 m_nAttributeID;       // attribute category (catXXX) granted while the wing is worn
	INT32 m_nAttributeValue;
	INT32 m_nPower;

	static BOOL ReadLine(INT32& rnID, const char*& rpcszName, WINGS_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow);
};

// wing panel (1.05 UnrealWing): the wing level and the owned wings are kept in two role variables
#define ROLE_VAR_WING_LEVEL		2046	// wing level, 0 .. last row of wings_level.txt
#define ROLE_VAR_WING_MASK		2047	// owned wings: bit N <=> wing id (WING_ID_BASE + N) of wings.txt
#define WING_ID_BASE			39501
#define WING_ID_BITS			31

// 1.05 wings_level.txt : a row holds what the upgrade TO that level costs and gives (bonuses stack over the levels)
struct WINGS_LEVEL_ENTRY
{
	INT32 m_nLevel;
	INT32 m_nPower;				// "illusion power" the level adds to the panel total
	INT32 m_nAttributeID[2];
	INT32 m_nAttributeValue[2];
	INT32 m_nConsumeItemID[2];
	INT32 m_nConsumeItemCnt[2];

	static BOOL ReadLine(INT32& rnID, const char*& rpcszName, WINGS_LEVEL_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow);
};

#endif // _WINGS_DATA_H_
