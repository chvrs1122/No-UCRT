#pragma once

#include "../core/common.h"
#include "../core/memory.h"
#include "string.h"
#include "wstring.h"
#include "string_view.h"
#include <utility>
#include "../functional/hash.h"
#include <memory/heap/heap.h>

namespace std
{
    template<typename TChar>
    class basic_string
    {
    public:
        typedef TChar value_type;
        typedef TChar* iterator;
        typedef const TChar* const_iterator;
        static constexpr size_t npos = (size_t)-1;
        static constexpr size_t kSso = 15;

        basic_string() noexcept : iSize(0), iCap(kSso)
        {
            tSso[0] = (TChar)0;
            bHeap = false;
        }

        basic_string(const TChar* pStr) : basic_string()
        {
            if (pStr != nullptr)
            {
                Assign(pStr, SLen(pStr));
            }
        }

        basic_string(const TChar* pStr, size_t iLen) : basic_string()
        {
            Assign(pStr, iLen);
        }

        basic_string(basic_string_view<TChar> tView) : basic_string()
        {
            Assign(tView.data(), tView.size());
        }

        basic_string(const basic_string& tOther) : basic_string()
        {
            Assign(tOther.data(), tOther.iSize);
        }

        basic_string(basic_string&& tOther) noexcept : iSize(0), iCap(kSso), bHeap(false)
        {
            tSso[0] = (TChar)0;
            MoveFrom(tOther);
        }

        ~basic_string()
        {
            Release();
        }

        basic_string& operator=(const basic_string& tOther)
        {
            if (this == &tOther)
            {
                return *this;
            }
            Release();
            iSize = 0;
            iCap = kSso;
            bHeap = false;
            tSso[0] = (TChar)0;
            Assign(tOther.data(), tOther.iSize);
            return *this;
        }

        basic_string& operator=(basic_string&& tOther) noexcept
        {
            if (this == &tOther)
            {
                return *this;
            }
            Release();
            iSize = 0;
            iCap = kSso;
            bHeap = false;
            tSso[0] = (TChar)0;
            MoveFrom(tOther);
            return *this;
        }

        basic_string& operator=(const TChar* pStr)
        {
            size_t iLen = 0;
            if (pStr != nullptr)
            {
                iLen = SLen(pStr);
            }
            Release();
            iSize = 0;
            iCap = kSso;
            bHeap = false;
            tSso[0] = (TChar)0;
            Assign(pStr, iLen);
            return *this;
        }

        size_t size() const noexcept
        {
            return iSize;
        }
        size_t length() const noexcept
        {
            return iSize;
        }
        size_t capacity() const noexcept
        {
            return iCap;
        }
        bool empty() const noexcept
        {
            return iSize == 0;
        }

        TChar* data() noexcept
        {
            if (bHeap)
            {
                return pHeap;
            }
            else
            {
                return tSso;
            }
        }
        const TChar* data() const noexcept
        {
            if (bHeap)
            {
                return pHeap;
            }
            else
            {
                return tSso;
            }
        }
        const TChar* c_str() const noexcept
        {
            if (bHeap)
            {
                return pHeap;
            }
            else
            {
                return tSso;
            }
        }

        TChar& operator[](size_t iIdx)
        {
            return data()[iIdx];
        }
        const TChar& operator[](size_t iIdx) const
        {
            return data()[iIdx];
        }

        TChar& front()
        {
            return data()[0];
        }
        const TChar& front() const
        {
            return data()[0];
        }
        TChar& back()
        {
            return data()[iSize - 1];
        }
        const TChar& back() const
        {
            return data()[iSize - 1];
        }

        TChar* begin()
        {
            return data();
        }
        TChar* end()
        {
            return data() + iSize;
        }
        const TChar* begin() const
        {
            return data();
        }
        const TChar* end() const
        {
            return data() + iSize;
        }

        operator basic_string_view<TChar>() const
        {
            return basic_string_view<TChar>(data(), iSize);
        }

        void clear()
        {
            iSize = 0;
            data()[0] = (TChar)0;
        }

        void reserve(size_t iNewCap)
        {
            if (iNewCap <= iCap)
            {
                return;
            }
            Grow(iNewCap);
        }

        void push_back(TChar tCh)
        {
            EnsureCap(iSize + 1);
            data()[iSize] = tCh;
            iSize++;
            data()[iSize] = (TChar)0;
        }

        void pop_back()
        {
            if (iSize == 0)
            {
                return;
            }
            iSize--;
            data()[iSize] = (TChar)0;
        }

        basic_string& append(const TChar* pStr, size_t iLen)
        {
            EnsureCap(iSize + iLen);
            TChar* pDst = data() + iSize;

            for (size_t i = 0; i < iLen; i++)
            {
                pDst[i] = pStr[i];
            }

            iSize += iLen;
            data()[iSize] = (TChar)0;
            return *this;
        }

        basic_string& append(const TChar* pStr)
        {
            return append(pStr, SLen(pStr));
        }

        basic_string& append(basic_string_view<TChar> tView)
        {
            return append(tView.data(), tView.size());
        }

        basic_string& append(const basic_string& tOther)
        {
            return append(tOther.data(), tOther.iSize);
        }

        basic_string& operator+=(const basic_string& tOther)
        {
            return append(tOther);
        }
        basic_string& operator+=(const TChar* pStr)
        {
            return append(pStr);
        }
        basic_string& operator+=(TChar tCh)
        {
            push_back(tCh);
            return *this;
        }

