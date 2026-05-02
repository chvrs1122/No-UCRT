#pragma once

#include "../core/common.h"
#include <utility>

namespace std
{
    inline constexpr size_t variant_npos = (size_t)-1;

    template<typename TFirst, typename... TRest>
    struct variant_max_size
    {
        static constexpr size_t kRest = variant_max_size<TRest...>::value;
        static constexpr size_t value = (sizeof(TFirst) > kRest) ? sizeof(TFirst) : kRest;
    };

    template<typename TFirst>
    struct variant_max_size<TFirst>
    {
        static constexpr size_t value = sizeof(TFirst);
    };

    template<typename TFirst, typename... TRest>
    struct variant_max_align
    {
        static constexpr size_t kRest = variant_max_align<TRest...>::value;
        static constexpr size_t value = (alignof(TFirst) > kRest) ? alignof(TFirst) : kRest;
    };

    template<typename TFirst>
    struct variant_max_align<TFirst>
    {
        static constexpr size_t value = alignof(TFirst);
    };

    template<size_t I, typename TWanted, typename TFirst, typename... TRest>
    struct variant_index_of
    {
        static constexpr size_t value = variant_index_of<I + 1, TWanted, TRest...>::value;
    };

    template<size_t I, typename TWanted, typename... TRest>
    struct variant_index_of<I, TWanted, TWanted, TRest...>
    {
        static constexpr size_t value = I;
    };

    template<size_t I, typename... TElems>
    struct variant_type_at;

    template<typename THead, typename... TTail>
    struct variant_type_at<0, THead, TTail...>
    {
        typedef THead type;
    };

    template<size_t I, typename THead, typename... TTail>
    struct variant_type_at<I, THead, TTail...>
    {
        typedef typename variant_type_at<I - 1, TTail...>::type type;
    };

    template<typename... TElems>
    class variant
    {
    public:
        variant() noexcept : iIdx(variant_npos) {}

        ~variant()
        {
            Destroy();
        }

        variant(const variant& tOther) : iIdx(variant_npos)
        {
            CopyFrom(tOther);
        }

        variant(variant&& tOther) noexcept : iIdx(variant_npos)
        {
            MoveFrom(tOther);
        }

        variant& operator=(const variant& tOther)
        {
            if (this != &tOther)
            {
                Destroy();
                CopyFrom(tOther);
            }
            return *this;
        }

        variant& operator=(variant&& tOther) noexcept
        {
            if (this != &tOther)
            {
                Destroy();
                MoveFrom(tOther);
            }
            return *this;
        }

        template<typename T>
        variant(T&& tVal) : iIdx(variant_npos)
        {
            typedef typename remove_reference<T>::type TBare;
            constexpr size_t kIdx = variant_index_of<0, TBare, TElems...>::value;
            new (&tStorage) TBare(std::forward<T>(tVal));
            iIdx = kIdx;
        }

        template<typename T>
        variant& operator=(T&& tVal)
        {
            typedef typename remove_reference<T>::type TBare;
            constexpr size_t kIdx = variant_index_of<0, TBare, TElems...>::value;
            Destroy();
            new (&tStorage) TBare(std::forward<T>(tVal));
            iIdx = kIdx;
            return *this;
        }

        size_t index() const noexcept
        {
            return iIdx;
        }
        bool valueless_by_exception() const noexcept
        {
            return iIdx == variant_npos;
        }

        template<typename T>
        bool holds_alternative() const noexcept
        {
            return iIdx == variant_index_of<0, T, TElems...>::value;
        }

        template<typename T>
        T& get()
        {
            return *reinterpret_cast<T*>(&tStorage);
        }

        template<typename T>
        const T& get() const
        {
            return *reinterpret_cast<const T*>(&tStorage);
        }

        template<size_t I>
        typename variant_type_at<I, TElems...>::type& get_index()
        {
            typedef typename variant_type_at<I, TElems...>::type TVal;
            return *reinterpret_cast<TVal*>(&tStorage);
        }

    private:
        static constexpr size_t kSize = variant_max_size<TElems...>::value;
        static constexpr size_t kAlign = variant_max_align<TElems...>::value;
        alignas(kAlign) unsigned char tStorage[kSize];
        size_t iIdx;

        template<size_t I>
        void DestroyAtIndex()
        {
            if constexpr (I < sizeof...(TElems))
            {
                if (iIdx == I)
                {
                    typedef typename variant_type_at<I, TElems...>::type TVal;
                    reinterpret_cast<TVal*>(&tStorage)->~TVal();
                    return;
                }
                DestroyAtIndex<I + 1>();
            }
        }

        template<size_t I>
        void CopyAtIndex(const variant& tOther)
        {
            if constexpr (I < sizeof...(TElems))
            {
                if (tOther.iIdx == I)
                {
                    typedef typename variant_type_at<I, TElems...>::type TVal;
                    new (&tStorage) TVal(*reinterpret_cast<const TVal*>(&tOther.tStorage));
                    iIdx = I;
                    return;
                }
                CopyAtIndex<I + 1>(tOther);
            }
        }

        template<size_t I>
        void MoveAtIndex(variant& tOther)
        {
            if constexpr (I < sizeof...(TElems))
            {
                if (tOther.iIdx == I)
                {
                    typedef typename variant_type_at<I, TElems...>::type TVal;
                    new (&tStorage) TVal(std::move(*reinterpret_cast<TVal*>(&tOther.tStorage)));
                    iIdx = I;
                    return;
                }
                MoveAtIndex<I + 1>(tOther);
            }
        }

        void Destroy()
        {
            if (iIdx == variant_npos)
            {
                return;
            }
            DestroyAtIndex<0>();
            iIdx = variant_npos;
        }

        void CopyFrom(const variant& tOther)
        {
            if (tOther.iIdx == variant_npos)
            {
                return;
            }
            CopyAtIndex<0>(tOther);
        }

        void MoveFrom(variant& tOther)
        {
            if (tOther.iIdx == variant_npos)
            {
                return;
            }
            MoveAtIndex<0>(tOther);
            tOther.Destroy();
        }
    };

    template<typename T, typename... TElems>
    __forceinline bool holds_alternative(const variant<TElems...>& tV) noexcept
    {
        return tV.template holds_alternative<T>();
    }

    template<typename T, typename... TElems>
    __forceinline T& get(variant<TElems...>& tV)
    {
        return tV.template get<T>();
    }

    template<typename T, typename... TElems>
    __forceinline const T& get(const variant<TElems...>& tV)
    {
        return tV.template get<T>();
    }
}
