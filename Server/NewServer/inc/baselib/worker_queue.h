#ifndef CQYH_BASELIB_WORKER_QUEUE_H
#define CQYH_BASELIB_WORKER_QUEUE_H

#include "circle_list.h"
#include "thread.h"
#include <chrono>

namespace cqyh
{
// The caller retains ownership until publication succeeds. A full result queue
// must not lose completed work or prevent the worker from stopping cooperatively.
inline bool PublishUntilStopped(CCircleList& output, void* record, CThread& worker)
{
    if (!record)
        return false;
    while (!output.Push(record))
        if (worker.WaitForStop(std::chrono::milliseconds(1)))
            return false;
    return true;
}
}
#endif // CQYH_BASELIB_WORKER_QUEUE_H
