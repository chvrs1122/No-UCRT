/*
 * Based on mini-printf by Michal Ludvig
 * https://github.com/mludvig/mini-printf
 *
 * Copyright (c) 2013,2014 Michal Ludvig <michal@logix.cz>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the author nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Extended with: %p, %f/%F, %z/%ll length modifiers, precision, float support.
 * Output via WriteConsoleA.
 */

// please do not use this in public code, it's not a full implementation and may have security issues 
#include "../core/common.h"
#include <stdarg.h>
#include "printf.h"

extern "C" __declspec(dllimport) void* __stdcall GetStdHandle(unsigned long nStdHandle);
extern "C" __declspec(dllimport) int __stdcall WriteConsoleA(void* hConsoleOutput, const void* lpBuffer, unsigned long nNumberOfCharsToWrite, unsigned long* lpNumberOfCharsWritten, void* lpReserved);

static int BufPuts(char* pStr, int iLen, void* pCtx)
{
    if (pCtx == nullptr)
    {
        return iLen;
    }

    PrintBuf* pB = (PrintBuf*)pCtx;
    int iWritten = 0;

    for (int i = 0; i < iLen; i++)
    {
        if (pB->pCurrent >= pB->pBuffer + pB->uLen - 1)
        {
            break;
        }
        *(pB->pCurrent++) = pStr[i];
        iWritten++;
    }

    *(pB->pCurrent) = '\0';
    return iWritten;
}

static int ItoA(long long iVal, unsigned int uRadix, int bUpper, int bUnsigned, char* pBuf)
{
    char* pCur = pBuf;
    int bNeg = 0;
    unsigned long long uVal;

    if (uRadix > 16)
    {
        return 0;
    }

    if (!bUnsigned && iVal < 0)
    {
        bNeg = 1;
        uVal = (unsigned long long)(-(iVal + 1)) + 1ULL;
    }
    else
    {
        uVal = (unsigned long long)iVal;
    }

    do
    {
        int iDigit = (int)(uVal % uRadix);

        if (iDigit < 10)
        {
            *(pCur++) = (char)('0' + iDigit);
        }
        else if (bUpper)
        {
            *(pCur++) = (char)('A' + iDigit - 10);
        }
        else
        {
            *(pCur++) = (char)('a' + iDigit - 10);
        }

        uVal /= uRadix;
    } while (uVal > 0);

    if (bNeg)
    {
        *(pCur++) = '-';
    }

    *pCur = '\0';
    int iLen = (int)(pCur - pBuf);

    for (int i = 0; i < iLen / 2; i++)
    {
        char cTmp = pBuf[i];
        pBuf[i] = pBuf[iLen - i - 1];
        pBuf[iLen - i - 1] = cTmp;
    }

    return iLen;
}

static int PadCopy(char* pPtr, int iLen, char cPadCh, int iPadTo, char* pOut)
{
    char* pCur = pOut;

    if (iPadTo == 0)
    {
        iPadTo = iLen;
    }

    if (iLen > iPadTo)
    {
        iLen = iPadTo;
    }

    for (int i = iPadTo - iLen; i > 0; i--)
    {
        *(pCur++) = cPadCh;
    }

    for (int i = iLen; i > 0; i--)
    {
        *(pCur++) = *(pPtr++);
    }

    return (int)(pCur - pOut);
}

static int SLen(const char* pStr)
{
    int iLen = 0;
    while (pStr[iLen] != '\0')
    {
        iLen++;
    }
    return iLen;
}

static int DtoA(double dVal, int iPrec, char* pBuf)
{
    char* pCur = pBuf;

    if (iPrec < 0)
    {
        iPrec = 6;
    }

    if (dVal < 0.0)
    {
        *(pCur++) = '-';
        dVal = -dVal;
    }

    unsigned long long uInt = (unsigned long long)dVal;
    double dFrac = dVal - (double)uInt;

    char szInt[24];
    int iIntLen = ItoA((long long)uInt, 10, 0, 1, szInt);

    for (int i = 0; i < iIntLen; i++)
    {
        *(pCur++) = szInt[i];
    }

    if (iPrec > 0)
    {
        *(pCur++) = '.';

        for (int i = 0; i < iPrec; i++)
        {
            dFrac *= 10.0;
            int iDigit = (int)dFrac;
            dFrac -= (double)iDigit;
            *(pCur++) = (char)('0' + iDigit);
        }
    }

    *pCur = '\0';
    return (int)(pCur - pBuf);
}

