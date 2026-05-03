#pragma once

#include <cstdint>
#include <cstddef>

namespace Syscall
{
    inline static unsigned int s_uSsnNtAllocate = 0;
    inline static unsigned int s_uSsnNtDelayExecution = 0;
    inline static unsigned int s_uSsnNtYieldExecution = 0;

    bool Init();

    bool AllocateVirtualMemory(void** ppBase, size_t iSize);
    bool DelayExecution(long long iDelayHns);
    bool YieldExecution();
}
