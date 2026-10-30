#include "stdafx.h"
#include "simple_timer.h"

CSimpleTimer::CSimpleTimer(void)
    : m_bEnabled(false), m_StartTime(0), m_NextTrigger(0), m_nRemainTrigTimes(0), m_Interval(0)
{
}

void CSimpleTimer::Enable(UINT32 nInterval, UINT32 nTrigTimes, UINT32 nNow, UINT32 nDelay)
{
    Enable(FrameDuration{nInterval}, nTrigTimes, FrameDuration{nNow}, FrameDuration{nDelay});
}

void CSimpleTimer::Enable(FrameDuration interval, UINT32 triggerCount, FrameDuration now, FrameDuration delay)
{
    m_StartTime = now;
    m_Interval = interval;
    m_nRemainTrigTimes = triggerCount;
    m_NextTrigger = now + delay;
    m_bEnabled = true;
}

void CSimpleTimer::Disable(void)
{
	m_bEnabled = false;
}

bool CSimpleTimer::Trig(UINT32 nNow)
{
    return Trig(FrameDuration{nNow});
}

bool CSimpleTimer::Trig(FrameDuration now)
{
	if(m_nRemainTrigTimes <= 0)
	{
		Disable();
		return false;
	}

	if(!m_bEnabled || m_NextTrigger > now)
		return false;

	m_nRemainTrigTimes--;

	m_NextTrigger = now + m_Interval;

	return true;
}

bool CSimpleTimer::TrigOver(void)
{
	return m_nRemainTrigTimes <= 0;
}

UINT32 CSimpleTimer::GetRemainTime(void)
{
    return GetRemainingFrames().count();
}

CSimpleTimer::FrameDuration CSimpleTimer::GetRemainingFrames() const
{
    if (!m_bEnabled)
        return FrameDuration::zero();
    return m_Interval * m_nRemainTrigTimes;
}