static int VPrintf(int (*pfnPuts)(char* pS, int iLen, void* pCtx), void* pCtx, const char* pFmt, va_list args)
{
    char szTmp[32];
    char szVal[32];
    int iTotal = 0;

    while (*pFmt != '\0')
    {
        if (*pFmt != '%')
        {
            iTotal += pfnPuts((char*)pFmt, 1, pCtx);
            pFmt++;
            continue;
        }

        pFmt++;

        char cPad = ' ';
        int iPad = 0;
        int bLong = 0;
        int iPrec = -1;

        if (*pFmt == '0')
        {
            cPad = '0';
        }

        while (*pFmt >= '0' && *pFmt <= '9')
        {
            iPad = iPad * 10 + (*pFmt - '0');
            pFmt++;
        }

        if (iPad > (int)sizeof(szTmp))
        {
            iPad = (int)sizeof(szTmp);
        }

        if (*pFmt == '.')
        {
            pFmt++;
            iPrec = 0;

            while (*pFmt >= '0' && *pFmt <= '9')
            {
                iPrec = iPrec * 10 + (*pFmt - '0');
                pFmt++;
            }
        }

        if (*pFmt == 'l')
        {
            bLong = 1;
            pFmt++;

            if (*pFmt == 'l')
            {
                pFmt++;
            }
        }
        else if (*pFmt == 'z')
        {
            bLong = 1;
            pFmt++;
        }

        char cSpec = *pFmt;

        if (cSpec == '\0')
        {
            break;
        }

        int iLen = 0;

        if (cSpec == 'd' || cSpec == 'i' || cSpec == 'u')
        {
            int bUnsigned = (cSpec == 'u') ? 1 : 0;
            long long iVal = 0;

            if (bLong)
            {
                iVal = (long long)va_arg(args, long long);
            }
            else if (bUnsigned)
            {
                iVal = (long long)(unsigned long long)va_arg(args, unsigned int);
            }
            else
            {
                iVal = (long long)va_arg(args, int);
            }

            iLen = ItoA(iVal, 10, 0, bUnsigned, szVal);
            iLen = PadCopy(szVal, iLen, cPad, iPad, szTmp);
            iTotal += pfnPuts(szTmp, iLen, pCtx);
        }
        else if (cSpec == 'x' || cSpec == 'X')
        {
            unsigned long long uVal = 0;

            if (bLong)
            {
                uVal = va_arg(args, unsigned long long);
            }
            else
            {
                uVal = (unsigned long long)va_arg(args, unsigned int);
            }

            iLen = ItoA((long long)uVal, 16, (cSpec == 'X') ? 1 : 0, 1, szVal);
            iLen = PadCopy(szVal, iLen, cPad, iPad, szTmp);
            iTotal += pfnPuts(szTmp, iLen, pCtx);
        }
        else if (cSpec == 'p')
        {
            unsigned long long uVal = (unsigned long long)va_arg(args, void*);
            char szHex[20];
            szHex[0] = '0';
            szHex[1] = 'x';
            iLen = ItoA((long long)uVal, 16, 0, 1, szHex + 2);
            iTotal += pfnPuts(szHex, 2 + iLen, pCtx);
        }
        else if (cSpec == 's')
        {
            char* pStr = va_arg(args, char*);

            if (pStr == nullptr)
            {
                pStr = (char*)"(null)";
            }

            iLen = SLen(pStr);

            if (iPad > 0)
            {
                iLen = PadCopy(pStr, iLen, cPad, iPad, szTmp);
                iTotal += pfnPuts(szTmp, iLen, pCtx);
            }
            else
            {
                iTotal += pfnPuts(pStr, iLen, pCtx);
            }
        }
        else if (cSpec == 'c')
        {
            char cCh = (char)va_arg(args, int);
            iLen = PadCopy(&cCh, 1, cPad, iPad, szTmp);
            iTotal += pfnPuts(szTmp, iLen, pCtx);
        }
        else if (cSpec == 'f' || cSpec == 'F')
        {
            double dVal = va_arg(args, double);
            iLen = DtoA(dVal, iPrec, szVal);
            iTotal += pfnPuts(szVal, iLen, pCtx);
        }
        else if (cSpec == '%')
        {
            char cPct = '%';
            iTotal += pfnPuts(&cPct, 1, pCtx);
        }
        else
        {
            iTotal += pfnPuts((char*)pFmt, 1, pCtx);
        }

        pFmt++;
    }

    return iTotal;
}

extern "C" int printf(const char* pFmt, ...)
{
    char szBuf[1024];
    PrintBuf tBuf;
    tBuf.pBuffer = szBuf;
    tBuf.pCurrent = szBuf;
    tBuf.uLen = sizeof(szBuf);
    szBuf[0] = '\0';

    va_list args;
    va_start(args, pFmt);
    int iRet = VPrintf(BufPuts, &tBuf, pFmt, args);
    va_end(args);

    void* hOut = GetStdHandle((unsigned long)-11);
    unsigned long uWritten = 0;
    WriteConsoleA(hOut, szBuf, (unsigned long)(tBuf.pCurrent - szBuf), &uWritten, nullptr);

    return iRet;
}
