#include "core/common.h"

extern "C" __attribute__((used)) int _fltused = 0;

extern "C" int abs(int iVal)
{
    if (iVal < 0)
    {
        return -iVal;
    }

    return iVal;
}
