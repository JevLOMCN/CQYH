#include "gm_log_data.h"
#include "gm_reports.h"
#include "gm_def.h"
#include <charconv>
#include <ctime>
#include <format>
#include <fstream>
#include <mutex>

namespace { std::mutex file_mutex; }
namespace
{
template<class T> std::expected<T,std::string> number(std::string_view field,std::string_view name)
{
    constexpr std::string_view space=" \t\r\n\f\v";
    const auto start=field.find_first_not_of(space);
    if (start==std::string_view::npos) return T{0}; // Empty legacy numeric filters mean zero.
    field=field.substr(start,field.find_last_not_of(space)-start+1);
    if (field.front()=='+')
    {
        field.remove_prefix(1);
        if (field.empty() || field.front()=='+' || field.front()=='-') return std::unexpected(std::format("Invalid {}",name));
    }
    T value{};
    const auto [end,error]=std::from_chars(field.data(),field.data()+field.size(),value);
    if (error!=std::errc{} || end!=field.data()+field.size()) return std::unexpected(std::format("Invalid {}",name));
    return value;
}
}
std::expected<std::string,std::string> gm::serialize_log_query(INT32 type,const LogQueryFields& fields)
{
    if (type==GM_LOG_QUERY_ROLE_LOGIN)
    {
        if (fields[0].size()>64 || fields[0].contains('\0')) return std::unexpected("Account must contain at most 64 UTF-8 bytes without NUL");
        return std::string(fields[0]);
    }
    if (type<GM_LOG_QUERY_ROLE_LOGOUT || type>GM_LOG_QUERY_QUEST) return std::unexpected("Unsupported log query type");
    const auto role=number<std::uint32_t>(fields[1],"role ID");if (!role) return std::unexpected(role.error());
    if (type==GM_LOG_QUERY_ROLE_LOGOUT) return std::format("{}",*role);
    if (type==GM_LOG_QUERY_ITEM)
    {
        const auto item=number<std::uint32_t>(fields[2],"item ID");if (!item) return std::unexpected(item.error());
        const auto guid=number<std::uint64_t>(fields[3],"item GUID");if (!guid) return std::unexpected(guid.error());
        return std::format("{} {} {}",*role,*item,*guid);
    }
    if (type==GM_LOG_QUERY_MONEY)
    {
        const auto target=number<std::uint32_t>(fields[4],"target role ID");if (!target) return std::unexpected(target.error());
        const auto money=number<std::int32_t>(fields[5],"money type");if (!money) return std::unexpected(money.error());
        return std::format("{} {} {}",*role,*target,*money);
    }
    const auto quest=number<std::int32_t>(fields[6],"quest ID");if (!quest) return std::unexpected(quest.error());
    return std::format("{} {}",*role,*quest);
}
std::expected<std::string,std::string> gm::timed_log(std::string_view text,std::chrono::system_clock::time_point when)
{
    const auto seconds=std::chrono::system_clock::to_time_t(when);
    std::tm local{};
#ifdef _WIN32
    if (localtime_s(&local,&seconds)!=0) return std::unexpected("log time is outside the local calendar range");
#else
    if (!localtime_r(&seconds,&local)) return std::unexpected("log time is outside the local calendar range");
#endif
    return std::format("{:02}-{:02} {:02}:{:02}:{:02} {}",local.tm_mon+1,local.tm_mday,local.tm_hour,local.tm_min,local.tm_sec,text);
}
std::expected<std::string_view,std::string> gm::game_log_body(std::span<const char> bytes)
{
    if (bytes.empty() || bytes.size()>0xFFFF || bytes.back()!=0) return std::unexpected("invalid game log length or terminator");
    const std::string_view text(bytes.data(),bytes.size()-1);
    if (text.contains('\0')) return std::unexpected("embedded game log terminator");
    return text;
}
std::expected<void,std::string> gm::clear_game_log(const std::filesystem::path& path)
{
    const std::lock_guard lock(file_mutex);
    return write_text_report(path,{});
}
std::expected<void,std::string> gm::append_game_log(const std::filesystem::path& path,std::string_view text)
{
    if (text.size()>0xFFFF) return std::unexpected("game log chunk exceeds the wire limit");
    const std::lock_guard lock(file_mutex);
    // Keep the Windows CRT text-mode LF translation used by legacy fwrite.
    std::ofstream output(path,std::ios::out|std::ios::app);
    if (!output) return std::unexpected("cannot open game log output");
    if (!text.empty()) output.write(text.data(),static_cast<std::streamsize>(text.size()));
    output.flush();const bool written=static_cast<bool>(output);output.close();
    if (!written || !output) return std::unexpected("cannot write complete game log output");
    return {};
}
