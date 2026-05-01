#pragma once

struct PrintBuf
{
    char* pBuffer;
    char* pCurrent;
    unsigned int uLen;
};

extern "C" int printf(const char* pFmt, ...);