        size_t find(TChar tCh, size_t iOff = 0) const
        {
            const TChar* pD = data();

            for (size_t i = iOff; i < iSize; i++)
            {
                if (pD[i] == tCh)
                {
                    return i;
                }
            }

            return npos;
        }

        size_t find(basic_string_view<TChar> tNeedle, size_t iOff = 0) const
        {
            return basic_string_view<TChar>(data(), iSize).find(tNeedle, iOff);
        }

        bool contains(basic_string_view<TChar> tNeedle) const
        {
            return find(tNeedle) != npos;
        }

        bool starts_with(basic_string_view<TChar> tPrefix) const
        {
            return basic_string_view<TChar>(data(), iSize).starts_with(tPrefix);
        }

        bool ends_with(basic_string_view<TChar> tSuffix) const
        {
            return basic_string_view<TChar>(data(), iSize).ends_with(tSuffix);
        }

        basic_string substr(size_t iOff, size_t iLen = npos) const
        {
            size_t iMax = iSize - iOff;

            if (iLen > iMax)
            {
                iLen = iMax;
            }

            return basic_string(data() + iOff, iLen);
        }

        bool operator==(const basic_string& tOther) const
        {
            if (iSize != tOther.iSize)
            {
                return false;
            }

            const TChar* pA = data();
            const TChar* pB = tOther.data();

            for (size_t i = 0; i < iSize; i++)
            {
                if (pA[i] != pB[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const basic_string& tOther) const
        {
            return !(*this == tOther);
        }

        bool operator==(const TChar* pStr) const
        {
            size_t iLen = 0;

            if (pStr != nullptr)
            {
                iLen = SLen(pStr);
            }

            if (iLen != iSize)
            {
                return false;
            }

            const TChar* pA = data();
            for (size_t i = 0; i < iSize; i++)
            {
                if (pA[i] != pStr[i])
                {
                    return false;
                }
            }

            return true;
        }

    private:
        union
        {
            TChar tSso[kSso + 1];
            TChar* pHeap;
        };
        size_t iSize;
        size_t iCap;
        bool bHeap;

        static size_t SLen(const TChar* pStr)
        {
            const TChar* pCur = pStr;

            while (*pCur != (TChar)0)
            {
                pCur++;
            }

            return (size_t)(pCur - pStr);
        }

        void Release()
        {
            if (bHeap && pHeap != nullptr)
            {
                Heap::Free(pHeap);
            }

            bHeap = false;
        }

        void Assign(const TChar* pSrc, size_t iLen)
        {
            EnsureCap(iLen);
            TChar* pDst = data();

            for (size_t i = 0; i < iLen; i++)
            {
                pDst[i] = pSrc[i];
            }

            iSize = iLen;
            pDst[iSize] = (TChar)0;
        }

        void EnsureCap(size_t iNeeded)
        {
            if (iNeeded <= iCap)
            {
                return;
            }

            size_t iNew = iCap * 2;

            if (iNew < iNeeded)
            {
                iNew = iNeeded;
            }

            Grow(iNew);
        }

        void Grow(size_t iNewCap)
        {
            TChar* pNew = (TChar*)Heap::Alloc((iNewCap + 1) * sizeof(TChar));
            const TChar* pOld = data();

            for (size_t i = 0; i < iSize; i++)
            {
                pNew[i] = pOld[i];
            }

            pNew[iSize] = (TChar)0;
            if (bHeap && pHeap != nullptr)
            {
                Heap::Free(pHeap);
            }

            pHeap = pNew;
            iCap = iNewCap;
            bHeap = true;
        }

        void MoveFrom(basic_string& tOther)
        {
            if (tOther.bHeap)
            {
                pHeap = tOther.pHeap;
                iSize = tOther.iSize;
                iCap = tOther.iCap;
                bHeap = true;
                tOther.pHeap = nullptr;
                tOther.bHeap = false;
                tOther.iSize = 0;
                tOther.iCap = kSso;
                tOther.tSso[0] = (TChar)0;
            }
            else
            {
                for (size_t i = 0; i <= tOther.iSize; i++)
                {
                    tSso[i] = tOther.tSso[i];
                }
                
                iSize = tOther.iSize;
                iCap = kSso;
                bHeap = false;
                tOther.iSize = 0;
                tOther.tSso[0] = (TChar)0;
            }
        }
    };

    typedef basic_string<char> string;
    typedef basic_string<wchar_t> wstring;

    template<>
    struct hash<string>
    {
        size_t operator()(const string& tStr) const noexcept
        {
            return (size_t)fnv1a_bytes(tStr.data(), tStr.size());
        }
    };

    template<>
    struct hash<wstring>
    {
        size_t operator()(const wstring& tStr) const noexcept
        {
            return (size_t)fnv1a_bytes((const uint8_t*)tStr.data(), tStr.size() * sizeof(wchar_t));
        }
    };

    template<>
    struct hash<string_view>
    {
        size_t operator()(string_view tView) const noexcept
        {
            return (size_t)fnv1a_bytes(tView.data(), tView.size());
        }
    };

    template<>
    struct hash<wstring_view>
    {
        size_t operator()(wstring_view tView) const noexcept
        {
            return (size_t)fnv1a_bytes((const uint8_t*)tView.data(), tView.size() * sizeof(wchar_t));
        }
    };
}
