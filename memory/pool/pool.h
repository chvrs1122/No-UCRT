#pragma once

#include <cstddef>
#include <cstdint>

namespace Pool
{
    struct BlockHeader
    {
        size_t iTotal;
        BlockHeader* pNext;
    };

    inline uint8_t* pPoolBase = nullptr;
    inline uint8_t* pPoolBump = nullptr;
    inline uint8_t* pPoolEnd = nullptr;
    inline BlockHeader* pFreeList = nullptr;

    bool Init();

    void* Alloc(size_t iSize);
    void Free(void* pPtr);
}
