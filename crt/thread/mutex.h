#pragma once

#include "../core/common.h"
#include "chrono.h"
#include "this_thread.h"
#include <intrin.h>

namespace std
{
	class mutex
	{
	public:
		typedef volatile long* native_handle_type;

		mutex() noexcept : iState(0) {}
		~mutex() = default;

		mutex(const mutex&) = delete;
		mutex& operator=(const mutex&) = delete;
		mutex(mutex&&) = delete;
		mutex& operator=(mutex&&) = delete;

		// reference for behavior: https://en.cppreference.com/w/cpp/thread/mutex
		void lock()
		{
			int iBackoff = 0;

			while (!try_lock())
			{
				while (iState != 0)
				{
					if (iBackoff < 32)
					{
						_mm_pause();
					}
					else if (iBackoff < 96)
					{
						this_thread::yield();
					}
					else
					{
						this_thread::sleep_for(chrono::microseconds(50));
					}

					if (iBackoff < 128)
					{
						iBackoff = iBackoff + 1;
					}
				}
			}
		}

		bool try_lock()
		{
			long iPrev = _InterlockedCompareExchange(&iState, 1, 0);

			if (iPrev == 0)
			{
				return true;
			}

			return false;
		}

		void unlock()
		{
			_InterlockedExchange(&iState, 0);
		}

		native_handle_type native_handle()
		{
			return &iState;
		}

	private:
		volatile long iState;
	};

	template<typename TMutex>
	class lock_guard
	{
	public:
		typedef TMutex mutex_type;

		explicit lock_guard(mutex_type& tMutex) : tMutexRef(tMutex)
		{
			tMutexRef.lock();
		}

		~lock_guard()
		{
			tMutexRef.unlock();
		}

		lock_guard(const lock_guard&) = delete;
		lock_guard& operator=(const lock_guard&) = delete;

	private:
		TMutex& tMutexRef;
	};

	// reference for behavior: https://en.cppreference.com/w/cpp/thread/call_once
	struct once_flag
	{
		once_flag() noexcept : iState(0) {}

		once_flag(const once_flag&) = delete;
		once_flag& operator=(const once_flag&) = delete;

		volatile long iState; // 0=not run, 1=running, 2=done
	};

	template<typename TCallable, typename... TArgs>
	void call_once(once_flag& tFlag, TCallable&& fn, TArgs&&... args)
	{
		if (tFlag.iState == 2)
		{
			return;
		}

		long iPrev = _InterlockedCompareExchange(&tFlag.iState, 1, 0);

		if (iPrev == 0)
		{
			fn(static_cast<TArgs&&>(args)...);
			_InterlockedExchange(&tFlag.iState, 2);
			return;
		}

		while (tFlag.iState != 2)
		{
			if (tFlag.iState == 1)
			{
				this_thread::yield();
			}
			else
			{
				this_thread::sleep_for(chrono::microseconds(50));
			}
		}
	}
}
