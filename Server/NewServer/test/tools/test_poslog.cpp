#include "common_log.h"
#include "bmp_file.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
namespace fs=std::filesystem;
unsigned checks;
void check(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
std::expected<poslog::LogData,std::string> parse(const std::string& text){std::istringstream stream(text);return poslog::parse_log(stream);}
std::string row(std::string time="2024-02-29 05:06:07",std::string scene="257",std::string x="134217728",std::string y="111219712")
{return time+"\tarea\tgroup\trole\t"+scene+"\tscene\t"+x+"\t"+y+"\tz";}
std::vector<char> read(const fs::path& path){std::ifstream stream(path,std::ios::binary);check(stream.good(),"fixture output exists");return {std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>()};}
void write(const fs::path& path,std::string_view text){std::ofstream stream(path,std::ios::binary);stream.write(text.data(),static_cast<std::streamsize>(text.size()));if(!stream)throw std::runtime_error("Cannot write log fixture");}
void parser()
{
    for(const std::string newline:{"\n","\r\n","\r"})
    {
        const auto data=parse("header"+newline+row()+newline+row("2024-03-01 05:59:59","4294967295")+newline+row("2024-03-02 05:00:00"));
        check(data && data->rows==3 && data->groups.size()==2,"CR/LF/CRLF and final unterminated record all load");
        check(data->groups.at({5,257})==std::vector<poslog::Position>({{76800,76800},{76800,76800}}),"hourly grouping across days and legacy projection retained");
        check(data->groups.contains({5,UINT32_MAX}),"full unsigned scene identity is retained");
    }
    check(!parse(""),"empty file without header is rejected");
    const auto empty=parse("header\n\n");check(empty && empty->rows==0 && empty->groups.empty(),"header and blank lines do not create phantom positions");
    check(!parse("header\nshort\trow"),"missing required columns rejected");
    for(const auto time:{"2023-02-29 05:00:00","2024-13-01 05:00:00","2024-02-30 05:00:00","2024-02-29 24:00:00","2024-02-29 05:60:00","2024-02-29 05:00:60","2024-02-29 5:00:00","not a date"})
        check(!parse("header\n"+row(time)),"invalid calendar/time fields rejected with no partial model");
    for(const auto scene:{"-1","4294967296","1x",""})check(!parse("header\n"+row("2024-02-29 05:00:00",scene)),"scene field checked without signed truncation");
    for(const auto x:{"9223372036854775808","-9223372036854775809","12x",""})check(!parse("header\n"+row("2024-02-29 05:00:00","1",x)),"64-bit coordinate syntax and overflow rejected");
    const auto outside=parse("header\n"+row("2024-02-29 05:00:00","1","9223372036854775807")+"\n"+row("2024-02-29 05:00:00","1","-9223372036854775808"));
    check(outside && outside->rows==2 && outside->outside_scene==2 && outside->groups.empty(),"extreme but representable coordinates skipped before arithmetic");
    check(!parse(std::string("header\0",7)+"\n"+row()),"NUL in header rejected");
    check(!parse("header\n"+row()+std::string("\0",1)),"NUL in unused fields rejected");
    check(!parse("header\n"+std::string(65537,'x')),"oversized row is bounded");
    auto large=row();large.insert(large.find('\t')+1,std::string(8000,'x'));
    check(parse("header\n"+large).has_value(),"standard strings handle fields beyond old fixed buffers");
    std::istringstream failed("header\n"+row());failed.setstate(std::ios::badbit);check(!poslog::parse_log(failed),"stream read failure is reported");
}
void projection()
{
    check(poslog::project(134217728,0)==poslog::Position{0,0},"projection origin retains integer truncation");
    check(poslog::project(134217728,111219712)==poslog::Position{76800,76800},"projection center matches legacy integer formula");
    check(!poslog::project(INT64_MIN,0) && !poslog::project(0,INT64_MAX),"projection bounds checked before multiplication");
    check(poslog::pixel({0,0},512,512)==poslog::Pixel{0,0},"origin maps to first bitmap pixel");
    check(poslog::pixel({76800,76800},512,1024)==poslog::Pixel{256,512},"column/row scale uses the correct dimension");
    check(poslog::pixel({153599,153599},512,512)==poslog::Pixel{511,511},"last in-frame position maps to final pixel");
    for(const auto point:std::array<poslog::Position,5>{{{-1,0},{0,-1},{153600,0},{0,153600},{INT32_MAX,INT32_MIN}}})
        check(!poslog::pixel(point,512,512),"negative and upper-edge pixels are clipped without index wrapping");
    check(!poslog::pixel({0,0},0,512) && !poslog::pixel({0,0},512,0),"empty canvas dimensions rejected");
}
}
int main(int argc,char** argv)
{
    try
    {
        parser();projection();check(argc==2,"expected private fixture directory");
        const auto directory=fs::absolute(argv[1]);fs::create_directories(directory);
        const auto unicode=directory/fs::path(u8"\u8DEF\u5F84");fs::create_directories(unicode);
        const auto log=unicode/"position.log";write(log,"header\n"+row());
        check(poslog::load_log(log)->groups.at({5,257}).front()==poslog::Position{76800,76800},"native Unicode log path loads checked model");
        check(!poslog::load_log(directory/"missing.log"),"missing log reports error");
        const std::array<poslog::Position,6> points{{{0,0},{76800,76800},{153599,153599},{-1,0},{153600,0},{INT32_MAX,INT32_MIN}}};
        const auto bitmap=poslog::make_bitmap(points,512);check(bitmap && bitmap->size()==786486,"512 bitmap has exact legacy byte size");
        const std::array<unsigned char,54> header{0x42,0x4d,0x36,0,0x0c,0,0,0,0,0,0x36,0,0,0,0x28,0,0,0,0,2,0,0,0,2,0,0,1,0,24,0,0,0,0,0,0,0,0x0c,0};
        check(std::equal(header.begin(),header.end(),bitmap->begin(),[](unsigned char a,char b){return a==static_cast<unsigned char>(b);}),"complete BMP header matches little-endian 24-bit legacy golden bytes");
        check(static_cast<unsigned char>((*bitmap)[56])==255 && static_cast<unsigned char>((*bitmap)[54+(256*512+256)*3+2])==255 && static_cast<unsigned char>(bitmap->back())==255,"BGR pixels occupy origin, center and final row in bottom-up order");
        check(std::count(bitmap->begin()+54,bitmap->end(),static_cast<char>(255))==3,"out-of-frame points cannot write stray pixels");
        const auto large=poslog::make_bitmap(points,1024);check(large && large->size()==3145782,"1024 bitmap size checked without integer overflow");
        check(!poslog::make_bitmap(points,0) && !poslog::make_bitmap(points,UINT32_MAX),"unsupported bitmap sizes rejected before allocation");
        const auto output=unicode/"map.bmp";write(output,"prior");check(poslog::write_bitmap(output,points,512).has_value() && read(output)==*bitmap,"native bitmap output publishes complete exact bytes");
        check(!poslog::write_bitmap(output,points,3) && read(output)==*bitmap,"invalid bitmap request preserves previous output");
        const auto blocked=directory/"blocked.bmp";fs::create_directories(blocked);check(!poslog::write_bitmap(blocked,points,512) && fs::is_directory(blocked),"publication failure preserves destination directory");
#ifdef _WIN32
        {
            const auto handle=CreateFileW(output.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);check(handle!=INVALID_HANDLE_VALUE,"locked bitmap fixture opens");
            struct Close{HANDLE handle;~Close(){CloseHandle(handle);}} close{handle};check(!poslog::write_bitmap(output,points,1024),"sharing failure reports unsuccessful publication");
        }
        check(read(output)==*bitmap,"sharing failure cannot truncate previous bitmap");
#endif
        auto nul=output.native();nul.push_back(fs::path::value_type{});nul+=fs::path("ignored").native();
        check(!poslog::load_log(fs::path(nul)) && !poslog::write_bitmap(fs::path(nul),points,512),"embedded-NUL paths rejected without truncation");
        for(const auto& file:fs::recursive_directory_iterator(directory))check(!file.path().filename().native().starts_with(fs::path(".poslog-bmp-").native()),"staging files are cleaned after success and failure");
        std::cout<<checks<<" PosLog model/bitmap checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
