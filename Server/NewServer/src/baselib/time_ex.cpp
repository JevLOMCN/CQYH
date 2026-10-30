#include "stdafx.h"
#include "time_ex.h"

// This translation unit uses standard formatting instead of the compatibility alias.
#ifdef snprintf
#undef snprintf
#endif

#include <array>
#include <charconv>
#include <chrono>
#include <ctime>
#include <format>
#include <optional>
#include <stdexcept>

namespace
{
std::optional<std::tm> LocalTime(std::time_t value)
{
    std::tm result{};
#if defined(_WIN32)
    if (::localtime_s(&result, &value) != 0)
        return std::nullopt;
#else
    if (::localtime_r(&value, &result) == nullptr)
        return std::nullopt;
#endif
    return result;
}

std::optional<std::tm> UtcTime(std::time_t value)
{
    std::tm result{};
#if defined(_WIN32)
    if (::gmtime_s(&result, &value) != 0)
        return std::nullopt;
#else
    if (::gmtime_r(&value, &result) == nullptr)
        return std::nullopt;
#endif
    return result;
}

constexpr bool SupportedYear(int value)
{
    return value >= static_cast<int>((std::chrono::year::min)()) &&
           value <= static_cast<int>((std::chrono::year::max)());
}
}

const tm_ex* localtime_ex(void)
{
    thread_local tm_ex result{};
#if defined(_WIN32) && __cpp_lib_chrono >= 201907L
    try
    {
        using namespace std::chrono;
        const auto now = floor<milliseconds>(system_clock::now());
        const auto local = current_zone()->to_local(now);
        const auto day = floor<days>(local);
        const year_month_day date{day};
        const hh_mm_ss time{local - day};
        result = {static_cast<INT32>(time.subseconds().count()),
                  static_cast<INT32>(time.seconds().count()),
                  static_cast<INT32>(time.minutes().count()),
                  static_cast<INT32>(time.hours().count()),
                  static_cast<INT32>(static_cast<unsigned>(date.day())),
                  static_cast<INT32>(static_cast<unsigned>(date.month())),
                  static_cast<int>(date.year()),
                  static_cast<INT32>(weekday{day}.c_encoding())};
        return &result;
    }
    catch (const std::runtime_error&)
    {
        // Custom Windows zones or unavailable timezone data retain the native fallback.
    }
#endif
#if defined(_WIN32)
    SYSTEMTIME time{};
    GetLocalTime(&time);
    result = {time.wMilliseconds, time.wSecond, time.wMinute, time.wHour,
              time.wDay, time.wMonth, time.wYear, time.wDayOfWeek};
#else
    const auto now = std::chrono::system_clock::now();
    if (const auto local = LocalTime(std::chrono::system_clock::to_time_t(now)))
    {
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        result = {static_cast<INT32>(milliseconds.count() % 1000),
                  local->tm_sec, local->tm_min, local->tm_hour, local->tm_mday,
                  local->tm_mon + 1, local->tm_year + 1900, local->tm_wday};
    }
#endif
    return &result;
}

bool is_leap_year(int nYear)
{
    if (SupportedYear(nYear))
        return std::chrono::year{nYear}.is_leap();
    // chrono::year has a bounded representation; retain the legacy full-int domain.
    return nYear % 4 == 0 && (nYear % 100 != 0 || nYear % 400 == 0);
}

int get_month_day_count(int nYear, int nMonth)
{
    if (nMonth < 0 || nMonth >= 12)
        return 0;
    if (SupportedYear(nYear))
    {
        const auto date = std::chrono::year{nYear} /
                          std::chrono::month{static_cast<unsigned>(nMonth) + 1} / std::chrono::last;
        return static_cast<int>(static_cast<unsigned>(date.day()));
    }
    constexpr std::array monthDays{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return nMonth == 1 && is_leap_year(nYear) ? 29 : monthDays[nMonth];
}

UINT64 GetTickCountEx()
{
#if defined(_WIN32)
    return GetTickCount64();
#else
    return static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
#endif
}

const char* get_time_str(UINT32 dwTime)
{
    thread_local std::array<char, 64> result{};
    if (const auto local = LocalTime(static_cast<std::time_t>(dwTime)))
    {
        const auto formatted = std::format_to_n(result.begin(), result.size() - 1,
            "{:04}-{:02}-{:02} {:02}:{:02}:{:02}", local->tm_year + 1900,
            local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
        *formatted.out = '\0';
    }
    else
    {
        const auto formatted = std::to_chars(result.data(), result.data() + result.size() - 1, dwTime);
        if (formatted.ec == std::errc{})
            *formatted.ptr = '\0';
        else
            result.front() = '\0';
    }
    return result.data();
}

int get_time_zone(void)
{
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto utc = UtcTime(now);
    auto local = LocalTime(now);
    if (!utc || !local)
        return 0;
    const auto utcEpoch = std::mktime(&*utc);
    const auto localEpoch = std::mktime(&*local);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::from_time_t(localEpoch) -
        std::chrono::system_clock::from_time_t(utcEpoch)).count());
}
