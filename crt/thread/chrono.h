#pragma once

#include "../core/common.h"
#include <ratio>

namespace std
{
	namespace chrono
	{
		template<typename TRep, typename TPeriod>
		class duration
		{
		public:
			typedef TRep rep;
			typedef TPeriod period;

			constexpr duration() : tCount(0) {}
			explicit constexpr duration(rep tVal) : tCount(tVal) {}

			constexpr rep count() const
			{
				return tCount;
			}

		private:
			rep tCount;
		};

		template<typename TToDuration, typename TRep, typename TPeriod>
		TToDuration duration_cast(const duration<TRep, TPeriod>& tFrom)
		{
			long double lfValue = (long double)tFrom.count();
			long double lfSrcNum = (long double)TPeriod::num;
			long double lfSrcDen = (long double)TPeriod::den;
			long double lfDstNum = (long double)TToDuration::period::num;
			long double lfDstDen = (long double)TToDuration::period::den;
			long double lfResult = lfValue * lfSrcNum * lfDstDen;
			lfResult = lfResult / (lfSrcDen * lfDstNum);

			return TToDuration((typename TToDuration::rep)lfResult);
		}

		using nanoseconds = duration<long long, std::ratio<1, 1000000000>>;
		using microseconds = duration<long long, std::ratio<1, 1000000>>;
		using milliseconds = duration<long long, std::ratio<1, 1000>>;
		using seconds = duration<long long, std::ratio<1, 1>>;
		using minutes = duration<long long, std::ratio<60, 1>>;
		using hours = duration<long long, std::ratio<3600, 1>>;
	}
}