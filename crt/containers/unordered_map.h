#pragma once

#include "../core/common.h"
#include <utility>
#include "../functional/hash.h"
#include "../containers/vector.h"

namespace std
{
    // reference: https://medium.com/@a66k/c-unordered-map-under-the-hood-4e695177ed87
    template<typename TKey, typename TVal, typename THash = std::hash<TKey>>
    class unordered_map
    {
    public:
        struct pair
        {
            TKey first;
            TVal second;
        };

    private:
        struct Node
        {
            pair tPair;
            Node* pNext;
        };

        static const size_t DEFAULT_BUCKETS = 16;

        Node** pBuckets;
        size_t iBucketCount;
        size_t iCount;
        THash tHasher;

        size_t BucketIndex(const TKey& tKey) const
        {
            return tHasher(tKey) % iBucketCount;
        }

        void FreeAll()
        {
            if (pBuckets == nullptr)
            {
                return;
            }

            for (size_t i = 0; i < iBucketCount; i++)
            {
                Node* pCur = pBuckets[i];

                while (pCur != nullptr)
                {
                    Node* pNext = pCur->pNext;
                    pCur->~Node();
                    Heap::Free(pCur);
                    pCur = pNext;
                }

                pBuckets[i] = nullptr;
            }

            Heap::Free(pBuckets);
            pBuckets = nullptr;
            iCount = 0;
        }

        void AllocBuckets(size_t iN)
        {
            pBuckets = (Node**)Heap::Alloc(iN * sizeof(Node*));
            iBucketCount = iN;

            for (size_t i = 0; i < iN; i++)
            {
                pBuckets[i] = nullptr;
            }
        }

    public:
        unordered_map() : pBuckets(nullptr), iBucketCount(0), iCount(0)
        {
            AllocBuckets(DEFAULT_BUCKETS);
        }

        unordered_map(const unordered_map& tOther) : pBuckets(nullptr), iBucketCount(0), iCount(0)
        {
            AllocBuckets(tOther.iBucketCount);

            for (size_t i = 0; i < tOther.iBucketCount; i++)
            {
                Node* pCur = tOther.pBuckets[i];

                while (pCur != nullptr)
                {
                    insert(pCur->tPair.first, pCur->tPair.second);
                    pCur = pCur->pNext;
                }
            }
        }

        unordered_map(unordered_map&& tOther) noexcept
            : pBuckets(tOther.pBuckets), iBucketCount(tOther.iBucketCount), iCount(tOther.iCount)
        {
            tOther.pBuckets = nullptr;
            tOther.iBucketCount = 0;
            tOther.iCount = 0;
        }

        ~unordered_map()
        {
            FreeAll();
        }

        unordered_map& operator=(const unordered_map& tOther)
        {
            if (this == &tOther)
            {
                return *this;
            }

            FreeAll();
            AllocBuckets(tOther.iBucketCount);

            for (size_t i = 0; i < tOther.iBucketCount; i++)
            {
                Node* pCur = tOther.pBuckets[i];

                while (pCur != nullptr)
                {
                    insert(pCur->tPair.first, pCur->tPair.second);
                    pCur = pCur->pNext;
                }
            }

            return *this;
        }

        unordered_map& operator=(unordered_map&& tOther) noexcept
        {
            if (this == &tOther)
            {
                return *this;
            }

            FreeAll();
            pBuckets = tOther.pBuckets;
            iBucketCount = tOther.iBucketCount;
            iCount = tOther.iCount;
            tOther.pBuckets = nullptr;
            tOther.iBucketCount = 0;
            tOther.iCount = 0;
            return *this;
        }

        size_t size() const
        {
            return iCount;
        }

        bool empty() const
        {
            return iCount == 0;
        }

        void insert(const TKey& tKey, const TVal& tVal)
        {
            size_t iBucket = BucketIndex(tKey);
            Node* pCur = pBuckets[iBucket];

            while (pCur != nullptr)
            {
                if (pCur->tPair.first == tKey)
                {
                    pCur->tPair.second = tVal;
                    return;
                }

                pCur = pCur->pNext;
            }

            Node* pNew = (Node*)Heap::Alloc(sizeof(Node));
            new (pNew) Node{ { tKey, tVal }, pBuckets[iBucket] };
            pBuckets[iBucket] = pNew;
            iCount++;
        }

        TVal* find(const TKey& tKey)
        {
            size_t iBucket = BucketIndex(tKey);
            Node* pCur = pBuckets[iBucket];

            while (pCur != nullptr)
            {
                if (pCur->tPair.first == tKey)
                {
                    return &pCur->tPair.second;
                }

                pCur = pCur->pNext;
            }

            return nullptr;
        }

        const TVal* find(const TKey& tKey) const
        {
            size_t iBucket = BucketIndex(tKey);
            Node* pCur = pBuckets[iBucket];

            while (pCur != nullptr)
            {
                if (pCur->tPair.first == tKey)
                {
                    return &pCur->tPair.second;
                }

                pCur = pCur->pNext;
            }

            return nullptr;
        }

        bool contains(const TKey& tKey) const
        {
            return find(tKey) != nullptr;
        }

        bool erase(const TKey& tKey)
        {
            size_t iBucket = BucketIndex(tKey);
            Node* pPrev = nullptr;
            Node* pCur = pBuckets[iBucket];

            while (pCur != nullptr)
            {
                if (pCur->tPair.first == tKey)
                {
                    if (pPrev != nullptr)
                    {
                        pPrev->pNext = pCur->pNext;
                    }
                    else
                    {
                        pBuckets[iBucket] = pCur->pNext;
                    }

                    pCur->~Node();
                    Heap::Free(pCur);
                    iCount--;
                    return true;
                }

                pPrev = pCur;
                pCur = pCur->pNext;
            }

            return false;
        }

        TVal& operator[](const TKey& tKey)
        {
            size_t iBucket = BucketIndex(tKey);
            Node* pCur = pBuckets[iBucket];

            while (pCur != nullptr)
            {
                if (pCur->tPair.first == tKey)
                {
                    return pCur->tPair.second;
                }

                pCur = pCur->pNext;
            }

            Node* pNew = (Node*)Heap::Alloc(sizeof(Node));
            new (pNew) Node{ { tKey, TVal{} }, pBuckets[iBucket] };
            pBuckets[iBucket] = pNew;
            iCount++;
            return pBuckets[iBucket]->tPair.second;
        }
    };
}
