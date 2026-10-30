#pragma once
#include "session_mgr.h"
#include <expected>
#include <optional>
#include <span>
#include <string>

namespace maintain
{
struct Metric { UINT64 current=0, average=0, minimum=0, maximum=0, total=0; };
using Statistics = std::array<Metric,statTotal>;
struct History
{
    std::array<std::optional<UINT64>,ACCUMU_MAX_TIMES> values{};
    UINT64 maximum=0;
};
std::expected<void,std::string> ingest_statistics(stProcessStatisInfo& destination,
    const stStatisInfo& message,std::span<const SProcessInfo> processes);
std::optional<Statistics> statistics(const stProcessStatisInfo& info,UINT32 pid);
History history(const stProcessStatisInfo& info,UINT32 pid,INT32 field);
void forget_statistics(stProcessStatisInfo& info,UINT32 pid);
}
