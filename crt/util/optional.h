#pragma once

#include "../core/common.h"
#include <utility>

namespace std
{
    struct nullopt_t
    {
        struct tag_t {};
        constexpr explicit nullopt_t(tag_t) {}
    };

    inline constexpr nullopt_t nullopt{nullopt_t::tag_t{}};

    template<typename T>
    class optional
    {
    public:
        optional() noexcept : bHas(false) {}
        optional(nullopt_t) noexcept : bHas(false) {}

        optional(const T& tVal) : bHas(true)
        {
            new (&tStorage) T(tVal);
        }

        optional(T&& tVal) : bHas(true)
        {
            new (&tStorage) T(std::move(tVal));
        }

        optional(const optional& tOther) : bHas(false)
        {
            if (tOther.bHas)
            {
                new (&tStorage) T(*tOther.Ptr());
                bHas = true;
            }
        }

        optional(optional&& tOther) noexcept : bHas(false)
        {
            if (tOther.bHas)
            {
                new (&tStorage) T(std::move(*tOther.Ptr()));
                bHas = true;
            }
        }

        ~optional()
        {
            reset();
        }

        optional& operator=(nullopt_t) noexcept
        {
            reset();
            return *this;
        }

        optional& operator=(const optional& tOther)
        {
            if (this == &tOther)
            {
                return *this;
            }
            reset();
            if (tOther.bHas)
            {
                new (&tStorage) T(*tOther.Ptr());
                bHas = true;
            }
            return *this;
        }

        optional& operator=(optional&& tOther) noexcept
        {
            if (this == &tOther)
            {
                return *this;
            }
            reset();
            if (tOther.bHas)
            {
                new (&tStorage) T(std::move(*tOther.Ptr()));
                bHas = true;
            }
            return *this;
        }

        optional& operator=(const T& tVal)
        {
            reset();
            new (&tStorage) T(tVal);
            bHas = true;
            return *this;
        }

        optional& operator=(T&& tVal)
        {
            reset();
            new (&tStorage) T(std::move(tVal));
            bHas = true;
            return *this;
        }

        bool has_value() const noexcept
        {
            return bHas;
        }
        explicit operator bool() const noexcept
        {
            return bHas;
        }

        T& value() &
        {
            return *Ptr();
        }
        const T& value() const &
        {
            return *Ptr();
        }
        T&& value() &&
        {
            return std::move(*Ptr());
        }

        T& operator*() &
        {
            return *Ptr();
        }
        const T& operator*() const &
        {
            return *Ptr();
        }
        T&& operator*() &&
        {
            return std::move(*Ptr());
        }

        T* operator->()
        {
            return Ptr();
        }
        const T* operator->() const
        {
            return Ptr();
        }

        T value_or(T tFallback) const
        {
            if (bHas)
            {
                return *Ptr();
            }
            return tFallback;
        }

        void reset() noexcept
        {
            if (bHas)
            {
                Ptr()->~T();
                bHas = false;
            }
        }

        template<typename... TArgs>
        T& emplace(TArgs&&... tArgs)
        {
            reset();
            new (&tStorage) T(std::forward<TArgs>(tArgs)...);
            bHas = true;
            return *Ptr();
        }

    private:
        alignas(T) unsigned char tStorage[sizeof(T)];
        bool bHas;

        T* Ptr()
        {
            return reinterpret_cast<T*>(&tStorage);
        }
        const T* Ptr() const
        {
            return reinterpret_cast<const T*>(&tStorage);
        }
    };
}
