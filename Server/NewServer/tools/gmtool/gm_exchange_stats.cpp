#include "gm_exchange_stats.h"
#include <ctime>
#include <cmath>
#include <limits>
#include <format>
#include <optional>
#include <utility>

namespace
{
std::string units(std::int64_t value)
{
    // Preserve the legacy report's whole-ten-thousand unit conversion.
    return std::format("{}.00", value / 10000);
}
std::optional<std::tm> local_time(std::uint32_t timestamp)
{
    if (!std::in_range<std::time_t>(timestamp)) return {};
    const auto seconds = static_cast<std::time_t>(timestamp);
    std::tm value{};
#ifdef _WIN32
    if (localtime_s(&value,&seconds) != 0) return {};
#else
    if (!localtime_r(&seconds,&value)) return {};
#endif
    return value;
}
}
std::string gm::exchange_price(std::int64_t value)
{
    const auto magnitude = value < 0 ? std::uint64_t{0} - static_cast<std::uint64_t>(value) : static_cast<std::uint64_t>(value);
    return std::format("{}{}.{:02}",value < 0 ? "-" : "",magnitude/100,magnitude%100);
}
std::optional<std::int32_t> gm::exchange_price_units(double price)
{
    if (!std::isfinite(price) || price<0) return {};
    const auto cents=std::round(price*100.0);
    if (!std::isfinite(cents) || cents>std::numeric_limits<std::int32_t>::max()) return {};
    return static_cast<std::int32_t>(cents);
}
std::optional<gm::ExchangeWindow> gm::exchange_window(std::int64_t begin,std::int64_t end)
{
    if (!std::in_range<std::uint32_t>(begin) || !std::in_range<std::uint32_t>(end) || end<begin) return {};
    return ExchangeWindow{static_cast<std::uint32_t>(begin),static_cast<std::uint32_t>(end)};
}
std::optional<gm::ExchangeWindow> gm::exchange_day(std::int64_t timestamp,std::int32_t offset,bool previous)
{
    if (!std::in_range<std::uint32_t>(timestamp)) return {};
    using namespace std::chrono;
    // Retain the existing server query's shifted calendar-day convention.
    const auto point=sys_seconds(seconds(timestamp+offset));
    const auto start=floor<days>(point)-(previous ? days(1) : days(0));
    return exchange_window(duration_cast<seconds>(start.time_since_epoch()).count(),
        duration_cast<seconds>((start+days(1)).time_since_epoch()).count());
}
std::optional<gm::ExchangeWindow> gm::exchange_hour(std::int64_t timestamp)
{
    if (!std::in_range<std::uint32_t>(timestamp)) return {};
    using namespace std::chrono;
    const auto start=floor<hours>(sys_seconds(seconds(timestamp)));
    return exchange_window(duration_cast<seconds>(start.time_since_epoch()).count(),
        duration_cast<seconds>((start+hours(1)).time_since_epoch()).count());
}
gm::ExchangeDue gm::ExchangePoll::take(Clock::time_point now)
{
    if (m_previous && now==*m_previous) return {};
    if (m_previous && now<*m_previous) m_previous.reset();
    constexpr std::array periods{std::chrono::seconds(1),std::chrono::seconds(3),std::chrono::seconds(5)};
    std::array<bool,3> due{};
    for (std::size_t index=0;index<periods.size();++index)
    {
        if (!m_previous || now>=m_next[index])
        {
            due[index]=true;
            m_next[index]=now>Clock::time_point::max()-periods[index] ? Clock::time_point::max() : now+periods[index];
        }
    }
    m_previous=now;
    return {due[0],due[1],due[2]};
}
std::string gm::exchange_summary(const ExchangeStatistics& s)
{
    return std::format("系统投放量\t{} 万\n系统成交量\t{} 万\n系统成交元宝\t{}\n\n玩家投放量\t{} 万\n玩家成交量\t{} 万\n玩家成交元宝\t{}\n\n当日平均价\t{}\n前1小时平均价\t{}\n\n",
        units(s.system_money),units(s.system_traded),exchange_price(s.system_gems),
        units(s.player_money),units(s.player_traded),exchange_price(s.player_gems),exchange_price(s.average),exchange_price(s.previous_average));
}
std::string gm::exchange_comparison(const ExchangeStatistics& s,const ExchangeStatistics& y)
{
    return std::format("系统投放量\t{} 万\n系统成交量\t{} 万\n系统成交元宝\t{}\n\n玩家投放量\t{} 万\n玩家成交量\t{} 万\n玩家成交元宝\t{}\n\n当日平均价\t{}\n\n前1小时平均价\t{}\n\n\n\n昨日系统投放量\t{} 万\n昨日系统成交量\t{} 万\n昨日系统成交元宝\t{}\n\n昨日玩家投放量\t{} 万\n昨日玩家成交量\t{} 万\n昨日玩家成交元宝\t{}\n\n昨日平均价\t{}\n\n",
        units(s.system_money),units(s.system_traded),exchange_price(s.system_gems),
        units(s.player_money),units(s.player_traded),exchange_price(s.player_gems),exchange_price(s.average),exchange_price(s.previous_average),
        units(y.system_money),units(y.system_traded),exchange_price(y.system_gems),units(y.player_money),units(y.player_traded),exchange_price(y.player_gems),y.average);
}
std::expected<gm::ExchangeExport,std::string> gm::exchange_export(std::uint32_t begin,std::uint32_t end,const ExchangeStatistics& s)
{
    const auto first=local_time(begin), last=local_time(end);
    if (!first || !last) return std::unexpected("exchange report time is outside the local calendar range");
    ExchangeExport result;
    result.filename=std::format("{}-{}_{}-{}.txt",first->tm_mon+1,first->tm_mday,last->tm_mon+1,last->tm_mday);
    result.bytes=std::format("开始时间\t结束时间\t系统投放量\t系统成交量\t系统成交元宝\t玩家投放量\t玩家成交量\t玩家成交元宝\t平均价\r\n{}-{}-{} {}:{}\t{}-{}-{} {}:{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\r\n",
        first->tm_year+1900,first->tm_mon+1,first->tm_mday,first->tm_hour,first->tm_min,
        last->tm_year+1900,last->tm_mon+1,last->tm_mday,last->tm_hour,last->tm_min,
        units(s.system_money),units(s.system_traded),exchange_price(s.system_gems),units(s.player_money),units(s.player_traded),exchange_price(s.player_gems),s.average);
    return result;
}
