#pragma once
#include <cstdint>
#include <array>
#include <chrono>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>

namespace gm
{
struct ExchangeStatistics
{
    std::int64_t system_money{}, player_money{}, system_traded{}, player_traded{}, system_gems{}, player_gems{};
    std::int32_t average{}, previous_average{};
};
std::string exchange_price(std::int64_t cents);
std::optional<std::int32_t> exchange_price_units(double price);
struct ExchangeWindow { std::uint32_t begin{},end{}; };
std::optional<ExchangeWindow> exchange_window(std::int64_t begin,std::int64_t end);
std::optional<ExchangeWindow> exchange_day(std::int64_t timestamp,std::int32_t offset=0,bool previous=false);
std::optional<ExchangeWindow> exchange_hour(std::int64_t timestamp);
struct ExchangeDue { bool log{},history{},statistics{}; };
class ExchangePoll
{
    using Clock=std::chrono::steady_clock;
    std::optional<Clock::time_point> m_previous;
    std::array<Clock::time_point,3> m_next{};
public:
    ExchangeDue take(Clock::time_point now);
};
std::string exchange_summary(const ExchangeStatistics& value);
std::string exchange_comparison(const ExchangeStatistics& today, const ExchangeStatistics& yesterday);
struct ExchangeExport { std::filesystem::path filename; std::string bytes; };
std::expected<ExchangeExport,std::string> exchange_export(std::uint32_t begin, std::uint32_t end,
    const ExchangeStatistics& value);
}
