#include "gm_exchange_stats.h"
#include "gm_reports.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
std::size_t checks{};
void check(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
std::string read(const std::filesystem::path& path)
{
    std::ifstream input(path,std::ios::binary);return {std::istreambuf_iterator<char>(input),{}};
}
std::uint32_t timestamp(int day,int hour,int minute)
{
    std::tm value{};value.tm_year=126;value.tm_mon=0;value.tm_mday=day;value.tm_hour=hour;value.tm_min=minute;value.tm_isdst=-1;
    return static_cast<std::uint32_t>(std::mktime(&value));
}
}
int main(int argc,char** argv)
{
    try
    {
        check(argc==2,"output directory argument");
        using namespace std::chrono;
        check(gm::exchange_window(0,UINT32_MAX).has_value() && gm::exchange_window(INT32_MAX,UINT32_MAX)->begin==INT32_MAX,"full unsigned wire timestamp range");
        check(!gm::exchange_window(-1,0) && !gm::exchange_window(0,INT64_MAX) &&
            !gm::exchange_window(2,1) && !gm::exchange_window(INT64_MIN,INT64_MAX),"invalid/reversed timestamp ranges rejected before narrowing");
        const auto day=gm::exchange_day(2200000000),hour=gm::exchange_hour(2200000000);
        check(day && day->begin==2200000000ULL/86400*86400 && day->end==day->begin+86400 &&
            hour && hour->begin==2200000000ULL/3600*3600 && hour->end==hour->begin+3600,"legacy UTC day/hour buckets beyond 2038");
        const auto shifted=gm::exchange_day(86400+20*3600,8*3600,true);
        check(shifted && shifted->begin==86400 && shifted->end==172800,"legacy shifted previous-day convention");
        check(!gm::exchange_day(0,0,true) && !gm::exchange_day(UINT32_MAX) && !gm::exchange_hour(UINT32_MAX) &&
            !gm::exchange_day(INT64_MAX,INT32_MAX) && !gm::exchange_hour(-1),"bucket endpoints cannot wrap the wire range");
        gm::ExchangePoll schedule;const auto tick=steady_clock::time_point{};
        auto due=schedule.take(tick);check(due.log && due.history && due.statistics,"initial poll requests every view");
        due=schedule.take(tick);check(!due.log && !due.history && !due.statistics,"same tick is not polled twice");
        due=schedule.take(tick+milliseconds(999));check(!due.log && !due.history && !due.statistics,"poll interval does not fire early");
        due=schedule.take(tick+seconds(1));check(due.log && !due.history && !due.statistics,"one-second log cadence");
        due=schedule.take(tick+seconds(3));check(due.log && due.history && !due.statistics,"three-second history cadence");
        due=schedule.take(tick+seconds(5));check(due.log && !due.history && due.statistics,"five-second statistics cadence");
        due=schedule.take(tick+hours(1));check(due.log && due.history && due.statistics,"late polling coalesces missed work");
        due=schedule.take(tick+hours(1));check(!due.log && !due.history && !due.statistics,"late tick has no catch-up burst");
        due=schedule.take(tick);check(due.log && due.history && due.statistics,"clock reset restarts deadlines");
        due=schedule.take(steady_clock::time_point::max());check(due.log && due.history && due.statistics,"deadline addition saturates safely");
        due=schedule.take(steady_clock::time_point::max());check(!due.log && !due.history && !due.statistics,"saturated deadline cannot repeat at same tick");
        check(gm::exchange_price_units(0.29)==29 && gm::exchange_price_units(1.15)==115 &&
            gm::exchange_price_units(999.99)==99999,"decimal hundredths round without binary truncation");
        check(gm::exchange_price_units(21474836.47)==INT32_MAX && gm::exchange_price_units(0.0)==0,"wire price endpoints");
        check(!gm::exchange_price_units(21474836.48) && !gm::exchange_price_units(-0.01) &&
            !gm::exchange_price_units(std::numeric_limits<double>::infinity()) &&
            !gm::exchange_price_units(std::numeric_limits<double>::quiet_NaN()) &&
            !gm::exchange_price_units(std::numeric_limits<double>::max()),"unrepresentable price values rejected before conversion");
        for (const auto [value,expected] : {std::pair<std::int64_t,const char*>{0,"0.00"},{1,"0.01"},{-1,"-0.01"},
            {12345,"123.45"},{-12345,"-123.45"},{INT64_MAX,"92233720368547758.07"},{INT64_MIN,"-92233720368547758.08"}})
            check(gm::exchange_price(value)==expected,"exact signed full-width hundredths");
        gm::ExchangeStatistics s{123456,987654,55555,77777,12345,-6789,1234,5678};
        char legacy[4096];
        std::snprintf(legacy,sizeof(legacy),"系统投放量\t%.2f 万\n系统成交量\t%.2f 万\n系统成交元宝\t%.2f\n\n玩家投放量\t%.2f 万\n玩家成交量\t%.2f 万\n玩家成交元宝\t%.2f\n\n当日平均价\t%.2f\n前1小时平均价\t%.2f\n\n",
            static_cast<double>(s.system_money/10000),static_cast<double>(s.system_traded/10000),s.system_gems/100.0,
            static_cast<double>(s.player_money/10000),static_cast<double>(s.player_traded/10000),s.player_gems/100.0,s.average/100.0,s.previous_average/100.0);
        check(gm::exchange_summary(s)==legacy,"summary matches legacy labels, units, order and spacing");
        const auto comparison=gm::exchange_comparison(s,s);
        check(comparison.find("昨日系统投放量\t12.00 万")!=std::string::npos && comparison.ends_with("昨日平均价\t1234\n\n"),"comparison retains legacy yesterday average units");
        const auto report=gm::exchange_export(timestamp(2,3,4),timestamp(3,5,6),s);
        check(report && report->filename=="1-2_1-3.txt","local date filename");
        check(report->bytes=="开始时间\t结束时间\t系统投放量\t系统成交量\t系统成交元宝\t玩家投放量\t玩家成交量\t玩家成交元宝\t平均价\r\n2026-1-2 3:4\t2026-1-3 5:6\t12.00\t5.00\t123.45\t98.00\t7.00\t-67.89\t1234\r\n","legacy export columns, encoding, conversions and CRLF bytes");
        check(gm::exchange_export(UINT32_MAX,UINT32_MAX,s).has_value(),"timestamps beyond 2038 remain representable");
        const auto root=std::filesystem::path(argv[1])/std::filesystem::path(u8"\u7EDF\u8BA1");std::filesystem::create_directories(root);
        const auto path=root/report->filename;
        check(gm::write_text_report(path,report->bytes).has_value() && read(path)==report->bytes,"standard staged export to native Unicode path");
        check(!gm::write_text_report(root/"missing"/"file",report->bytes),"file publication failure reported");
        check(gm::write_text_report(path,{}).has_value() && read(path).empty(),"empty report replacement");
        check(gm::write_text_report(path,report->bytes).has_value(),"report replacement restores complete bytes");
        s.system_money=INT64_MIN;s.system_gems=INT64_MIN;s.player_gems=INT64_MAX;
        check(gm::exchange_summary(s).find("-922337203685477.00")!=std::string::npos,"whole unit conversion avoids signed overflow");
        check(gm::exchange_summary(s).find("92233720368547758.07")!=std::string::npos,"summary keeps full-width gem fraction");
        std::cout<<checks<<" GM exchange-statistics checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
