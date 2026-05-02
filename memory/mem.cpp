#include <windows.h>
#include <winternl.h>
#include "mem.h"
#include "../crt/std.h"


__attribute__((always_inline)) static PebLdrData64* GetPebLdr()
{
    PEB* pPeb = (PEB*)__readgsqword(0x60);

    if (pPeb == nullptr)
    {
        return nullptr;
    }

    return (PebLdrData64*)pPeb->Ldr;
}


__attribute__((always_inline)) void* Memory::FindModuleByName(const char* pName)
{
    size_t iLen = std::strlen(pName);

    PebLdrData64* pLdr = GetPebLdr();

    if (pLdr == nullptr)
    {
        return nullptr;
    }

    ListEntry64* pHead = &pLdr->InLoadOrderModuleList;
    ListEntry64* pCur = pHead->Flink;

    while (pCur != pHead)
    {
        LdrDataTableEntry64* pEntry = (LdrDataTableEntry64*)pCur;
        size_t iEntryLen = (size_t)(pEntry->BaseDllName.Length / 2);

        if (iEntryLen == iLen)
        {
            int iMismatch = std::wcsnicmp_ascii(pEntry->BaseDllName.Buffer, pName, iLen);

            if (iMismatch == 0)
            {
                return pEntry->DllBase;
            }
        }

        pCur = pCur->Flink;
    }

    return nullptr;
}


__attribute__((always_inline)) void* Memory::FindExport(void* pBase, const char* pName)
{
    if (pBase == nullptr)
    {
        return nullptr;
    }

    // reference: https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#export-directory-table
    uint8_t* pImage = (uint8_t*)pBase;
    IMAGE_DOS_HEADER* pDos = (IMAGE_DOS_HEADER*)pImage;

    if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return nullptr;
    }

    IMAGE_NT_HEADERS64* pNt = (IMAGE_NT_HEADERS64*)(pImage + pDos->e_lfanew);

    if (pNt->Signature != IMAGE_NT_SIGNATURE)
    {
        return nullptr;
    }

    IMAGE_DATA_DIRECTORY* pDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    if (pDir->VirtualAddress == 0 || pDir->Size == 0)
    {
        return nullptr;
    }

    IMAGE_EXPORT_DIRECTORY* pExp = (IMAGE_EXPORT_DIRECTORY*)(pImage + pDir->VirtualAddress);
    unsigned long* pNames = (unsigned long*)(pImage + pExp->AddressOfNames);
    unsigned long* pFuncs = (unsigned long*)(pImage + pExp->AddressOfFunctions);
    unsigned short* pOrdinals = (unsigned short*)(pImage + pExp->AddressOfNameOrdinals);

    for (unsigned long i = 0; i < pExp->NumberOfNames; i++)
    {
        const char* pCandidate = (const char*)(pImage + pNames[i]);

        if (std::strcmp(pCandidate, pName) == 0)
        {
            unsigned short uOrd = pOrdinals[i];
            unsigned long uRva = pFuncs[uOrd];
            return (void*)(pImage + uRva);
        }
    }

    return nullptr;
}



