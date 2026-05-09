#include "heap.h"
#include "../pool/pool.h"
#include <cstdint>

bool Heap::Init()
{
    return Pool::Init();
}

void* Heap::Alloc(size_t iSize)
{
    return Pool::Alloc(iSize);
}

void Heap::Free(void* pPtr)
{
    Pool::Free(pPtr);
}

void* operator new(size_t iSize)
{
    return Pool::Alloc(iSize);
}

void* operator new[](size_t iSize)
{
    return Pool::Alloc(iSize);
}

void operator delete(void* pPtr) noexcept
{
    Pool::Free(pPtr);
}

void operator delete[](void* pPtr) noexcept
{
    Pool::Free(pPtr);
}

void operator delete(void* pPtr, size_t) noexcept
{
    Pool::Free(pPtr);
}

void operator delete[](void* pPtr, size_t) noexcept
{
    Pool::Free(pPtr);
}

extern "C" void* malloc(size_t iSize)
{
    return Pool::Alloc(iSize);
}

extern "C" void free(void* pPtr)
{
    Pool::Free(pPtr);
}

extern "C" void* memset(void* pDst, int iVal, size_t iLen)
{
    uint8_t* pBytes = (uint8_t*)pDst;
    uint8_t bVal = (uint8_t)iVal;

    for (size_t i = 0; i < iLen; i++)
    {
        pBytes[i] = bVal;
    }

    return pDst;
}

extern "C" void* memcpy(void* pDst, const void* pSrc, size_t iLen)
{
    uint8_t* pD = (uint8_t*)pDst;
    const uint8_t* pS = (const uint8_t*)pSrc;

    for (size_t i = 0; i < iLen; i++)
    {
        pD[i] = pS[i];
    }

    return pDst;
}
