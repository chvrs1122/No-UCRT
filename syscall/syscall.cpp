#include <windows.h>
#include "syscall.h"
#include "../memory/mem.h"
#include "../crt/std.h"

static unsigned int ExtractSsn(void* pStub)
{
    if (pStub == nullptr)
    {
        return 0;
    }

    const uint8_t* pBytes = (const uint8_t*)pStub;

    if (pBytes[0] != 0x4C || pBytes[1] != 0x8B || pBytes[2] != 0xD1)
    {
        return 0;
    }

    if (pBytes[3] != 0xB8)
    {
        return 0;
    }

    unsigned int uSsn = 0;
    uSsn |= (unsigned int)pBytes[4];
    uSsn |= (unsigned int)pBytes[5] << 8;
    uSsn |= (unsigned int)pBytes[6] << 16;
    uSsn |= (unsigned int)pBytes[7] << 24;

    return uSsn;
}

bool Syscall::Init()
{
    void* pNtdll = Memory::FindModuleByName("ntdll.dll");

    if (pNtdll == nullptr)
    {
        return false;
    }

    void* pProc = Memory::FindExport(pNtdll, "NtAllocateVirtualMemory");

    if (pProc == nullptr)
    {
        return false;
    }

    s_uSsnNtAllocate = ExtractSsn(pProc);

    if (s_uSsnNtAllocate == 0)
    {
        return false;
    }

    void* pDelayProc = Memory::FindExport(pNtdll, "NtDelayExecution");

    if (pDelayProc == nullptr)
    {
        return false;
    }

    s_uSsnNtDelayExecution = ExtractSsn(pDelayProc);

    if (s_uSsnNtDelayExecution == 0)
    {
        return false;
    }

    void* pYieldProc = Memory::FindExport(pNtdll, "NtYieldExecution");

    if (pYieldProc == nullptr)
    {
        return false;
    }

    s_uSsnNtYieldExecution = ExtractSsn(pYieldProc);

    if (s_uSsnNtYieldExecution == 0)
    {
        return false;
    }

    return true;
}

__attribute__((naked))
static long SyscallNtAllocateVirtualMemory(void* hProcess, void** ppBase, uintptr_t uZeroBits, size_t* pSize, unsigned long uType, unsigned long uProt, unsigned int uSsn)
{
    __asm {
        mov r10, rcx
        mov eax, [rsp + 0x38]
        syscall
        ret
    };
}

static __forceinline bool AllocateVirtualMemoryImpl(void** ppBase, size_t iSize)
{
    if (Syscall::s_uSsnNtAllocate == 0)
    {
        return false;
    }

    long lStatus = SyscallNtAllocateVirtualMemory((void*)(intptr_t)-1, ppBase, 0, &iSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE, Syscall::s_uSsnNtAllocate);

    if (lStatus < 0 || *ppBase == nullptr)
    {
        return false;
    }

    return true;
}

bool Syscall::AllocateVirtualMemory(void** ppBase, size_t iSize)
{
    return AllocateVirtualMemoryImpl(ppBase, iSize);
}

__attribute__((naked))
static long SyscallNtDelayExecution(unsigned char bAlertable, long long* pDelay, unsigned int uSsn)
{
    __asm {
        mov r10, rcx
        mov eax, r8d
        syscall
        ret
    };
}

static __forceinline bool DelayExecutionImpl(long long iDelayHns)
{
    if (Syscall::s_uSsnNtDelayExecution == 0)
    {
        return false;
    }

    long lStatus = SyscallNtDelayExecution(0, &iDelayHns, Syscall::s_uSsnNtDelayExecution);

    return lStatus >= 0;
}

bool Syscall::DelayExecution(long long iDelayHns)
{
    return DelayExecutionImpl(iDelayHns);
}

__attribute__((naked))
static long SyscallNtYieldExecution(unsigned int uSsn)
{
    __asm {
        mov r10, rcx
        mov eax, ecx
        syscall
        ret
    };
}

static __forceinline bool YieldExecutionImpl()
{
    if (Syscall::s_uSsnNtYieldExecution == 0)
    {
        return false;
    }

    long lStatus = SyscallNtYieldExecution(Syscall::s_uSsnNtYieldExecution);

    return lStatus >= 0;
}

bool Syscall::YieldExecution()
{
    return YieldExecutionImpl();
}
