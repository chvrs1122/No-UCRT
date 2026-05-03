#pragma once

#include "std.h"
#include <syscall/syscall.h>
#include <memory/heap/heap.h>

namespace NoUCRT
{
    [[nodiscard]] inline bool Initialize()
    {
        if (!Syscall::Init())
        {
            return false;
        }

        if (!Heap::Init())
        {
            return false;
        }

        return true;
    }
}
