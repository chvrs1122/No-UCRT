#pragma once

#include "../core/common.h"
#include "../core/memory.h"
#include "../core/utility.h"
#include <memory/heap/heap.h>

namespace std
{
    template<typename T>
    class vector
    {
    public:
        typedef T* iterator;
        typedef const T* const_iterator;

        vector() : pData(nullptr), iCount(0), iCap(0) {}

        vector(const vector& tOther) : pData(nullptr), iCount(0), iCap(0)
        {
            CopyFrom(tOther);
        }

        vector(vector&& tOther) noexcept : pData(tOther.pData), iCount(tOther.iCount), iCap(tOther.iCap)
        {
            tOther.pData = nullptr;
            tOther.iCount = 0;
            tOther.iCap = 0;
        }

        ~vector()
        {
            Clear();
            FreeBuffer();
        }

        vector& operator=(const vector& tOther)
        {
            if (this == &tOther)
            {
                return *this;
            }

            Clear();
            FreeBuffer();
            CopyFrom(tOther);
            return *this;
        }

        vector& operator=(vector&& tOther) noexcept
        {
            if (this == &tOther)
            {
                return *this;
            }

            Clear();
            FreeBuffer();
            pData = tOther.pData;
            iCount = tOther.iCount;
            iCap = tOther.iCap;
            tOther.pData = nullptr;
            tOther.iCount = 0;
            tOther.iCap = 0;
            return *this;
        }

        size_t size() const
        {
            return iCount;
        }
        size_t capacity() const
        {
            return iCap;
        }
        bool empty() const
        {
            return iCount == 0;
        }

        T* data()
        {
            return pData;
        }
        const T* data() const
        {
            return pData;
        }

        T& operator[](size_t iIdx)
        {
            return pData[iIdx];
        }
        const T& operator[](size_t iIdx) const
        {
            return pData[iIdx];
        }

        T& front()
        {
            return pData[0];
        }
        const T& front() const
        {
            return pData[0];
        }

        T& back()
        {
            return pData[iCount - 1];
        }
        const T& back() const
        {
            return pData[iCount - 1];
        }

        iterator begin()
        {
            return pData;
        }
        iterator end()
        {
            return pData + iCount;
        }
        const_iterator begin() const
        {
            return pData;
        }
        const_iterator end() const
        {
            return pData + iCount;
        }

        void reserve(size_t iNewCap)
        {
            if (iNewCap <= iCap)
            {
                return;
            }

            Grow(iNewCap);
        }

        void resize(size_t iNewSize)
        {
            if (iNewSize < iCount)
            {
                ShrinkTo(iNewSize);
                return;
            }

            if (iNewSize > iCap)
            {
                Grow(iNewSize);
            }

            ConstructRange(iCount, iNewSize);
            iCount = iNewSize;
        }

        void push_back(const T& tVal)
        {
            EnsureGrowth();
            new (pData + iCount) T(tVal);
            iCount++;
        }

        void push_back(T&& tVal)
        {
            EnsureGrowth();
            new (pData + iCount) T(std::move(tVal));

            iCount++;
        }

        template<typename... TArgs>
        T& emplace_back(TArgs&&... tArgs)
        {
            EnsureGrowth();
            new (pData + iCount) T(std::forward<TArgs>(tArgs)...);

            T& tRef = pData[iCount];
            iCount++;
            return tRef;
        }

        void pop_back()
        {
            if (iCount == 0)
            {
                return;
            }
            iCount--;
            pData[iCount].~T();
        }

        void clear()
        {
            Clear();
        }

        iterator erase(iterator itPos)
        {
            if (itPos < pData || itPos >= pData + iCount)
            {
                return end();
            }

            size_t iIdx = (size_t)(itPos - pData);
            ShiftLeftFrom(iIdx);

            iCount--;
            pData[iCount].~T();
            return pData + iIdx;
        }

    private:
        T* pData;
        size_t iCount;
        size_t iCap;

        void Clear()
        {
            for (size_t i = 0; i < iCount; i++)
            {
                pData[i].~T();
            }
            iCount = 0;
        }

        void FreeBuffer()
        {
            if (pData != nullptr)
            {
                Heap::Free(pData);
                pData = nullptr;
            }
            iCap = 0;
        }

        void EnsureGrowth()
        {
            if (iCount < iCap)
            {
                return;
            }
            size_t iNew = iCap * 2;
            if (iNew < 4)
            {
                iNew = 4;
            }
            Grow(iNew);
        }

        void Grow(size_t iNewCap)
        {
            T* pNew = (T*)Heap::Alloc(iNewCap * sizeof(T));
            for (size_t i = 0; i < iCount; i++)
            {
                new (pNew + i) T(std::move(pData[i]));
                pData[i].~T();
            }

            if (pData != nullptr)
            {
                Heap::Free(pData);
            }

            pData = pNew;
            iCap = iNewCap;
        }

        void ConstructRange(size_t iFrom, size_t iTo)
        {
            for (size_t i = iFrom; i < iTo; i++)
            {
                new (pData + i) T();
            }
        }

        void ShrinkTo(size_t iNewSize)
        {
            for (size_t i = iNewSize; i < iCount; i++)
            {
                pData[i].~T();
            }
            iCount = iNewSize;
        }

        void ShiftLeftFrom(size_t iIdx)
        {
            for (size_t i = iIdx; i + 1 < iCount; i++)
            {
                pData[i] = std::move(pData[i + 1]);
            }
        }

        void CopyFrom(const vector& tOther)
        {
            if (tOther.iCount == 0)
            {
                return;
            }

            Grow(tOther.iCount);
            
            for (size_t i = 0; i < tOther.iCount; i++)
            {
                new (pData + i) T(tOther.pData[i]);
            }

            iCount = tOther.iCount;
        }
    };
}
