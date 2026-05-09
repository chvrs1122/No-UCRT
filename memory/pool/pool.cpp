#include "pool.h"
#include "../mem.h"

bool Pool::Init()
{
    if (Pool::pPoolBase != nullptr)
    {
        return true;
    }

    static uint8_t pPoolBuffer[2 * 1024 * 1024]; // 2 MB

    Pool::pPoolBase = pPoolBuffer;
    Pool::pPoolBump = Pool::pPoolBase;
    Pool::pPoolEnd = Pool::pPoolBase + sizeof(pPoolBuffer);
    Pool::pFreeList = nullptr;

    return true;
}

void* Pool::Alloc(size_t iSize)
{
    if (Pool::pPoolBase == nullptr)
    {
        return nullptr;
    }

    size_t iAligned = (iSize + 15) & ~(size_t)15; // 16 byte alignment
    size_t iTotal = sizeof(Pool::BlockHeader) + iAligned;

    Pool::BlockHeader* pPrev = nullptr;
    Pool::BlockHeader* pCur = Pool::pFreeList;

    // first fit
    while (pCur != nullptr)
    {
        if (pCur->iTotal >= iTotal)
        {
            if (pPrev != nullptr)
            {
                pPrev->pNext = pCur->pNext;
            }
            else
            {
                Pool::pFreeList = pCur->pNext;
            }

            // return block
            pCur->pNext = nullptr;
            return (void*)((uint8_t*)pCur + sizeof(Pool::BlockHeader));
        }

        pPrev = pCur;
        pCur = pCur->pNext;
    }

    // allocate new block
    if (Pool::pPoolBump + iTotal > Pool::pPoolEnd)
    {
        return nullptr;
    }

    // setup header
    Pool::BlockHeader* pHdr = (Pool::BlockHeader*)Pool::pPoolBump;
    pHdr->iTotal = iTotal;
    pHdr->pNext = nullptr;
    Pool::pPoolBump += iTotal;

    return (void*)((uint8_t*)pHdr + sizeof(Pool::BlockHeader));
}

void Pool::Free(void* pPtr)
{
    // ignore invalid pointers
    if (pPtr == nullptr)
    {
        return;
    }

    // free block 
    Pool::BlockHeader* pHdr = (Pool::BlockHeader*)((uint8_t*)pPtr - sizeof(Pool::BlockHeader));
    pHdr->pNext = Pool::pFreeList;
    Pool::pFreeList = pHdr;
}
