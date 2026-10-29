#ifndef CQYH_BASELIB_TIME_EX_H
#define CQYH_BASELIB_TIME_EX_H

#include "std_type.h"

struct tm_ex
{
	INT32 tm_msec;	/* millisecond after the second - [0,999]*/
	INT32 tm_sec;     /* seconds after the minute - [0,59] */
	INT32 tm_min;     /* minutes after the hour - [0,59] */
	INT32 tm_hour;    /* hours since midnight - [0,23] */
	INT32 tm_mday;    /* day of the month - [1,31] */
	INT32 tm_mon;     /* months since January - [1,12] */
	INT32 tm_year;    /* years (NOT since 1900) */
	INT32 tm_wday;    /* days since Sunday - [0,6] */
};

// Returned storage belongs to the calling thread and is reused on its next call.
extern const tm_ex* localtime_ex(void);
extern bool is_leap_year(int nYear);
// Month is zero-based (January = 0). Invalid months return zero.
extern int get_month_day_count(int nYear, int nMonth);
// Preserve Windows uptime and Unix epoch millisecond origins for existing callers.
extern UINT64 GetTickCountEx(void);
// Uses the CRT's local timezone, with per-thread result storage.
extern const char* get_time_str(UINT32 dwTime);
// Preserve the existing standard-offset calculation, including its DST convention.
extern int get_time_zone(void);

#endif // CQYH_BASELIB_TIME_EX_H
