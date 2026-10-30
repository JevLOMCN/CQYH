#include <Windows.h>
#include "std_type.h"
#include "simple_timer.h"
#include "time_ex.h"

#include <array>
#include <barrier>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <optional>
#include <string>
#include <thread>
#include <utility>

namespace
{
int checks = 0;
int failures = 0;
void check(bool condition, const char* description)
{
    ++checks;
    if (!condition)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}

struct TimezoneScope
{
    std::optional<std::string> previous;
    TimezoneScope()
    {
        if (const auto* value = std::getenv("TZ"))
            previous = value;
    }
    ~TimezoneScope()
    {
        _putenv_s("TZ", previous ? previous->c_str() : "");
        _tzset();
    }
    void set(const char* value)
    {
        check(_putenv_s("TZ", value) == 0, "test timezone is set");
        _tzset();
    }
};

std::string reference_time(UINT32 timestamp)
{
    const auto seconds = static_cast<std::time_t>(timestamp);
    std::tm local{};
    if (::localtime_s(&local, &seconds) != 0)
        return std::to_string(timestamp);
    std::array<char, 64> buffer{};
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S", &local);
    return buffer.data();
}

void check_calendar()
{
    static_assert(sizeof(tm_ex) == 32, "Preserve the existing calendar record size");
    constexpr std::array years{
        std::pair{1896, true}, std::pair{1900, false}, std::pair{2000, true},
        std::pair{2004, true}, std::pair{2100, false}, std::pair{2400, true},
        std::pair{0, true}, std::pair{-100, false}, std::pair{-400, true},
        std::pair{32767, false}, std::pair{32768, true}, std::pair{100000, true},
        std::pair{(std::numeric_limits<int>::min)(), true},
        std::pair{(std::numeric_limits<int>::max)(), false}};
    for (const auto [year, expected] : years)
        check(is_leap_year(year) == expected, "Gregorian leap-year rule covers the full legacy int domain");
    constexpr std::array days{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int month = 0; month < 12; ++month)
    {
        check(get_month_day_count(2023, month) == days[month], "zero-based month length in a common year");
        check(get_month_day_count(2024, month) == (month == 1 ? 29 : days[month]), "zero-based month length in a leap year");
    }
    check(get_month_day_count(100000, 1) == 29, "calendar helper retains years beyond chrono::year range");
    check(get_month_day_count(2024, -1) == 0 && get_month_day_count(2024, 12) == 0,
          "invalid months return zero without indexing outside the table");

    TimezoneScope timezone;
    constexpr std::array<UINT32, 9> timestamps{0, 1, 86399, 86400, 946684800, 951827696,
                                            2147483647U, 2147483648U, 4294967295U};
    for (const char* zone : {"UTC0", "JST-9", "PST8PDT"})
    {
        timezone.set(zone);
        for (const auto timestamp : timestamps)
            check(get_time_str(timestamp) == reference_time(timestamp), "formatted local time preserves the CRT timezone contract");
    }
    timezone.set("UTC0");
    check(get_time_zone() == 0, "UTC offset remains zero");
    timezone.set("JST-9");
    check(get_time_zone() == 9 * 3600, "positive standard timezone offset is preserved");
    timezone.set("PST8PDT");
    check(get_time_zone() == -8 * 3600, "legacy standard offset retains its daylight-saving convention");

    // Windows' system-local calendar remains independent of the process CRT timezone.
    localtime_ex();
    SYSTEMTIME before{}, after{};
    GetLocalTime(&before);
    const auto local = *localtime_ex();
    GetLocalTime(&after);
    check(local.tm_msec >= 0 && local.tm_msec < 1000 && local.tm_sec >= 0 && local.tm_sec < 60,
          "local calendar fields keep their documented ranges");
    if (before.wYear == after.wYear && before.wMonth == after.wMonth && before.wDay == after.wDay &&
        before.wHour == after.wHour && before.wMinute == after.wMinute)
        check(local.tm_year == before.wYear && local.tm_mon == before.wMonth && local.tm_mday == before.wDay &&
              local.tm_hour == before.wHour && local.tm_min == before.wMinute && local.tm_wday == before.wDayOfWeek,
              "system-local calendar matches Windows despite a different CRT timezone");

    timezone.set("UTC0");
    const char* mainText = get_time_str(0);
    const tm_ex* mainCalendar = localtime_ex();
    std::array<const char*, 2> texts{};
    std::array<const tm_ex*, 2> calendars{};
    std::barrier rendezvous{3};
    auto worker = [&](int index)
    {
        texts[index] = get_time_str(static_cast<UINT32>((index + 1) * 86400));
        calendars[index] = localtime_ex();
        rendezvous.arrive_and_wait();
        rendezvous.arrive_and_wait();
    };
    std::jthread first(worker, 0);
    std::jthread second(worker, 1);
    rendezvous.arrive_and_wait();
    check(texts[0] != texts[1] && texts[0] != mainText && texts[1] != mainText,
          "formatted time has separate live storage per thread");
    check(calendars[0] != calendars[1] && calendars[0] != mainCalendar && calendars[1] != mainCalendar,
          "local calendar has separate live storage per thread");
    check(std::string(texts[0]) == "1970-01-02 00:00:00" && std::string(texts[1]) == "1970-01-03 00:00:00" &&
          std::string(mainText) == "1970-01-01 00:00:00", "other threads preserve the caller's formatted value");
    rendezvous.arrive_and_wait();

    const auto tickBefore = GetTickCount64();
    const auto tick = GetTickCountEx();
    const auto tickAfter = GetTickCount64();
    check(tick >= tickBefore && tick <= tickAfter, "Windows uptime clock keeps its existing epoch and units");
}
}

