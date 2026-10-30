#include "maintain_stats.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace
{
int checks=0,failures=0;
void check(bool value,const char* name) { ++checks;if (!value) { ++failures;std::fprintf(stderr,"FAIL: %s\n",name); } }
void run()
{
    auto info=std::make_unique<stProcessStatisInfo>();
    auto message=std::make_unique<stStatisInfo>();
    std::vector<SProcessInfo> metadata(2);metadata[0].dwProcessID=100;metadata[1].dwProcessID=200;
    auto& first=message->ProcessInfo[0];first.bUsed=TRUE;first.dwProcessID=100;first.qwStatisEntry[statMemUsage]=10;
    auto& second=message->ProcessInfo[1];second.bUsed=TRUE;second.dwProcessID=200;second.qwStatisEntry[statMemUsage]=20;
    check(maintain::ingest_statistics(*info,*message,metadata).has_value(),"first sample is ingested");
    auto values=maintain::statistics(*info,100);
    check(values && (*values)[statMemUsage].minimum==10 && (*values)[statMemUsage].maximum==10 &&
        (*values)[statMemUsage].total==10,"first real value initializes minimum, maximum and total");
    std::memset(message.get(),0,sizeof(*message));
    message->ProcessInfo[5].bUsed=TRUE;message->ProcessInfo[5].dwProcessID=100;message->ProcessInfo[5].qwStatisEntry[statMemUsage]=5;
    message->ProcessInfo[0].bUsed=TRUE;message->ProcessInfo[0].dwProcessID=200;message->ProcessInfo[0].qwStatisEntry[statMemUsage]=30;
    check(maintain::ingest_statistics(*info,*message,metadata).has_value(),"PID can move between wire slots");
    values=maintain::statistics(*info,100);
    check(values && (*values)[statMemUsage].minimum==5 && (*values)[statMemUsage].maximum==10 &&
        (*values)[statMemUsage].total==15,"slot movement retains only that PID's aggregates");
    check((*maintain::statistics(*info,200))[statMemUsage].total==50,"another PID has independent totals");
    const auto saved=info->totals.at(100);
    for (int redraw=0;redraw<20;++redraw) (void)maintain::statistics(*info,100);
    check(info->totals.at(100)[statMemUsage].qwTotal==saved[statMemUsage].qwTotal,"reading statistics cannot inflate totals");
    const auto index=info->nAccumuIndex;
    message->ProcessInfo[1]=message->ProcessInfo[5];
    check(!maintain::ingest_statistics(*info,*message,metadata) && info->nAccumuIndex==index &&
        info->totals.at(100)[statMemUsage].qwTotal==15,"duplicate PID rejects a sample before mutating state");
    message->ProcessInfo[1].dwProcessID=0;
    check(!maintain::ingest_statistics(*info,*message,metadata) && info->nAccumuIndex==index,"used zero-PID record is rejected atomically");
    for (int sample=0;sample<100;++sample)
    {
        std::memset(message.get(),0,sizeof(*message));
        auto& process=message->ProcessInfo[sample%MAX_STATIS_PROCESS_COUNT];
        process.bUsed=TRUE;process.dwProcessID=100;
        process.qwStatisEntry[statMemUsage]=sample+1;
        process.qwStatisEntry[statCpuUsage]=std::numeric_limits<UINT64>::max();
        if (!maintain::ingest_statistics(*info,*message,metadata)) throw std::runtime_error("valid rolling sample failed");
    }
    values=maintain::statistics(*info,100);
    check(values && (*values)[statMemUsage].average==50 && (*values)[statMemUsage].minimum==1 &&
        (*values)[statMemUsage].maximum==100 && (*values)[statMemUsage].total==5065,
        "history rolls independently of lifetime aggregates and correlates PID movement");
    check((*values)[statCpuUsage].average==std::numeric_limits<UINT64>::max() &&
        (*values)[statCpuUsage].total==std::numeric_limits<UINT64>::max(),"mean remains exact and lifetime total saturates at UINT64 maximum");
    const auto history=maintain::history(*info,100,statMemUsage);
    check(history.maximum==100 && history.values.front()==1 && history.values.back()==100,"history is chronological across the ring cursor");
    check(std::ranges::all_of(history.values,[](const auto& value) { return value.has_value(); }),"moving PID does not create false graph gaps");
    const auto absent=maintain::history(*info,999,statMemUsage);
    check(!absent.maximum && std::ranges::none_of(absent.values,[](const auto& value) { return value.has_value(); }),
        "another process's slot contents cannot appear in an absent PID graph");
    check(!maintain::statistics(*info,0) && !maintain::statistics(*info,999),"unknown current PIDs have no statistics view");
    check(!maintain::history(*info,100,statTotal).maximum,"invalid metric cannot index the wire array");
    maintain::forget_statistics(*info,100);
    check(!info->totals.contains(100) && !maintain::statistics(*info,100) &&
        !maintain::history(*info,100,statMemUsage).maximum,"process retirement clears current, history and lifetime state");
    std::memset(message.get(),0,sizeof(*message));
    message->ProcessInfo[0].bUsed=TRUE;message->ProcessInfo[0].dwProcessID=100;message->ProcessInfo[0].qwStatisEntry[statMemUsage]=7;
    check(maintain::ingest_statistics(*info,*message,metadata).has_value() &&
        (*maintain::statistics(*info,100))[statMemUsage].total==7,"reused PID starts a fresh aggregate lifetime");
    const auto fresh=maintain::history(*info,100,statMemUsage);
    check(std::ranges::count_if(fresh.values,[](const auto& value) { return value.has_value(); })==1,"reused PID cannot inherit retired graph samples");
    metadata.clear();std::memset(message.get(),0,sizeof(*message));
    check(maintain::ingest_statistics(*info,*message,metadata).has_value() && info->totals.empty(),"inactive PIDs absent from metadata are pruned");
}
}
int main()
{
    try { run(); } catch (const std::exception& error) { check(false,error.what()); }
    std::printf("maintain statistics: %d checks, %d failures\n",checks,failures);return failures!=0;
}
