#ifndef CQYH_BASELIB_SIMPLE_TIMER_H
#define CQYH_BASELIB_SIMPLE_TIMER_H

#include <chrono>
#include "std_type.h"
#include "scene_def.h"

class CSimpleTimer
{
public:
	// All callers supply simulation frames, at GAME_FPS frames per second.
	using FrameDuration = std::chrono::duration<UINT32, std::ratio<1, GAME_FPS>>;

	CSimpleTimer(void);

	void Enable(UINT32 nInterval, UINT32 nTrigTimes, UINT32 nNow, UINT32 nDelay);
	void Enable(FrameDuration interval, UINT32 triggerCount, FrameDuration now, FrameDuration delay);

	void Disable(void);

	bool Trig(UINT32 nNow);
	bool Trig(FrameDuration now);

	UINT32 GetRemainTime(void);
	FrameDuration GetRemainingFrames() const;

	bool TrigOver(void);

private:
	bool m_bEnabled;
	FrameDuration m_StartTime;
	FrameDuration m_NextTrigger;
	UINT32 m_nRemainTrigTimes;
	FrameDuration m_Interval;
};

#endif // CQYH_BASELIB_SIMPLE_TIMER_H
