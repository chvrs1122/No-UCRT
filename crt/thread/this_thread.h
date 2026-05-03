#pragma once

#include "../core/common.h"
#include "chrono.h"
#include "../../syscall/syscall.h"

namespace std
{
	namespace this_thread
	{
		inline void yield()
		{
			Syscall::YieldExecution();
		}

		template<typename TRep, typename TPeriod>
		void sleep_for(const chrono::duration<TRep, TPeriod>& tRelTime)
		{
			long long iNs = chrono::duration_cast<chrono::nanoseconds>(tRelTime).count();

			if (iNs <= 0)
			{
				return;
			}

			long long iDelayHns = -(iNs / 100);

			if (iDelayHns == 0)
			{
				iDelayHns = -1;
			}

			Syscall::DelayExecution(iDelayHns);
		}
	}
}
