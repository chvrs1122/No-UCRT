#pragma once

#include "../core/common.h"
#include "string.h"

namespace std
{
    __forceinline size_t wcslen(const wchar_t* pStr)
    {
        const wchar_t* pCur = pStr;

        while (*pCur != L'\0')
        {
            pCur++;
        }

        return (size_t)(pCur - pStr);
    }

    __forceinline int wcscmp(const wchar_t* pStr1, const wchar_t* pStr2)
    {
        while (*pStr1 != L'\0' && *pStr1 == *pStr2)
        {
            pStr1++;
            pStr2++;
        }

        return (int)*pStr1 - (int)*pStr2;
    }

    __forceinline wchar_t* wcscpy(wchar_t* pDst, const wchar_t* pSrc)
    {
        wchar_t* pOut = pDst;

        while (*pSrc != L'\0')
        {
            *pOut = *pSrc;
            pOut++;
            pSrc++;
        }

        *pOut = L'\0';
        return pDst;
    }

    __forceinline wchar_t* wcscat(wchar_t* pDst, const wchar_t* pSrc)
    {
        wchar_t* pOut = pDst;

        while (*pOut != L'\0')
        {
            pOut++;
        }

        while (*pSrc != L'\0')
        {
            *pOut = *pSrc;
            pOut++;
            pSrc++;
        }

        *pOut = L'\0';
        return pDst;
    }

    __forceinline int towupper(int iC)
    {
        if (iC >= L'a' && iC <= L'z')
        {
            return iC - L'a' + L'A';
        }

        if (iC >= 0x0430 && iC <= 0x044F)
        {
            return iC - 0x0430 + 0x0410;
        }

        if (iC == 0x0451)
        {
            return 0x0401;
        }

        return iC;
    }

    __forceinline int towlower(int iC)
    {
        if (iC >= L'A' && iC <= L'Z')
        {
            return iC - L'A' + L'a';
        }

        if (iC >= 0x0410 && iC <= 0x042F)
        {
            return iC - 0x0410 + 0x0430;
        }

        if (iC == 0x0401)
        {
            return 0x0451;
        }

        return iC;
    }

    __forceinline bool iswdigit(wchar_t cC)
    {
        return cC >= L'0' && cC <= L'9';
    }

    __forceinline int wcsicmp(const wchar_t* pStr1, const wchar_t* pStr2)
    {
        if (pStr1 == nullptr || pStr2 == nullptr)
        {
            return -1;
        }

        while (*pStr1 != L'\0' && *pStr2 != L'\0')
        {
            int iA = std::towlower(*pStr1);
            int iB = std::towlower(*pStr2);

            if (iA != iB)
            {
                return iA - iB;
            }

            pStr1++;
            pStr2++;
        }

        return std::towlower(*pStr1) - std::towlower(*pStr2);
    }

    __forceinline int wcsnicmp(const wchar_t* pStr1, const wchar_t* pStr2, size_t iN)
    {
        if (iN == 0)
        {
            return 0;
        }

        while (iN > 0)
        {
            int iA = std::towlower(*pStr1);
            int iB = std::towlower(*pStr2);

            if (iA != iB)
            {
                return iA - iB;
            }

            if (*pStr1 == L'\0')
            {
                return 0;
            }

            pStr1++;
            pStr2++;
            iN--;
        }

        return 0;
    }

    __forceinline int wcsnicmp_ascii(const wchar_t* pWide, const char* pAscii, size_t iN)
    {
        for (size_t i = 0; i < iN; i++)
        {
            int iA = std::towlower(pWide[i]);
            int iB = std::tolower((uint8_t)pAscii[i]);

            if (iA != iB)
            {
                return iA - iB;
            }

            if (pAscii[i] == '\0')
            {
                return 0;
            }
        }

        return 0;
    }

    __forceinline wchar_t* wcschr(const wchar_t* pStr, wchar_t cC)
    {
        while (*pStr != L'\0')
        {
            if (*pStr == cC)
            {
                return (wchar_t*)pStr;
            }

            pStr++;
        }

        if (cC == L'\0')
        {
            return (wchar_t*)pStr;
        }

        return nullptr;
    }

