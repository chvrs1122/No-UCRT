#include "pool.h"
#include "../mem.h"
#include "../../syscall/syscall.h"

static size_t Pool::AlignUp(size_t iSize)
{
    return (iSize + BLOCK_ALIGN - 1) & ~(BLOCK_ALIGN - 1);
}

bool Pool::Init()
{
    if (Pool::pPoolBase != nullptr)
    {
        return true;
    }

    // ensure syscalls are initialized !!
    void* pBase = nullptr;
    size_t iSize = POOL_SIZE;

    if (!Syscall::AllocateVirtualMemory(&pBase, iSize))
    {
        return false;
    }

    Pool::pPoolBase = (uint8_t*)pBase;
    Pool::pPoolBump = Pool::pPoolBase;
    Pool::pPoolEnd = Pool::pPoolBase + iSize;
    Pool::pFreeList = nullptr;

    return true;
}

void* Pool::Alloc(size_t iSize)
{
    if (Pool::pPoolBase == nullptr)
    {
        return nullptr;
    }

    size_t iAligned = AlignUp(iSize);
    size_t iTotal = sizeof(Pool::BlockHeader) + iAligned;

    Pool::BlockHeader* pPrev = nullptr;
    Pool::BlockHeader* pCur = Pool::pFreeList;

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

            pCur->pNext = nullptr;
            return (void*)((uint8_t*)pCur + sizeof(Pool::BlockHeader));
        }

        pPrev = pCur;
        pCur = pCur->pNext;
    }

    if (Pool::pPoolBump + iTotal > Pool::pPoolEnd)
    {
        return nullptr;
    }

    Pool::BlockHeader* pHdr = (Pool::BlockHeader*)Pool::pPoolBump;
    pHdr->iTotal = iTotal;
    pHdr->pNext = nullptr;
    Pool::pPoolBump += iTotal;

    return (void*)((uint8_t*)pHdr + sizeof(Pool::BlockHeader));
}

void Pool::Free(void* pPtr)
{
    if (pPtr == nullptr)
    {
        return;
    }

    Pool::BlockHeader* pHdr = (Pool::BlockHeader*)((uint8_t*)pPtr - sizeof(Pool::BlockHeader));
    pHdr->pNext = Pool::pFreeList;
    Pool::pFreeList = pHdr;
}
