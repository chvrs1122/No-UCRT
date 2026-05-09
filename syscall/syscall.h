#pragma once

#include <cstdint>
#include <cstddef>

namespace Syscall
{
    static unsigned int ExtractSsn(void* pStub);

    inline static unsigned int s_uSsnNtDelayExecution = 0;
    inline static unsigned int s_uSsnNtYieldExecution = 0;

    bool Init();

    bool DelayExecution(long long iDelayHns);
    bool YieldExecution();
}
