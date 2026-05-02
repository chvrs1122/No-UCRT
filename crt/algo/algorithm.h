#pragma once

#include "../core/common.h"
#include <utility>

namespace std
{
    template<typename T>
    __forceinline const T& clamp(const T& tVal, const T& tLo, const T& tHi)
    {
        if (tVal < tLo)
        {
            return tLo;
        }

        if (tHi < tVal)
        {
            return tHi;
        }

        return tVal;
    }

    template<typename TIter>
    __forceinline void sort(TIter itBegin, TIter itEnd)
    {
        for (TIter itI = itBegin; itI != itEnd; ++itI)
        {
            for (TIter itJ = itI + 1; itJ != itEnd; ++itJ)
            {
                if (*itJ < *itI)
                {
                    auto tTmp = std::move(*itI);
                    *itI = std::move(*itJ);
                    *itJ = std::move(tTmp);
                }
            }
        }
    }

    template<typename TIter, typename TCmp>
    __forceinline void sort(TIter itBegin, TIter itEnd, TCmp tCmp)
    {
        for (TIter itI = itBegin; itI != itEnd; ++itI)
        {
            for (TIter itJ = itI + 1; itJ != itEnd; ++itJ)
            {
                if (tCmp(*itJ, *itI))
                {
                    auto tTmp = std::move(*itI);
                    *itI = std::move(*itJ);
                    *itJ = std::move(tTmp);
                }
            }
        }
    }

    template<typename TIter, typename T>
    __forceinline TIter find(TIter itBegin, TIter itEnd, const T& tVal)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (*itCur == tVal)
            {
                return itCur;
            }
        }

        return itEnd;
    }

    template<typename TIter, typename TPred>
    __forceinline TIter find_if(TIter itBegin, TIter itEnd, TPred tPred)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (tPred(*itCur))
            {
                return itCur;
            }
        }

        return itEnd;
    }

    template<typename TIter, typename TPred>
    __forceinline TIter find_if_not(TIter itBegin, TIter itEnd, TPred tPred)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (!tPred(*itCur))
            {
                return itCur;
            }
        }

        return itEnd;
    }

    template<typename TContainer, typename T>
    __forceinline bool contains(const TContainer& tC, const T& tVal)
    {
        for (auto& tItem : tC)
        {
            if (tItem == tVal)
            {
                return true;
            }
        }

        return false;
    }

    template<typename TIter, typename T>
    __forceinline size_t count(TIter itBegin, TIter itEnd, const T& tVal)
    {
        size_t iN = 0;

        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (*itCur == tVal)
            {
                iN++;
            }
        }

        return iN;
    }

    template<typename TIter, typename TPred>
    __forceinline size_t count_if(TIter itBegin, TIter itEnd, TPred tPred)
    {
        size_t iN = 0;

        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (tPred(*itCur))
            {
                iN++;
            }
        }

        return iN;
    }

    template<typename TIter, typename TPred>
    __forceinline bool any_of(TIter itBegin, TIter itEnd, TPred tPred)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (tPred(*itCur))
            {
                return true;
            }
        }
        return false;
    }

    template<typename TIter, typename TPred>
    __forceinline bool all_of(TIter itBegin, TIter itEnd, TPred tPred)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (!tPred(*itCur))
            {
                return false;
            }
        }
        return true;
    }

    template<typename TIter, typename TPred>
    __forceinline bool none_of(TIter itBegin, TIter itEnd, TPred tPred)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (tPred(*itCur))
            {
                return false;
            }
        }
        return true;
    }

    template<typename TIter, typename TFn>
    __forceinline TFn for_each(TIter itBegin, TIter itEnd, TFn tFn)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            tFn(*itCur);
        }
        return tFn;
    }

    template<typename TIter, typename TOut>
    __forceinline TOut copy(TIter itBegin, TIter itEnd, TOut itDst)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            *itDst = *itCur;
            ++itDst;
        }
        return itDst;
    }

    template<typename TIter, typename T>
    __forceinline void fill(TIter itBegin, TIter itEnd, const T& tVal)
    {
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            *itCur = tVal;
        }
    }

    template<typename TIter>
    __forceinline void reverse(TIter itBegin, TIter itEnd)
    {
        if (itBegin == itEnd)
        {
            return;
        }
        --itEnd;
        while (itBegin < itEnd)
        {
            std::swap(*itBegin, *itEnd);
            ++itBegin;
            --itEnd;
        }
    }

    template<typename TIter, typename T>
    __forceinline TIter lower_bound(TIter itBegin, TIter itEnd, const T& tVal)
    {
        size_t iCount = (size_t)(itEnd - itBegin);

        while (iCount > 0)
        {
            size_t iStep = iCount / 2;
            TIter itMid = itBegin + iStep;

            if (*itMid < tVal)
            {
                itBegin = itMid + 1;
                iCount -= iStep + 1;
            }
            else
            {
                iCount = iStep;
            }
        }

        return itBegin;
    }

    template<typename TIter, typename T>
    __forceinline bool binary_search(TIter itBegin, TIter itEnd, const T& tVal)
    {
        TIter it = std::lower_bound(itBegin, itEnd, tVal);

        if (it == itEnd)
        {
            return false;
        }
        if (tVal < *it)
        {
            return false;
        }

        return true;
    }

    template<typename TIter, typename TPred>
    __forceinline TIter remove_if(TIter itBegin, TIter itEnd, TPred tPred)
    {
        TIter itDst = itBegin;
        
        for (TIter itCur = itBegin; itCur != itEnd; ++itCur)
        {
            if (!tPred(*itCur))
            {
                if (itDst != itCur)
                {
                    *itDst = std::move(*itCur);
                }
                ++itDst;
            }
        }

        return itDst;
    }
}
