#ifndef _SPELL_H_
#define _SPELL_H_

#include "spell_def.h"
#include "spell_data.h"
#include "char_def.h"	// GetClassSpellMask

class CCharacter;

class CSpell
{
public:
	CSpell(void);
	virtual ~CSpell(void);
	virtual void Release(void) { }

	virtual void SetAttr(INT32 eAttrType, INT32 nVal);
	virtual CSpell* MakeCopy(void) = 0;

	inline void SetIDLevelRune(INT32 nID, INT32 nLevel, INT32 nRuneID);
	inline INT32 GetID(void);
	inline INT32 GetLevel(void);
	inline INT32 GetRuneSlot(void);
	inline INT32 GetAttr(INT32 eAttrType);

	inline INT32 GetLengthAttr(INT32 eAttrType);
	inline INT32 GetAngleAttr(INT32 eAttrType);

	inline void SetX(INT32 nIndex, INT32 nValue);
	inline ExtraParam* GetExParam(void);

	inline void CopyTo(CSpell* p);
	inline void CopyData(SPELL_ENTRY* pData);

	inline SPELL_ENTRY* GetData(void);

	inline INT32 GetSelfCoolDownFrm(void);
	inline INT32 GetEventCoolDownFrm(void);

	INT32 GetConsumeAttr(INT32 eAttrType);

	inline BOOL CanBindSlot(INT32 nSlotID);
	inline BOOL CanShowOnUI(void);
	inline BOOL CanLearnByClass(INT32 nClass);

protected:
	SPELL_ENTRY m_Data;
	ExtraParam m_Param;
};

class CSpellTemplate : public CSpell
{
public:
	CSpellTemplate(void);
	virtual ~CSpellTemplate(void) { }

	virtual void SetAttr(INT32 eAttrType, INT32 nVal);
	virtual CSpell* MakeCopy(void);

	inline void InitAttr(INT32 eAttrType, INT32 nVal);
	inline BOOL IsInited(void);
	inline void ResetFlag(void);

	void InitWithData(const SPELL_ENTRY* pData);
private:
	BOOL m_bInited;
};

class CSpellCopy : public CSpell
{
public:
	CSpellCopy(void);
	virtual ~CSpellCopy(void) { }
	virtual void Release(void) { delete this; }

	virtual CSpell* MakeCopy(void);
};

#include "spell_inl.h"

#endif	// _SPELL_H_