    __forceinline wchar_t* wcsstr(const wchar_t* pStr, const wchar_t* pFind)
    {
        if (pStr == nullptr || pFind == nullptr)
        {
            return nullptr;
        }

        if (*pFind == L'\0')
        {
            return (wchar_t*)pStr;
        }

        while (*pStr != L'\0')
        {
            const wchar_t* pA = pStr;
            const wchar_t* pB = pFind;

            while (*pA != L'\0' && *pB != L'\0' && *pA == *pB)
            {
                pA++;
                pB++;
            }

            if (*pB == L'\0')
            {
                return (wchar_t*)pStr;
            }

            pStr++;
        }

        return nullptr;
    }

    __forceinline wchar_t* wcsrev(wchar_t* pStr)
    {
        wchar_t* pBeg = pStr;
        wchar_t* pEnd = pStr;

        while (*pEnd != L'\0')
        {
            pEnd++;
        }

        if (pEnd != pBeg)
        {
            pEnd--;
        }

        while (pBeg < pEnd)
        {
            wchar_t cTmp = *pBeg;
            *pBeg = *pEnd;
            *pEnd = cTmp;
            pBeg++;
            pEnd--;
        }

        return pStr;
    }

    __forceinline wchar_t* wcslwr(wchar_t* pStr)
    {
        wchar_t* pCur = pStr;

        while (*pCur != L'\0')
        {
            *pCur = (wchar_t)std::towlower(*pCur);
            pCur++;
        }

        return pStr;
    }

    __forceinline int wtoi(const wchar_t* pStr)
    {
        if (pStr == nullptr)
        {
            return 0;
        }

        while (*pStr == L' ' || *pStr == L'\t')
        {
            pStr++;
        }

        int iNeg = 0;

        if (*pStr == L'-')
        {
            iNeg = 1;
            pStr++;
        }
        else if (*pStr == L'+')
        {
            pStr++;
        }

        int iAcc = 0;

        while (std::iswdigit(*pStr))
        {
            iAcc = 10 * iAcc + (*pStr - L'0');
            pStr++;
        }

        if (iNeg != 0)
        {
            return -iAcc;
        }

        return iAcc;
    }

    __forceinline int64_t wtoi64(const wchar_t* pStr)
    {
        if (pStr == nullptr)
        {
            return 0;
        }

        while (*pStr == L' ' || *pStr == L'\t')
        {
            pStr++;
        }

        int iNeg = 0;

        if (*pStr == L'-')
        {
            iNeg = 1;
            pStr++;
        }
        else if (*pStr == L'+')
        {
            pStr++;
        }

        int64_t iAcc = 0;

        while (std::iswdigit(*pStr))
        {
            iAcc = 10 * iAcc + (int64_t)(*pStr - L'0');
            pStr++;
        }

        if (iNeg != 0)
        {
            return -iAcc;
        }

        return iAcc;
    }

    __forceinline wchar_t* itow(int iNum, wchar_t* pDst, int iBase)
    {
        if (iBase < 2 || iBase > 36)
        {
            pDst[0] = L'\0';
            return pDst;
        }

        int iNeg = 0;
        unsigned int uVal = 0;

        if (iNum < 0 && iBase == 10)
        {
            iNeg = 1;
            uVal = (unsigned int)(-iNum);
        }
        else
        {
            uVal = (unsigned int)iNum;
        }

        int iCount = 0;

        if (uVal == 0)
        {
            pDst[iCount] = L'0';
            iCount++;
        }

        while (uVal != 0)
        {
            unsigned int uDigit = uVal % (unsigned int)iBase;

            if (uDigit > 9)
            {
                pDst[iCount] = (wchar_t)(L'A' + (uDigit - 10));
            }
            else
            {
                pDst[iCount] = (wchar_t)(L'0' + uDigit);
            }

            iCount++;
            uVal /= (unsigned int)iBase;
        }

        if (iNeg != 0)
        {
            pDst[iCount] = L'-';
            iCount++;
        }

        pDst[iCount] = L'\0';

        int iI = 0;
        int iJ = iCount - 1;

        while (iI < iJ)
        {
            wchar_t cTmp = pDst[iI];
            pDst[iI] = pDst[iJ];
            pDst[iJ] = cTmp;
            iI++;
            iJ--;
        }

        return pDst;
    }
}
