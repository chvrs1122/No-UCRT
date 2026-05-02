#pragma once

#include "../core/common.h"
#include <utility>
#include <memory/heap/heap.h>

namespace std
{
    template<typename TSig>
    class function;

    template<typename TRet, typename... TArgs>
    class function<TRet(TArgs...)>
    {
    public:
        function() noexcept : pVtable(nullptr), pHeap(nullptr) {}

        function(decltype(nullptr)) noexcept : pVtable(nullptr), pHeap(nullptr) {}

        template<typename TF>
        function(TF tFn) : pVtable(nullptr), pHeap(nullptr)
        {
            Build(std::move(tFn));
        }

        function(const function& tOther) : pVtable(nullptr), pHeap(nullptr)
        {
            CopyFrom(tOther);
        }

        function(function&& tOther) noexcept : pVtable(nullptr), pHeap(nullptr)
        {
            MoveFrom(tOther);
        }

        ~function()
        {
            Reset();
        }

        function& operator=(const function& tOther)
        {
            if (this != &tOther)
            {
                Reset();
                CopyFrom(tOther);
            }
            return *this;
        }

        function& operator=(function&& tOther) noexcept
        {
            if (this != &tOther)
            {
                Reset();
                MoveFrom(tOther);
            }
            return *this;
        }

        function& operator=(decltype(nullptr)) noexcept
        {
            Reset();
            return *this;
        }

        template<typename TF>
        function& operator=(TF tFn)
        {
            Reset();
            Build(std::move(tFn));
            return *this;
        }

        explicit operator bool() const noexcept
        {
            return pVtable != nullptr;
        }

        TRet operator()(TArgs... tArgs) const
        {
            return pVtable->pInvoke(Slot(), std::forward<TArgs>(tArgs)...);
        }

    private:
        struct Vtable
        {
            TRet (*pInvoke)(void*, TArgs&&...);
            void (*pCopy)(void*, const void*);
            void (*pMove)(void*, void*);
            void (*pDestroy)(void*);
            size_t iSize;
            bool bSso;
        };

        static constexpr size_t kSso = 32;

        const Vtable* pVtable;
        void* pHeap;
        alignas(8) unsigned char tSsoBuf[kSso];

        void* Slot() const
        {
            if (pVtable == nullptr)
            {
                return nullptr;
            }
            if (pVtable->bSso)
            {
                return (void*)tSsoBuf;
            }
            return pHeap;
        }

        template<typename TF>
        static TRet InvokeImpl(void* pStore, TArgs&&... tArgs)
        {
            TF* pFn = (TF*)pStore;
            return (*pFn)(std::forward<TArgs>(tArgs)...);
        }

        template<typename TF>
        static void CopyImpl(void* pDst, const void* pSrc)
        {
            new (pDst) TF(*(const TF*)pSrc);
        }

        template<typename TF>
        static void MoveImpl(void* pDst, void* pSrc)
        {
            new (pDst) TF(std::move(*(TF*)pSrc));
        }

        template<typename TF>
        static void DestroyImpl(void* pStore)
        {
            ((TF*)pStore)->~TF();
        }

        template<typename TF>
        static const Vtable* VtableFor(bool bSso)
        {
            static const Vtable kVt = {
                &InvokeImpl<TF>,
                &CopyImpl<TF>,
                &MoveImpl<TF>,
                &DestroyImpl<TF>,
                sizeof(TF),
                false
            };
            static const Vtable kVtSso = {
                &InvokeImpl<TF>,
                &CopyImpl<TF>,
                &MoveImpl<TF>,
                &DestroyImpl<TF>,
                sizeof(TF),
                true
            };
            if (bSso)
            {
                return &kVtSso;
            }
            return &kVt;
        }

        template<typename TF>
        void Build(TF tFn)
        {
            constexpr bool bSso = (sizeof(TF) <= kSso) && (alignof(TF) <= 8);
            if constexpr (bSso)
            {
                new (tSsoBuf) TF(std::move(tFn));
                pVtable = VtableFor<TF>(true);
                pHeap = nullptr;
            }
            else
            {
                pHeap = Heap::Alloc(sizeof(TF));
                new (pHeap) TF(std::move(tFn));
                pVtable = VtableFor<TF>(false);
            }
        }

        void Reset() noexcept
        {
            if (pVtable == nullptr)
            {
                return;
            }
            void* pSlot = Slot();
            pVtable->pDestroy(pSlot);
            if (!pVtable->bSso && pHeap != nullptr)
            {
                Heap::Free(pHeap);
            }
            pHeap = nullptr;
            pVtable = nullptr;
        }

        void CopyFrom(const function& tOther)
        {
            if (tOther.pVtable == nullptr)
            {
                return;
            }
            if (tOther.pVtable->bSso)
            {
                tOther.pVtable->pCopy(tSsoBuf, tOther.tSsoBuf);
                pHeap = nullptr;
            }
            else
            {
                pHeap = Heap::Alloc(tOther.pVtable->iSize);
                tOther.pVtable->pCopy(pHeap, tOther.pHeap);
            }
            pVtable = tOther.pVtable;
        }

        void MoveFrom(function& tOther) noexcept
        {
            if (tOther.pVtable == nullptr)
            {
                return;
            }
            if (tOther.pVtable->bSso)
            {
                tOther.pVtable->pMove(tSsoBuf, tOther.tSsoBuf);
                tOther.pVtable->pDestroy(tOther.tSsoBuf);
                pHeap = nullptr;
            }
            else
            {
                pHeap = tOther.pHeap;
                tOther.pHeap = nullptr;
            }
            pVtable = tOther.pVtable;
            tOther.pVtable = nullptr;
        }
    };
}
