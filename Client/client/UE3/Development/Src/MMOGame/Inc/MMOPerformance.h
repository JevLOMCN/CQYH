/**
 *	PerformanceBalancer.cpp: auto adjust the options to improve user experience
 *	Copyright 1998-2014 Shanda Games, Inc. All Rights Reserved.
 */

/*
 * zhaojun@snda, 2014-6-30, first edition
*/
#pragma once

#include "MMOSingleton.h"

void AutoLowScaleLevel(bool scaleDown);

class MMOPerformanceBalancer : public Singleton<MMOPerformanceBalancer>
{

private:

	enum eBottleNeckType
	{
		BOTTLENECK_GPU,
		BOTTLENECK_GAMETHREAD,
		BOTTLENECK_RENDERTHREAD,
		BOTTLENECK_COUNT,
	};

	enum eBottleNeckEntry
	{
		// game-thread
		SKELETAL_UPDATE,
		LINECHECK,
		PARTICLE_UPDATE,

		// render-thread

		// gpu
		SCENE_LOD,
		MATERIAL_QUALITY,

		// other
		BN_ALL,

		BN_COUNT,
	};

	enum eRankEnum
	{
		RANK_0_20 = 0,
		RANK_20_30,
		RANK_30_45,
		RANK_45_60,
		RANK_60_INFINITY,
		RANK_COUNT,
	};

	typedef INT (*fnEntryAdjustInterface)(INT DesireValue);	// return the old value 

	class EntryData
	{
	public:
		eBottleNeckEntry		eBottleNeckDetail;

		INT32					nDefaultValue;
		INT32					nCurValue;
		INT32					nDesireValue;

		fnEntryAdjustInterface	pfnAdjust;

	public:

		EntryData()
		{
			Init(BN_COUNT, NULL);
		}

		EntryData(eBottleNeckEntry eBottleNeckDetail, fnEntryAdjustInterface pfnAdjust)
		{
			Init(eBottleNeckDetail, pfnAdjust);
		}

		void Init(eBottleNeckEntry eBottleNeckDetail, fnEntryAdjustInterface pfnAdjust)
		{
			EntryData::eBottleNeckDetail = eBottleNeckDetail;
			nDefaultValue = -1;
			nCurValue = -1;
			nDesireValue = -1;
			EntryData::pfnAdjust = pfnAdjust;
		}

		void Adjust(INT DesireValue)
		{
			nDesireValue = DesireValue;
		}

		void Apply()
		{
			if (pfnAdjust != NULL && nDesireValue != nCurValue)
			{
				nCurValue = pfnAdjust(nDesireValue);
			}
		}

		void Restore()
		{
			if (pfnAdjust != NULL && nDefaultValue != nCurValue)
			{
				nCurValue = pfnAdjust(nDefaultValue);
			}
		}

		void FetchValue()
		{
			if (pfnAdjust != NULL)
			{
				(pfnAdjust)(-1);
			}
		}
	};

public:

	// although it is a singleton
	MMOPerformanceBalancer(FLOAT fTickInterval = 3.0f)
	{
		_fTickInterval = fTickInterval;
		_fNextTickTime = 0;
		Enable();
		Initialize();
	}

	~MMOPerformanceBalancer()
	{
		ResetToUserDefine();
		Finalize();
	}

private:

	MMOPerformanceBalancer(const MMOPerformanceBalancer& rhs)
	{
		// not permit
	}

	MMOPerformanceBalancer& operator=(const MMOPerformanceBalancer& rhs)
	{
		// not permit
		return *this;
	}

public:
	void Tick(FLOAT delta);

	void Enable(){_bEnable = TRUE;}
	void Disable(){_bEnable = FALSE;}
	UBOOL IsEnable() const {return _bEnable;}
	void OnOptionChanged();

private:
	void Initialize();
	void Finalize();
	void LocateBottleNeck(UINT& nGameThread, UINT& nRenderThread, UINT& nGPU);
	
	static void Adjust(TArray<EntryData*>& EntryArray, INT DesireValue)
	{
		for (INT n = 0; n < EntryArray.Num(); ++n)
		{
			EntryData* entry = EntryArray(n);
			entry->Adjust(DesireValue);
		}
	}

	static void Apply(EntryData* EntryArray, UINT EntryCount)
	{
		for (UINT n = 0; n < EntryCount; ++n)
		{
			EntryData& entry = EntryArray[n];
			entry.Apply();
		}
	}

	static void FetchValue(EntryData* EntryArray, UINT EntryCount)
	{
		for (UINT n = 0; n < EntryCount; ++n)
		{
			EntryData& entry = EntryArray[n];
			entry.FetchValue();
		}
	}

	static void RestorDefault(EntryData* EntryArray, UINT EntryCount)
	{
		for (UINT n = 0; n < EntryCount; ++n)
		{
			EntryData& entry = EntryArray[n];
			entry.Restore();
		}
	}

private:
	void ResetToUserDefine();

private:
	FLOAT _fTickInterval;
	FLOAT _fNextTickTime;

	class FPSRecorder
	{
	public:
		eRankEnum eCurLevel;
		eRankEnum eTargetLevel;
		float fAccTime;

	public:
		FPSRecorder()
		{
			eCurLevel = (RANK_60_INFINITY);
			eTargetLevel = (RANK_60_INFINITY);
			fAccTime = 0;
		}

		eRankEnum Update(eRankEnum e, float fDeltaTime, float fOverTime = 10.0f)
		{
			if (e < eCurLevel && eTargetLevel < eCurLevel)
			{
				eTargetLevel = (e < eTargetLevel ? eTargetLevel : e);
				fAccTime += fDeltaTime;
			}
			else if (e > eCurLevel && eTargetLevel > eCurLevel)
			{
				eTargetLevel = (e < eTargetLevel ? e : eTargetLevel);
				fAccTime += fDeltaTime;
			}
			else
			{
				eTargetLevel = e;
				fAccTime = 0;
			}

			if (fAccTime > fOverTime && eCurLevel != eTargetLevel)
			{
				eCurLevel = eTargetLevel;
				fAccTime = 0;
				return eTargetLevel;
			}
			else
			{
				return RANK_COUNT;
			}
		}
	};

	FPSRecorder _fpsRecorder[BOTTLENECK_COUNT];

private:
	// user defines
	UBOOL	_bEnable;

private:

	EntryData*			_pArrayAllEntry;

	TArray<EntryData*>	_ArrayBottleNeckEntry[BOTTLENECK_COUNT];

};


class MMOMemPerformanceProfiler : public UMemPerformanceProfiler
{
public:
	virtual void OnEntryCreate(eMemPerformanceProfilerType type, const FString& filename, UINT size);
	virtual void OnEntryDestroy(eMemPerformanceProfilerType type, const FString& filename, UINT size);
	virtual INT RenderToScreen(FViewport* Viewport, FCanvas* Canvas, INT X, INT Y, 
		UMemPerformanceProfiler::eMemPerformanceProfilerType etype);

	virtual void RenderToFile(){}

protected:

	typedef TMap<FString, UINT> TMapMemPerf;

	TMapMemPerf m_memSize[MPPT_COUNT];

};

extern MMOMemPerformanceProfiler g_mmoTexturePerfProfiler;



