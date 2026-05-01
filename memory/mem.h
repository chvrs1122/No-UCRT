#pragma once

#include <cstdint>
#include <cstddef>

struct ListEntry64
{
    ListEntry64* Flink;
    ListEntry64* Blink;
};


struct PebLdrData64
{
    unsigned long Length;
    unsigned char Initialized;
    void* SsHandle;
    ListEntry64 InLoadOrderModuleList;
    ListEntry64 InMemoryOrderModuleList;
    ListEntry64 InInitializationOrderModuleList;
};


struct UnicodeString64
{
    unsigned short Length;
    unsigned short MaximumLength;
    wchar_t* Buffer;
};


struct LdrDataTableEntry64
{
    ListEntry64 InLoadOrderLinks;
    ListEntry64 InMemoryOrderLinks;
    ListEntry64 InInitializationOrderLinks;
    void* DllBase;
    void* EntryPoint;
    unsigned long SizeOfImage;
    UnicodeString64 FullDllName;
    UnicodeString64 BaseDllName;
};

namespace Memory
{
    void* FindModuleByName(const char* pName);

    void* FindExport(void* pBase, const char* pName);
}
