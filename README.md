# No-UCRT
A C++ library for Windows x64 that removes all CRT dependencies.

Feel free to contribute !
 
## Features
**Memory**
- Custom Pool Allocator: set to 8MB
- Heap: wrapper for `operator new/delete` & `malloc/free`
- Direct Syscall: extracts SSNs from ntdll stubs at runtime for NtAllocateVirtualMemory

**CRT**
- `vector`: growable, push_back, erase, copy/move
- `array`, `span`: fixed size container and non-owning view
- `string`: append, substr, find, starts_with, ends_with
- `string_view`: non-owning string view
- `optional`: holds a value or nothing
- `variant`: tagged union, type-safe get
- `function`: type-erased callable with small-object optimization
- `hash`: FNV-1a 64-bit for int, uint, long long, pointer, string
- `unordered_map`: hash map, insert, find, contains, erase, operator[]
- `addressof`: returns the actual address of an object
- `sort`, `find`, `find_if`, `min`, `max`, `clamp`: algorithm helpers
- `sqrt`, `sin`, `cos`, `tan`, `atan2`, `pow`, `abs`, `isfinite`: math
- `printf`: WriteConsoleA output, `%d/%u/%x/%X/%p/%s/%c/%f`, width, zero-pad, precision, `%ll`/`%z`

## Example
```cpp
#define NOMINMAX
#include <windows.h>
#include "memory/heap/heap.h"
#include "syscall/syscall.h"
#include "crt/std.h"

int main()
{
    if (!NoUCRT::Initialize()) {
        // handle

        return false;
    }

    std::vector<int> vector;
    vector.push_back(1);
    vector.push_back(2);

    std::string string = "hello";
    string += " world";

    std::printf("vec[0]=%d str=%s sqrt2=%.4f\n", vector[0], string.c_str(), std::sqrt(2.0));
    return 0;
}
```

## Building
```
build.cmd
```

Compiler flags used:
```
clang-cl /nologo /c /O2 /GS- /Gs9999999 /GR- /Gy /Zl /Brepro
    /std:c++latest
    /guard:cf- /EHs-c- /Zc:threadSafeInit-
    /clang:-fno-builtin /clang:-flto=thin
    /I . /D_CRT_SECURE_NO_WARNINGS /DWIN32_LEAN_AND_MEAN /D_NO_CRT_STDIO_INLINE
```

Linker flags used:
```
lld-link /NOLOGO /NODEFAULTLIB /ENTRY:main /SUBSYSTEM:CONSOLE
    /GUARD:NO /Brepro /OPT:REF /OPT:ICF
    kernel32.lib
```

Compile Flags
```
-x
c++
-I.
-std=c++23
-DWIN32_LEAN_AND_MEAN
-D_CRT_SECURE_NO_WARNINGS
-D_NO_CRT_STDIO_INLINE
-DNOMINMAX
-D_CSTDLIB_
```

## Notes
- `Syscall::Init()` must run before `Heap::Init()`.
- `printf` outputs to stdout via WriteConsoleA.
- Its suggested to implement your own pool allocator

## Credits
https://github.com/mludvig/mini-printf