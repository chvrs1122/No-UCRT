#pragma once

#include "../core/common.h"
#include "string.h"
#include "wstring.h"

namespace std
{
    template<typename TChar>
    class basic_string_view
    {
    public:
        typedef TChar value_type;
        typedef const TChar* iterator;
        typedef const TChar* const_iterator;
        static constexpr size_t npos = (size_t)-1;

        constexpr basic_string_view() noexcept : pData(nullptr), iCount(0) {}
        constexpr basic_string_view(const TChar* pStr, size_t iN) noexcept : pData(pStr), iCount(iN) {}

        basic_string_view(const TChar* pStr) noexcept : pData(pStr), iCount(0)
        {
            if (pStr != nullptr)
            {
                iCount = SLen(pStr);
            }
        }

        constexpr const TChar* data() const noexcept
        {
            return pData;
        }
        constexpr size_t size() const noexcept
        {
            return iCount;
        }
        constexpr size_t length() const noexcept
        {
            return iCount;
        }
        constexpr bool empty() const noexcept
        {
            return iCount == 0;
        }

        constexpr const TChar& operator[](size_t iIdx) const
        {
            return pData[iIdx];
        }
        constexpr const TChar& front() const
        {
            return pData[0];
        }
        constexpr const TChar& back() const
        {
            return pData[iCount - 1];
        }

        constexpr const TChar* begin() const noexcept
        {
            return pData;
        }
        constexpr const TChar* end() const noexcept
        {
            return pData + iCount;
        }

        constexpr basic_string_view substr(size_t iOff, size_t iLen = npos) const
        {
            size_t iMax = iCount - iOff;
            if (iLen > iMax)
            {
                iLen = iMax;
            }
            return basic_string_view(pData + iOff, iLen);
        }

        bool starts_with(basic_string_view tPrefix) const
        {
            if (tPrefix.iCount > iCount)
            {
                return false;
            }
            for (size_t i = 0; i < tPrefix.iCount; i++)
            {
                if (pData[i] != tPrefix.pData[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool ends_with(basic_string_view tSuffix) const
        {
            if (tSuffix.iCount > iCount)
            {
                return false;
            }
            size_t iOff = iCount - tSuffix.iCount;
            for (size_t i = 0; i < tSuffix.iCount; i++)
            {
                if (pData[iOff + i] != tSuffix.pData[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool contains(basic_string_view tNeedle) const
        {
            return find(tNeedle) != npos;
        }

        size_t find(TChar tCh, size_t iOff = 0) const
        {
            for (size_t i = iOff; i < iCount; i++)
            {
                if (pData[i] == tCh)
                {
                    return i;
                }
            }
            return npos;
        }

        size_t find(basic_string_view tNeedle, size_t iOff = 0) const
        {
            if (tNeedle.iCount == 0)
            {
                return iOff;
            }
            if (tNeedle.iCount > iCount)
            {
                return npos;
            }
            size_t iEnd = iCount - tNeedle.iCount;
            for (size_t i = iOff; i <= iEnd; i++)
            {
                bool bMatch = true;
                for (size_t j = 0; j < tNeedle.iCount; j++)
                {
                    if (pData[i + j] != tNeedle.pData[j])
                    {
                        bMatch = false;
                        break;
                    }
                }
                if (bMatch)
                {
                    return i;
                }
            }
            return npos;
        }

        bool operator==(basic_string_view tOther) const
        {
            if (iCount != tOther.iCount)
            {
                return false;
            }
            for (size_t i = 0; i < iCount; i++)
            {
                if (pData[i] != tOther.pData[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(basic_string_view tOther) const
        {
            return !(*this == tOther);
        }

    private:
        const TChar* pData;
        size_t iCount;

        static size_t SLen(const TChar* pStr)
        {
            const TChar* pCur = pStr;
            while (*pCur != (TChar)0)
            {
                pCur++;
            }
            return (size_t)(pCur - pStr);
        }
    };

    typedef basic_string_view<char> string_view;
    typedef basic_string_view<wchar_t> wstring_view;
}
