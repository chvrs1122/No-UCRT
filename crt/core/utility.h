#pragma once

#include "common.h"
#include "type_traits.h"
#include <utility>

inline void* operator new(size_t, void* pPtr) noexcept
{
    return pPtr;
}

inline void* operator new[](size_t, void* pPtr) noexcept
{
    return pPtr;
}

inline void operator delete(void*, void*) noexcept
{
}

inline void operator delete[](void*, void*) noexcept
{
}