int main()
{
    using Frames = CSimpleTimer::FrameDuration;
    using namespace std::chrono;
    static_assert(Frames{GAME_FPS} == seconds{1});
    static_assert(sizeof(CSimpleTimer) == 20, "Preserve the existing timer object size");

    CSimpleTimer timer;
    check(timer.TrigOver(), "default timer has no remaining triggers");
    check(timer.GetRemainTime() == 0 && !timer.Trig(100), "default timer stays disabled");

    timer.Enable(16, 3, 100, 8);
    check(timer.GetRemainTime() == 48, "remaining time retains the legacy interval-count contract");
    check(!timer.Trig(107), "delay is measured in frames");
    check(timer.Trig(108), "first trigger fires at its deadline");
    check(timer.GetRemainingFrames() == Frames{32}, "typed remaining duration tracks triggers");
    check(!timer.Trig(108) && !timer.Trig(123), "positive interval prevents early repeat triggers");
    check(timer.Trig(124) && timer.Trig(140), "subsequent frame deadlines fire");
    check(timer.TrigOver() && timer.GetRemainTime() == 0, "finite triggers are exhausted");
    check(!timer.Trig(1000), "exhausted timer never fires again");

    timer.Enable(Frames{16}, 2, Frames{100}, Frames{8});
    check(timer.Trig(Frames{200}), "late timer fires once per call");
    check(!timer.Trig(Frames{215}) && timer.Trig(Frames{216}), "late trigger schedules from actual trigger time");

    timer.Enable(0, 3, 0, 0);
    check(timer.Trig(0) && timer.Trig(0) && timer.Trig(0), "zero interval permits repeated same-frame triggers");
    check(!timer.Trig(0), "zero-interval timer still respects its trigger count");

    timer.Enable(1, 0, 100, 0);
    check(timer.TrigOver() && !timer.Trig(100), "zero trigger count stays exhausted");
    timer.Enable(1, 3, 100, 0);
    timer.Disable();
    check(!timer.Trig(100) && timer.GetRemainTime() == 0, "disabled timer has no active duration");
    check(!timer.TrigOver(), "disable preserves the remaining trigger count");

    const auto maximum = (std::numeric_limits<UINT32>::max)();
    timer.Enable(10, 2, maximum - 100, 5);
    check(!timer.Trig(maximum - 96) && timer.Trig(maximum - 95), "high unsigned frame counts do not become negative");
    check(!timer.Trig(maximum - 86) && timer.Trig(maximum - 85), "high frame deadlines keep their interval");
    timer.Enable(1, maximum, 0, 0);
    check(!timer.TrigOver() && timer.GetRemainTime() == maximum, "unbounded-style trigger count retains all 32 bits");
    check(timer.Trig(0) && timer.GetRemainTime() == maximum - 1, "large trigger count decrements normally");
    timer.Enable(2, maximum, 0, 0);
    check(timer.GetRemainTime() == maximum - 1, "legacy unsigned remaining-time arithmetic is preserved");

    check(duration_cast<microseconds>(Frames{1}) == microseconds{62500}, "one simulation frame is 1/16 second");
    check(duration_cast<Frames>(milliseconds{125}) == Frames{2}, "standard durations convert to frame units");

    check_calendar();

    std::printf("%d timing checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
