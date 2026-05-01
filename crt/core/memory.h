#pragma once

#include "common.h"

namespace std
{
    inline void* memset(void* pDst, int iVal, size_t iLen)
    {
        uint8_t* pBytes = (uint8_t*)pDst;
        uint8_t bVal = (uint8_t)iVal;

        for (size_t i = 0; i < iLen; i++)
        {
            pBytes[i] = bVal;
        }

        return pDst;
    }

    inline void* memcpy(void* pDst, const void* pSrc, size_t iLen)
    {
        uint8_t* pD = (uint8_t*)pDst;
        const uint8_t* pS = (const uint8_t*)pSrc;

        for (size_t i = 0; i < iLen; i++)
        {
            pD[i] = pS[i];
        }

        return pDst;
    }

    inline void* memmove(void* pDst, const void* pSrc, size_t iLen)
    {
        uint8_t* pD = (uint8_t*)pDst;
        const uint8_t* pS = (const uint8_t*)pSrc;

        if (pD == pS || iLen == 0)
        {
            return pDst;
        }

        if (pD < pS)
        {
            for (size_t i = 0; i < iLen; i++)
            {
                pD[i] = pS[i];
            }
        }
        else
        {
            for (size_t i = iLen; i > 0; i--)
            {
                pD[i - 1] = pS[i - 1];
            }
        }

        return pDst;
    }

    __forceinline int memcmp(const void* pBuf1, const void* pBuf2, size_t iLen)
    {
        const uint8_t* pA = (const uint8_t*)pBuf1;
        const uint8_t* pB = (const uint8_t*)pBuf2;

        for (size_t i = 0; i < iLen; i++)
        {
            if (pA[i] != pB[i])
            {
                return (int)pA[i] - (int)pB[i];
            }
        }

        return 0;
    }
}
