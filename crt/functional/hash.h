#pragma once

#include "../core/common.h"
#include "../string/string.h"
#include "../string/wstring.h"

namespace std
{
    /* @brief FNV-1a 64-bit hash func*/
    template<typename TChar, typename TLen>
    __forceinline uint64_t fnv1a_bytes(const TChar* pData, TLen iLen)
    {
        uint64_t uHash = 0xcbf29ce484222325ULL;

        for (TLen i = 0; i < iLen; i++)
        {
            uHash ^= (uint64_t)(uint8_t)pData[i];
            uHash *= 0x100000001b3ULL;
        }

        return uHash;
    }

    template<typename T>
    struct hash;

    template<>
    struct hash<int>
    {
        size_t operator()(int iVal) const noexcept
        {
            return (size_t)fnv1a_bytes((const uint8_t*)&iVal, sizeof(iVal));
        }
    };

    template<>
    struct hash<unsigned int>
    {
        size_t operator()(unsigned int uVal) const noexcept
        {
            return (size_t)fnv1a_bytes((const uint8_t*)&uVal, sizeof(uVal));
        }
    };

    template<>
    struct hash<long long>
    {
        size_t operator()(long long iVal) const noexcept
        {
            return (size_t)fnv1a_bytes((const uint8_t*)&iVal, sizeof(iVal));
        }
    };

    template<>
    struct hash<unsigned long long>
    {
        size_t operator()(unsigned long long uVal) const noexcept
        {
            return (size_t)fnv1a_bytes((const uint8_t*)&uVal, sizeof(uVal));
        }
    };

    template<typename T>
    struct hash<T*>
    {
        size_t operator()(T* pVal) const noexcept
        {
            uintptr_t uPtr = (uintptr_t)pVal;
            return (size_t)fnv1a_bytes((const uint8_t*)&uPtr, sizeof(uPtr));
        }
    };

    template<>
    struct hash<const char*>
    {
        size_t operator()(const char* pStr) const noexcept
        {
            return (size_t)fnv1a_bytes(pStr, std::strlen(pStr));
        }
    };

    template<>
    struct hash<const wchar_t*>
    {
        size_t operator()(const wchar_t* pStr) const noexcept
        {
            size_t iLen = std::wcslen(pStr);
            return (size_t)fnv1a_bytes((const uint8_t*)pStr, iLen * sizeof(wchar_t));
        }
    };
}
