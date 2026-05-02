#pragma once

struct PrintBuf
{
    char* pBuffer;
    char* pCurrent;
    unsigned int uLen;
};

extern "C" int printf(const char* pFmt, ...);

namespace std
{
    // std:c++latest pulls in MSVC headers this is why we need to put this in our own header to avoid conflicts
    using ::printf;
}