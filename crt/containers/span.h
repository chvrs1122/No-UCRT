#pragma once

#include "../core/common.h"
#include "array.h"

namespace std
{
    inline constexpr size_t dynamic_extent = (size_t)-1;

    template<typename T>
    class span
    {
    public:
        typedef T element_type;
        typedef T* iterator;
        typedef const T* const_iterator;

        constexpr span() noexcept : pData(nullptr), iCount(0) {}
        constexpr span(T* pPtr, size_t iN) noexcept : pData(pPtr), iCount(iN) {}

        template<size_t N>
        constexpr span(T (&tArr)[N]) noexcept : pData(tArr), iCount(N) {}

        template<size_t N>
        constexpr span(array<T, N>& tArr) noexcept : pData(tArr.data()), iCount(N) {}

        template<size_t N>
        constexpr span(const array<T, N>& tArr) noexcept : pData(tArr.data()), iCount(N) {}

        constexpr T* data() const noexcept
        {
            return pData;
        }
        constexpr size_t size() const noexcept
        {
            return iCount;
        }
        constexpr size_t size_bytes() const noexcept
        {
            return iCount * sizeof(T);
        }
        constexpr bool empty() const noexcept
        {
            return iCount == 0;
        }

        constexpr T& operator[](size_t iIdx) const
        {
            return pData[iIdx];
        }
        constexpr T& front() const
        {
            return pData[0];
        }
        constexpr T& back() const
        {
            return pData[iCount - 1];
        }

        constexpr T* begin() const noexcept
        {
            return pData;
        }
        constexpr T* end() const noexcept
        {
            return pData + iCount;
        }

        constexpr span<T> first(size_t iCountOut) const
        {
            return span<T>(pData, iCountOut);
        }

        constexpr span<T> last(size_t iCountOut) const
        {
            return span<T>(pData + (iCount - iCountOut), iCountOut);
        }

        constexpr span<T> subspan(size_t iOff, size_t iCountOut) const
        {
            return span<T>(pData + iOff, iCountOut);
        }

    private:
        T* pData;
        size_t iCount;
    };
}
