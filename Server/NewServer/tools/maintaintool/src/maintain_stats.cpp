#include "maintain_stats.h"
#include <algorithm>
#include <limits>
#include <unordered_set>

namespace maintain
{
namespace
{
const stStatisProcess* find(const stStatisInfo& info,UINT32 pid)
{
    const auto process=std::ranges::find_if(info.ProcessInfo,[pid](const auto& entry) {
        return entry.bUsed && entry.dwProcessID==pid;
    });
    return process==std::end(info.ProcessInfo) ? nullptr : process;
}
UINT64 add(UINT64 left,UINT64 right)
{
    const auto maximum=std::numeric_limits<UINT64>::max();
    return right>maximum-left ? maximum : left+right;
}
}
std::expected<void,std::string> ingest_statistics(stProcessStatisInfo& info,
    const stStatisInfo& message,std::span<const SProcessInfo> processes)
try
{
    std::unordered_set<UINT32> ids;
    for (const auto& process:message.ProcessInfo)
        if (process.bUsed && (!process.dwProcessID || !ids.insert(process.dwProcessID).second))
            return std::unexpected("invalid or duplicate statistics PID");
    auto totals=info.totals; // Validate and allocate before publishing this sample.
    for (const auto& process:message.ProcessInfo)
    {
        if (!process.bUsed) continue;
        auto [entry,inserted]=totals.try_emplace(process.dwProcessID);
        for (int field=statInvalid+1;field<statTotal;++field)
        {
            const auto value=process.qwStatisEntry[field];
            auto& metric=entry->second[field];
            if (inserted) metric={value,value,value};
            else
            {
                metric.qwMin=std::min(metric.qwMin,value);
                metric.qwMax=std::max(metric.qwMax,value);
                metric.qwTotal=add(metric.qwTotal,value);
            }
        }
    }
    std::erase_if(totals,[&](const auto& entry) {
        return !ids.contains(entry.first) && std::ranges::none_of(processes,[&](const auto& process) {
            return process.dwProcessID==entry.first;
        });
    });
    const auto index=info.nAccumuIndex>=0 && info.nAccumuIndex<ACCUMU_MAX_TIMES ? info.nAccumuIndex : 0;
    std::ranges::copy(message.ProcessInfo,info.ProcessInfo);
    info.oAccumuStatis[index]=message;
    info.nAccumuIndex=(index+1)%ACCUMU_MAX_TIMES;
    info.totals.swap(totals);
    return {};
}
catch (const std::exception& error) { return std::unexpected(error.what()); }

std::optional<Statistics> statistics(const stProcessStatisInfo& info,UINT32 pid)
{
    if (!pid) return {};
    const auto* current=find(info,pid);
    if (!current) return {};
    Statistics result{};
    const auto totals=info.totals.find(pid);
    for (int field=statInvalid+1;field<statTotal;++field)
    {
        auto& value=result[field];
        value.current=current->qwStatisEntry[field];
        value.minimum=value.maximum=value.total=value.current;
        if (totals!=info.totals.end())
        {
            value.minimum=totals->second[field].qwMin;
            value.maximum=totals->second[field].qwMax;
            value.total=totals->second[field].qwTotal;
        }
        UINT64 quotient=0,remainder=0;
        for (const auto& sample:info.oAccumuStatis)
            if (const auto* process=find(sample,pid))
            {
                quotient+=process->qwStatisEntry[field]/ACCUMU_MAX_TIMES;
                remainder+=process->qwStatisEntry[field]%ACCUMU_MAX_TIMES;
            }
        value.average=quotient+remainder/ACCUMU_MAX_TIMES;
    }
    return result;
}
History history(const stProcessStatisInfo& info,UINT32 pid,INT32 field)
{
    History result;
    if (!pid || field<=statInvalid || field>=statTotal) return result;
    const auto first=info.nAccumuIndex>=0 && info.nAccumuIndex<ACCUMU_MAX_TIMES ? info.nAccumuIndex : 0;
    for (int index=0;index<ACCUMU_MAX_TIMES;++index)
        if (const auto* process=find(info.oAccumuStatis[(first+index)%ACCUMU_MAX_TIMES],pid))
        {
            const auto value=process->qwStatisEntry[field];
            result.values[index]=value;
            result.maximum=std::max(result.maximum,value);
        }
    return result;
}
void forget_statistics(stProcessStatisInfo& info,UINT32 pid)
{
    info.totals.erase(pid);
    const auto clear=[pid](stStatisInfo& sample) {
        for (auto& process:sample.ProcessInfo) if (process.dwProcessID==pid) process={};
    };
    clear(info);
    for (auto& sample:info.oAccumuStatis) clear(sample);
}
}
