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

    static size_t AlignUp(size_t iSize);
    inline static const size_t POOL_SIZE = 8 * 1024 * 1024; // 8 MB
    inline static const size_t BLOCK_ALIGN = 16; // 1 byte

    inline unsigned int uSsnNtAllocate = 0;
    inline uint8_t* pPoolBase = nullptr;
    inline uint8_t* pPoolBump = nullptr;
    inline uint8_t* pPoolEnd = nullptr;
    inline BlockHeader* pFreeList = nullptr;

    bool Init();

    void* Alloc(size_t iSize);
    void Free(void* pPtr);
}
