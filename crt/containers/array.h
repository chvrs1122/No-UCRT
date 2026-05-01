#pragma once

#include "../core/common.h"

namespace std
{
    template<typename T, size_t N>
    struct array
    {
        T tData[N];

        constexpr size_t size() const
        {
            return N;
        }
        constexpr bool empty() const
        {
            return N == 0;
        }

        T& operator[](size_t iIdx)
        {
            return tData[iIdx];
        }
        const T& operator[](size_t iIdx) const
        {
            return tData[iIdx];
        }

        T& front()
        {
            return tData[0];
        }
        const T& front() const
        {
            return tData[0];
        }

        T& back()
        {
            return tData[N - 1];
        }
        const T& back() const
        {
            return tData[N - 1];
        }

        T* data()
        {
            return tData;
        }
        const T* data() const
        {
            return tData;
        }

        T* begin()
        {
            return tData;
        }
        T* end()
        {
            return tData + N;
        }
        const T* begin() const
        {
            return tData;
        }
        const T* end() const
        {
            return tData + N;
        }

        void fill(const T& tVal)
        {
            for (size_t i = 0; i < N; i++)
            {
                tData[i] = tVal;
            }
        }
    };

    template<typename T>
    struct array<T, 0>
    {
        constexpr size_t size() const
        {
            return 0;
        }
        constexpr bool empty() const
        {
            return true;
        }

        T* data()
        {
            return nullptr;
        }
        const T* data() const
        {
            return nullptr;
        }

        T* begin()
        {
            return nullptr;
        }
        T* end()
        {
            return nullptr;
        }
        const T* begin() const
        {
            return nullptr;
        }
        const T* end() const
        {
            return nullptr;
        }
    };

    template<typename T, size_t N>
    struct tuple_size_impl
    {
    };
}
