# No-UCRT
A C++ library for Windows x64 that removes all CRT dependencies.
 
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
- `hash`: FNV-1a 64-bit, specializations for int, uint, long long, pointer, string
- `sort`, `find`, `find_if`, `min`, `max`, `clamp`: algorithm helpers
- `sqrt`, `sin`, `cos`, `tan`, `atan2`, `pow`, `abs`, `isfinite`: math
- `printf`: WriteConsoleA output, `%d/%u/%x/%X/%p/%s/%c/%f`, width, zero-pad, precision, `%ll`/`%z`

## Usage
```cpp
#define NOMINMAX
#include <windows.h>
#include "memory/heap/heap.h"
#include "syscall/syscall.h"
#include "crt/std.h"

int main()
{
    Syscall::Init();  // must come before Heap::Init
    Heap::Init();

    std::vector<int> vector;
    vector.push_back(1);
    vector.push_back(2);

    std::string string = "hello";
    string += " world";

    std::printf("vec[0]=%d str=%s sqrt2=%.4f\n", vector[0], string.c_str(), std::sqrt(2.0));
    return 0;
}
```

## Notes
- `Syscall::Init()` must run before `Heap::Init()`.
- `printf` outputs to stdout via WriteConsoleA. 

## Credits
https://github.com/mludvig/mini-printf