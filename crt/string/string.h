#pragma once

#include "../core/common.h"

namespace std
{
    __forceinline size_t strlen(const char* pStr)
    {
        const char* pCur = pStr;

        while (*pCur != '\0')
        {
            pCur++;
        }

        return (size_t)(pCur - pStr);
    }

    __forceinline int strcmp(const char* pStr1, const char* pStr2)
    {
        while (*pStr1 != '\0' && *pStr1 == *pStr2)
        {
            pStr1++;
            pStr2++;
        }

        return (int)(uint8_t)*pStr1 - (int)(uint8_t)*pStr2;
    }

    __forceinline char* strcpy(char* pDst, const char* pSrc)
    {
        char* pOut = pDst;

        while (*pSrc != '\0')
        {
            *pOut = *pSrc;
            pOut++;
            pSrc++;
        }

        *pOut = '\0';
        return pDst;
    }

    __forceinline char* strcat(char* pDst, const char* pSrc)
    {
        char* pOut = pDst;

        while (*pOut != '\0')
        {
            pOut++;
        }

        while (*pSrc != '\0')
        {
            *pOut = *pSrc;
            pOut++;
            pSrc++;
        }

        *pOut = '\0';
        return pDst;
    }

    __forceinline int toupper(int iC)
    {
        if (iC >= 'a' && iC <= 'z')
        {
            return iC - 'a' + 'A';
        }

        return iC;
    }

    __forceinline int tolower(int iC)
    {
        if (iC >= 'A' && iC <= 'Z')
        {
            return iC - 'A' + 'a';
        }

        return iC;
    }

    __forceinline bool isdigit(char cC)
    {
        return cC >= '0' && cC <= '9';
    }

    __forceinline int stricmp(const char* pStr1, const char* pStr2)
    {
        if (pStr1 == nullptr || pStr2 == nullptr)
        {
            return -1;
        }

        while (*pStr1 != '\0' && *pStr2 != '\0')
        {
            int iA = std::tolower((uint8_t)*pStr1);
            int iB = std::tolower((uint8_t)*pStr2);

            if (iA != iB)
            {
                return iA - iB;
            }

            pStr1++;
            pStr2++;
        }

        return std::tolower((uint8_t)*pStr1) - std::tolower((uint8_t)*pStr2);
    }

    __forceinline int strnicmp(const char* pStr1, const char* pStr2, size_t iN)
    {
        if (iN == 0)
        {
            return 0;
        }

        while (iN > 0)
        {
            int iA = std::tolower((uint8_t)*pStr1);
            int iB = std::tolower((uint8_t)*pStr2);

            if (iA != iB)
            {
                return iA - iB;
            }

            if (*pStr1 == '\0')
            {
                return 0;
            }

            pStr1++;
            pStr2++;
            iN--;
        }

        return 0;
    }

    __forceinline char* strchr(const char* pStr, char cC)
    {
        while (*pStr != '\0')
        {
            if (*pStr == cC)
            {
                return (char*)pStr;
            }

            pStr++;
        }

        if (cC == '\0')
        {
            return (char*)pStr;
        }

        return nullptr;
    }

    __forceinline char* strstr(const char* pStr, const char* pFind)
    {
        if (pStr == nullptr || pFind == nullptr)
        {
            return nullptr;
        }

        if (*pFind == '\0')
        {
            return (char*)pStr;
        }

        while (*pStr != '\0')
        {
            const char* pA = pStr;
            const char* pB = pFind;

            while (*pA != '\0' && *pB != '\0' && *pA == *pB)
            {
                pA++;
                pB++;
            }

            if (*pB == '\0')
            {
                return (char*)pStr;
            }

            pStr++;
        }

        return nullptr;
    }

    __forceinline char* strrev(char* pStr)
    {
        char* pBeg = pStr;
        char* pEnd = pStr;

        while (*pEnd != '\0')
        {
            pEnd++;
        }

        if (pEnd != pBeg)
        {
            pEnd--;
        }

        while (pBeg < pEnd)
        {
            char cTmp = *pBeg;
            *pBeg = *pEnd;
            *pEnd = cTmp;
            pBeg++;
            pEnd--;
        }

        return pStr;
    }

    __forceinline char* strlwr(char* pStr)
    {
        char* pCur = pStr;

        while (*pCur != '\0')
        {
            *pCur = (char)std::tolower((uint8_t)*pCur);
            pCur++;
        }

        return pStr;
    }

    __forceinline int atoi(const char* pStr)
    {
        if (pStr == nullptr)
        {
            return 0;
        }

        while (*pStr == ' ' || *pStr == '\t')
        {
            pStr++;
        }

        int iNeg = 0;

        if (*pStr == '-')
        {
            iNeg = 1;
            pStr++;
        }
        else if (*pStr == '+')
        {
            pStr++;
        }

        int iAcc = 0;

        while (std::isdigit(*pStr))
        {
            iAcc = 10 * iAcc + (*pStr - '0');
            pStr++;
        }

        if (iNeg != 0)
        {
            return -iAcc;
        }

        return iAcc;
    }

    __forceinline int64_t atoi64(const char* pStr)
    {
        if (pStr == nullptr)
        {
            return 0;
        }

        while (*pStr == ' ' || *pStr == '\t')
        {
            pStr++;
        }

        int iNeg = 0;

        if (*pStr == '-')
        {
            iNeg = 1;
            pStr++;
        }
        else if (*pStr == '+')
        {
            pStr++;
        }

        int64_t iAcc = 0;

        while (std::isdigit(*pStr))
        {
            iAcc = 10 * iAcc + (int64_t)(*pStr - '0');
            pStr++;
        }

        if (iNeg != 0)
        {
            return -iAcc;
        }

        return iAcc;
    }

    __forceinline char* itoa(int iNum, char* pDst, int iBase)
    {
        if (iBase < 2 || iBase > 36)
        {
            pDst[0] = '\0';
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
            pDst[iCount] = '0';
            iCount++;
        }

        while (uVal != 0)
        {
            unsigned int uDigit = uVal % (unsigned int)iBase;

            if (uDigit > 9)
            {
                pDst[iCount] = (char)('A' + (uDigit - 10));
            }
            else
            {
                pDst[iCount] = (char)('0' + uDigit);
            }

            iCount++;
            uVal /= (unsigned int)iBase;
        }

        if (iNeg != 0)
        {
            pDst[iCount] = '-';
            iCount++;
        }

        pDst[iCount] = '\0';

        int iI = 0;
        int iJ = iCount - 1;

        while (iI < iJ)
        {
            char cTmp = pDst[iI];
            pDst[iI] = pDst[iJ];
            pDst[iJ] = cTmp;
            iI++;
            iJ--;
        }

        return pDst;
    }
}
