#include "gm_log.h"
#include "gm_log_data.h"
#include "log.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
std::size_t checks{};
bool accepted=true;
std::vector<std::string> messages;
std::function<void()> output_callback;
void check(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
std::string read(const std::filesystem::path& path)
{
    std::ifstream input(path,std::ios::binary);return {std::istreambuf_iterator<char>(input),{}};
}
}
BOOL gm::output_tool_log(std::string_view text)
{
    messages.emplace_back(text);if(output_callback)output_callback();return accepted;
}
int main(int argc,char** argv)
{
    CLog::GetInstance().SetLevelMask(0);
    try
    {
        check(argc==2,"output directory argument");
        gm::LogQueryFields fields{};
        check(gm::serialize_log_query(0,fields).value().empty() && gm::serialize_log_query(2,fields).value()=="0 0 0","empty legacy filters remain zero/default");
        std::string account(64,'a');fields[0]=account;
        const auto query=gm::serialize_log_query(0,fields);check(query && *query==account,"maximum account bytes copied without truncation");
        account.push_back('b');fields[0]=account;check(!gm::serialize_log_query(0,fields),"oversized account rejected instead of silently truncated");
        fields[0]=std::string_view("a\0b",3);check(!gm::serialize_log_query(0,fields),"embedded account terminator rejected");
        fields[1]=" +4294967295 \t";check(gm::serialize_log_query(1,fields).value()=="4294967295","full unsigned role ID with legacy whitespace/plus input");
        fields[2]="4294967295";fields[3]="18446744073709551615";
        check(gm::serialize_log_query(2,fields).value()=="4294967295 4294967295 18446744073709551615","exact full-width item-query fields");
        fields[3]="18446744073709551616";check(!gm::serialize_log_query(2,fields),"overflow GUID rejected");fields[3]="1x";
        check(!gm::serialize_log_query(2,fields),"partial numeric field rejected");fields[3]="1";
        fields[4]="4294967295";fields[5]="-2147483648";
        check(gm::serialize_log_query(3,fields).value()=="4294967295 4294967295 -2147483648","signed money filter and unsigned target preserved");
        fields[6]="2147483647";check(gm::serialize_log_query(4,fields).value()=="4294967295 2147483647","maximum quest filter");
        fields[6]="-2147483648";check(gm::serialize_log_query(4,fields).value()=="4294967295 -2147483648","negative legacy quest filter preserved");
        fields[6]="2147483648";check(!gm::serialize_log_query(4,fields),"signed query filter overflow rejected");
        for(const auto* invalid:{"-1","4294967296","garbage","+"})
        {fields[1]=invalid;check(!gm::serialize_log_query(1,fields),"invalid unsigned role filter rejected");}
        check(!gm::serialize_log_query(-1,fields) && !gm::serialize_log_query(5,fields) && !gm::serialize_log_query(INT32_MAX,fields),"unsupported query types rejected");
        fields[1]="1";fields[5]="+-1";check(!gm::serialize_log_query(3,fields),"multiple signs cannot become a valid signed filter");
        auto& logger=CGMLog::GetInstance();check(logger.Init(),"logger init");
        const std::string long_text=std::string(8192,'x')+" 100% %n {}";
        check(logger.Log("{} {}",long_text,UINT64_MAX) && messages.back()==long_text+" 18446744073709551615","typed formatting has no legacy 4 KB truncation");
        check(logger.Log("100% %n {}") && messages.back()=="100% %n {}","plain messages never become printf or format strings");
        const auto count=messages.size();check(!logger.Log(static_cast<const char*>(nullptr)) && !logger.Log(std::string_view("a\0b",3)) && messages.size()==count,"invalid plain messages rejected before output");
        accepted=false;check(!logger.Log("rejected"),"UI output failure propagates");accepted=true;
        std::string input="owned before callback";output_callback=[&]{input.assign(8192,'y');};
        check(logger.Log(input) && messages.back()=="owned before callback","message snapshot outlives caller mutation");output_callback={};
        std::tm local{};local.tm_year=126;local.tm_mon=0;local.tm_mday=2;local.tm_hour=3;local.tm_min=4;local.tm_sec=5;local.tm_isdst=-1;
        const auto when=std::chrono::system_clock::from_time_t(std::mktime(&local));
        const auto line=gm::timed_log(long_text,when);check(line && *line=="01-02 03:04:05 "+long_text,"local timestamp bytes retain complete message");
        const char complete[]{'a','b','c',0};const auto body=gm::game_log_body(complete);
        check(body && *body=="abc" && gm::game_log_body(std::span(complete).last(1))->empty(),"counted body excludes final terminator and accepts empty text");
        check(!gm::game_log_body({}) && !gm::game_log_body(std::span(complete).first(3)) && !gm::game_log_body(std::span("a\0b",4)),"missing/embedded terminators rejected");
        std::vector<char> largest(65535,'x');largest.back()=0;check(gm::game_log_body(largest)->size()==65534,"maximum counted body");largest.push_back(0);check(!gm::game_log_body(largest),"oversized counted body rejected");
        const auto root=std::filesystem::path(argv[1]);std::filesystem::create_directories(root);
        const auto native=root/std::filesystem::path(u8"\u65E5\u5FD7");std::filesystem::create_directories(native);const auto output=native/"output.txt";
        check(gm::clear_game_log(output).has_value() && read(output).empty(),"staged empty output at native Unicode path");
        const std::string bytes="first\nsecond\r\n100% <b>text</b>";
        check(gm::append_game_log(output,bytes).has_value(),"standard append to Unicode output");
        const auto legacy=root/"legacy.txt";
        auto* file=std::fopen(legacy.string().c_str(),"w");check(file!=nullptr,"legacy text-mode oracle open");
        const auto written=std::fwrite(bytes.data(),1,bytes.size(),file);const auto closed=std::fclose(file);
        check(written==bytes.size() && closed==0 && read(output)==read(legacy),"append retains legacy Windows newline translation and bytes");
        check(gm::append_game_log(output," tail").has_value() && read(output)==read(legacy)+" tail","subsequent append preserves prior bytes");
        check(!gm::append_game_log(native/"missing"/"log",bytes) && !gm::append_game_log(native,bytes),"open/publication errors reported");
        const auto before=read(output);check(!gm::append_game_log(output,std::string(65536,'x')) && read(output)==before,"oversized append leaves prior file unchanged");
        check(gm::clear_game_log(output).has_value(),"clear existing log atomically");
        std::vector<std::jthread> writers;
        for(int thread=0;thread<4;++thread) writers.emplace_back([&,thread]
        {
            for(int index=0;index<8;++index)
                if(!gm::append_game_log(output,std::format("{}:{}\n",thread,index)))std::abort();
        });
        writers.clear();std::ifstream input_file(output);std::multiset<std::string> lines;
        for(std::string row;std::getline(input_file,row);)lines.insert(row);
        std::multiset<std::string> expected;for(int thread=0;thread<4;++thread)for(int index=0;index<8;++index)expected.insert(std::format("{}:{}",thread,index));
        check(lines==expected,"concurrent append chunks remain whole and complete");
        check(logger.UnInit(),"logger teardown");
        std::cout<<checks<<" GM logging checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
