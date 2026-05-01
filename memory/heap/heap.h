#pragma once
#include <cstddef>

extern "C" void* malloc(size_t iSize);
extern "C" void free(void* pPtr);

namespace Heap
{
    bool Init();

    void* Alloc(size_t iSize);
    void Free(void* pPtr);
}
