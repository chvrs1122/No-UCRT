#pragma once

#include <cstdint>
#include <cstddef>

namespace Syscall
{
    inline static unsigned int s_uSsnNtAllocate = 0;

    bool Init();

    bool AllocateVirtualMemory(void** ppBase, size_t iSize);
}
