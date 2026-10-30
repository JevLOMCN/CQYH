#include "common_log.h"
#include <array>
#include <charconv>
#include <chrono>
#include <format>
#include <fstream>
#include <string_view>

namespace
{
template<class T> std::expected<T,std::string> number(std::string_view text)
{
    if(text.empty())return std::unexpected("Empty numeric field.");
    T value{};
    const auto [end,error]=std::from_chars(text.data(),text.data()+text.size(),value);
    if(error!=std::errc{} || end!=text.data()+text.size()) return std::unexpected("Invalid numeric field.");
    return value;
}
std::expected<unsigned,std::string> hour(std::string_view text)
{
    if(text.size()!=19 || text[4]!='-' || text[7]!='-' || text[10]!=' ' || text[13]!=':' || text[16]!=':')
        return std::unexpected("Expected YYYY-MM-DD HH:MM:SS timestamp.");
    std::array<unsigned,6> fields{};
    constexpr std::array<std::size_t,6> offsets{0,5,8,11,14,17};
    for(std::size_t index=0;index<fields.size();++index)
    {
        const auto parsed=number<unsigned>(text.substr(offsets[index],index ? 2 : 4));
        if(!parsed) return std::unexpected(parsed.error());fields[index]=*parsed;
    }
    const auto date=std::chrono::year{static_cast<int>(fields[0])}/std::chrono::month{fields[1]}/std::chrono::day{fields[2]};
    if(!date.ok() || fields[3]>23 || fields[4]>59 || fields[5]>59) return std::unexpected("Invalid calendar date or time.");
    return fields[3];
}
std::expected<void,std::string> append(std::string_view line,std::size_t index,poslog::LogData& data)
{
    if(line.empty()) return {};
    std::array<std::string_view,8> fields{};
    for(auto& field:fields)
    {
        const auto end=line.find('\t');field=line.substr(0,end);
        if(&field!=&fields.back() && end==line.npos) return std::unexpected(std::format("Line {}: expected at least eight tab-separated fields.",index));
        line=end==line.npos ? std::string_view{} : line.substr(end+1);
    }
    const auto timestamp=hour(fields[0]);const auto scene=number<std::uint32_t>(fields[4]);
    const auto x=number<std::int64_t>(fields[6]),y=number<std::int64_t>(fields[7]);
    if(!timestamp || !scene || !x || !y) return std::unexpected(std::format("Line {}: {}",index,!timestamp ? timestamp.error() : !scene ? scene.error() : !x ? x.error() : y.error()));
    ++data.rows;
    if(const auto point=poslog::project(*x,*y)) data.groups[{*timestamp,*scene}].push_back(*point);
    else ++data.outside_scene;
    return {};
}
}

std::optional<poslog::Position> poslog::project(std::int64_t x,std::int64_t y)
{
    if(x>max_scene_width || y>max_scene_width || x< -max_scene_width || y< -max_scene_width) return {};
    const auto shifted=x/points_per_cm-max_scene_width/points_per_cm/2;
    const auto vertical=y/points_per_cm;
    return Position{static_cast<std::int32_t>((shifted*70710+vertical*70710)/100000),
                    static_cast<std::int32_t>((-shifted*70710+vertical*70710)/100000)};
}
std::optional<poslog::Pixel> poslog::pixel(Position point,std::uint32_t width,std::uint32_t height)
{
    if(!width || !height || point.x<0 || point.y<0 || point.x>=projection_extent || point.y>=projection_extent) return {};
    return Pixel{static_cast<std::uint32_t>(static_cast<std::int64_t>(point.y)*width/projection_extent),
                 static_cast<std::uint32_t>(static_cast<std::int64_t>(point.x)*height/projection_extent)};
}
std::expected<poslog::LogData,std::string> poslog::parse_log(std::istream& input)
{
    try
    {
        constexpr std::size_t max_line=64*1024;
        LogData data;std::string line;line.reserve(256);std::size_t index=0;bool after_cr=false;
        const auto flush=[&]() -> std::expected<void,std::string>
        {
            ++index;
            if(line.find('\0')!=line.npos) return std::unexpected(std::format("Line {} contains a NUL character.",index));
            auto result=index==1 ? std::expected<void,std::string>{} : append(line,index,data);
            line.clear();return result;
        };
        char byte;
        while(input.get(byte))
        {
            if(byte=='\n' && after_cr){after_cr=false;continue;}
            after_cr=byte=='\r';
            if(byte=='\r' || byte=='\n') {const auto result=flush();if(!result)return std::unexpected(result.error());}
            else {if(line.size()==max_line)return std::unexpected(std::format("Line {} exceeds the supported row length.",index+1));line.push_back(byte);}
        }
        if(input.bad() || (!input.eof() && input.fail())) return std::unexpected("Cannot read the complete position log.");
        if(!line.empty()){const auto result=flush();if(!result)return std::unexpected(result.error());}
        if(index==0) return std::unexpected("Position log has no header.");
        return data;
    }
    catch(const std::bad_alloc&) {return std::unexpected("Not enough memory to load the position log.");}
    catch(const std::ios_base::failure&) {return std::unexpected("Cannot read the complete position log.");}
}
std::expected<poslog::LogData,std::string> poslog::load_log(const std::filesystem::path& filename)
{
    if(filename.empty() || filename.native().find(std::filesystem::path::value_type{})!=filename.native().npos)
        return std::unexpected("Select a filename without embedded NUL characters.");
    std::ifstream stream(filename,std::ios::binary);
    if(!stream) return std::unexpected("Cannot open the position log.");
    return parse_log(stream);
}
