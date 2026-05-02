#pragma once

#include "../core/common.h"

// hidden one to avoid conflict with MSVC headers
#if defined(_CSTDLIB_) && !defined(__INTELLISENSE__)
__forceinline int abs(int iVal)
{
    if (iVal < 0)
    {
        return -iVal;
    }

    return iVal;
}
#endif

namespace std
{
    namespace detail
    {
        __forceinline double FAbs(double dVal)
        {
            if (dVal < 0.0)
            {
                return -dVal;
            }

            return dVal;
        }

        __forceinline double Exp(double dVal)
        {
            double dResult = 1.0;
            double dTerm = 1.0;

            for (int i = 1; i < 64; i++)
            {
                dTerm *= dVal / (double)i;
                dResult += dTerm;

                if (FAbs(dTerm) < 1e-17)
                {
                    break;
                }
            }

            return dResult;
        }

        __forceinline double Log(double dVal)
        {
            if (dVal <= 0.0)
            {
                return 0.0;
            }

            int iExp = 0;

            while (dVal >= 2.0)
            {
                dVal *= 0.5;
                iExp++;
            }

            while (dVal < 1.0)
            {
                dVal *= 2.0;
                iExp--;
            }

            double dY = (dVal - 1.0) / (dVal + 1.0);
            double dY2 = dY * dY;
            double dSum = 0.0;
            double dTerm = dY;

            for (int i = 0; i < 64; i++)
            {
                double dPart = dTerm / (double)(2 * i + 1);
                dSum += dPart;
                dTerm *= dY2;

                if (FAbs(dPart) < 1e-17)
                {
                    break;
                }
            }

            return 2.0 * dSum + (double)iExp * 0.69314718055994530942;
        }

        __forceinline double NormalizeAngle(double dVal)
        {
            const double dPi = 3.14159265358979323846;
            const double dTwoPi = 6.28318530717958647692;

            while (dVal > dPi)
            {
                dVal -= dTwoPi;
            }

            while (dVal < -dPi)
            {
                dVal += dTwoPi;
            }

            return dVal;
        }

        __forceinline double Atan(double dVal)
        {
            const double dHalfPi = 1.57079632679489661923;

            bool bInverted = false;
            bool bNegative = false;

            if (dVal < 0.0)
            {
                dVal = -dVal;
                bNegative = true;
            }

            if (dVal > 1.0)
            {
                dVal = 1.0 / dVal;
                bInverted = true;
            }

            const double dSqrt3 = 1.73205080756887729353;
            const double dPiOver6 = 0.52359877559829887308;
            const double dTanPiOver12 = 0.26794919243112270647;
            double dReduce = 0.0;

            if (dVal > dTanPiOver12)
            {
                dReduce = dPiOver6;
                dVal = (dVal * dSqrt3 - 1.0) / (dVal + dSqrt3);
            }

            double dResult = 0.0;
            double dTerm = dVal;
            double dX2 = dVal * dVal;

            for (int i = 0; i < 256; i++)
            {
                double dPart = dTerm / (double)(2 * i + 1);

                if ((i & 1) == 0)
                {
                    dResult += dPart;
                }
                else
                {
                    dResult -= dPart;
                }

                dTerm *= dX2;

                if (FAbs(dPart) < 1e-17)
                {
                    break;
                }
            }

            dResult += dReduce;

            if (bInverted)
            {
                dResult = dHalfPi - dResult;
            }

            if (bNegative)
            {
                dResult = -dResult;
            }

            return dResult;
        }
    }

    __forceinline double sqrt(double dVal)
    {
        if (dVal <= 0.0)
        {
            return 0.0;
        }

        double dGuess = dVal;

        for (int i = 0; i < 32; i++)
        {
            double dNext = 0.5 * (dGuess + dVal / dGuess);

            if (detail::FAbs(dNext - dGuess) < 1e-15 * dGuess)
            {
                return dNext;
            }

            dGuess = dNext;
        }

        return dGuess;
    }

    __forceinline double pow(double dBase, double dExp)
    {
        if (dExp == 0.0)
        {
            return 1.0;
        }

        if (dBase == 0.0)
        {
            return 0.0;
        }

        if (dBase < 0.0)
        {
            int iIntExp = (int)dExp;

            if ((double)iIntExp == dExp)
            {
                double dResult = 1.0;
                double dAbs = -dBase;
                int iN = iIntExp;

                if (iN < 0)
                {
                    iN = -iN;
                }

                for (int i = 0; i < iN; i++)
                {
                    dResult *= dAbs;
                }

                if (iIntExp < 0)
                {
                    dResult = 1.0 / dResult;
                }

                if ((iIntExp & 1) != 0)
                {
                    dResult = -dResult;
                }

                return dResult;
            }

            return 0.0;
        }

        return detail::Exp(dExp * detail::Log(dBase));
    }

    __forceinline double sin(double dVal)
    {
        dVal = detail::NormalizeAngle(dVal);

        double dTerm = dVal;
        double dX2 = dVal * dVal;
        double dResult = dTerm;

        for (int i = 1; i < 32; i++)
        {
            double dDen = (double)((2 * i) * (2 * i + 1));
            dTerm *= -dX2 / dDen;
            dResult += dTerm;

            if (detail::FAbs(dTerm) < 1e-17)
            {
                break;
            }
        }

        return dResult;
    }

    __forceinline double cos(double dVal)
    {
        dVal = detail::NormalizeAngle(dVal);

        double dResult = 1.0;
        double dTerm = 1.0;
        double dX2 = dVal * dVal;

        for (int i = 1; i < 32; i++)
        {
            double dDen = (double)((2 * i - 1) * (2 * i));
            dTerm *= -dX2 / dDen;
            dResult += dTerm;

            if (detail::FAbs(dTerm) < 1e-17)
            {
                break;
            }
        }

        return dResult;
    }

    __forceinline double tan(double dVal)
    {
        double dC = std::cos(dVal);

        if (dC == 0.0)
        {
            return 0.0;
        }

        return std::sin(dVal) / dC;
    }

    __forceinline float tan(float fVal)
    {
        return (float)std::tan((double)fVal);
    }

    __forceinline double atan2(double dY, double dX)
    {
        const double dPi = 3.14159265358979323846;
        const double dHalfPi = 1.57079632679489661923;

        if (dX > 0.0)
        {
            return detail::Atan(dY / dX);
        }

        if (dX < 0.0)
        {
            if (dY >= 0.0)
            {
                return detail::Atan(dY / dX) + dPi;
            }

            return detail::Atan(dY / dX) - dPi;
        }

        if (dY > 0.0)
        {
            return dHalfPi;
        }

        if (dY < 0.0)
        {
            return -dHalfPi;
        }

        return 0.0;
    }

    __forceinline float atan2(float fY, float fX)
    {
        return (float)std::atan2((double)fY, (double)fX);
    }

// same here msvc conflicts with cstdlib
#ifndef _CSTDLIB_
    __forceinline int abs(int iVal)
    {
        if (iVal < 0)
        {
            return -iVal;
        }

        return iVal;
    }
#else
    using ::abs;
#endif

    __forceinline bool isfinite(double dVal)
    {
        if (dVal != dVal)
        {
            return false;
        }

        union
        {
            double d;
            uint64_t u;
        } uPun;

        uPun.d = dVal;
        uint64_t uExp = (uPun.u >> 52) & 0x7FF;

        if (uExp == 0x7FF)
        {
            return false;
        }

        return true;
    }
}
