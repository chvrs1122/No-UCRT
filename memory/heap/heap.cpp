#include "heap.h"
#include "../pool/pool.h"

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
