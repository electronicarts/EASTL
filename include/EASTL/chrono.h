/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// This file implements the eastl::chrono specification which is part of the
// standard STL date and time library.  eastl::chrono implements all the
// mechanisms required to capture and manipulate times retrieved from the
// provided clocks.  It implements the all of the features to allow type safe
// durations to be used in code.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_CHRONO_H
#define EASTL_CHRONO_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once 
#endif

#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <EASTL/numeric_limits.h>
#include <EASTL/ratio.h>


// TODO:  move to platform specific cpp or header file
#if  defined EA_PLATFORM_MICROSOFT
	EA_DISABLE_ALL_VC_WARNINGS()

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#undef NOMINMAX
	#define NOMINMAX

	#include <Windows.h>

	#ifdef min
		#undef min
	#endif
	#ifdef max
		#undef max
	#endif

	EA_RESTORE_ALL_VC_WARNINGS()
#endif

#if defined(EA_PLATFORM_MICROSOFT) && !defined(EA_PLATFORM_MINGW)
	// Nothing to do
#elif defined(EA_PLATFORM_SONY)
	#include <Dinkum/threads/xtimec.h>
	#include <kernel.h>
#elif defined(EA_PLATFORM_APPLE)
	#include <mach/mach_time.h>
#elif defined(EA_PLATFORM_POSIX) || defined(EA_PLATFORM_MINGW) || defined(EA_PLATFORM_ANDROID) 
	// Posix means Linux, Unix, and Macintosh OSX, among others (including Linux-based mobile platforms).
	#if defined(EA_PLATFORM_MINGW)
		#include <pthread_time.h>
	#endif
	#include <time.h>
	#if (defined(CLOCK_REALTIME) || defined(CLOCK_MONOTONIC))
		#include <errno.h>
	#else
		#include <sys/time.h>
		#include <unistd.h>
	#endif
#endif


namespace eastl
{
namespace chrono
{
	///////////////////////////////////////////////////////////////////////////////
	// treat_as_floating_point
	///////////////////////////////////////////////////////////////////////////////
	template <class Rep>
	struct treat_as_floating_point : is_floating_point<Rep> {};


    ///////////////////////////////////////////////////////////////////////////////
	// 20.12.4, duration_values
	///////////////////////////////////////////////////////////////////////////////
	template <class Rep>
	struct duration_values
	{
	public:
		EASTL_FORCE_INLINE static EA_CONSTEXPR Rep zero() { return Rep(0); }
		EASTL_FORCE_INLINE static EA_CONSTEXPR Rep max()  { return eastl::numeric_limits<Rep>::max(); }
		EASTL_FORCE_INLINE static EA_CONSTEXPR Rep min()  { return eastl::numeric_limits<Rep>::lowest(); }
	};


	///////////////////////////////////////////////////////////////////////////////
	// duration fwd_decl
	///////////////////////////////////////////////////////////////////////////////
	template <typename Rep, typename Period = ratio<1>>
	class duration;


	namespace Internal
	{
		///////////////////////////////////////////////////////////////////////////////
		// IsRatio 
		///////////////////////////////////////////////////////////////////////////////
		template <typename> struct IsRatio                                           : eastl::false_type {};
		template <intmax_t N, intmax_t D> struct IsRatio<ratio<N, D>>                : eastl::true_type {};
		template <intmax_t N, intmax_t D> struct IsRatio<const ratio<N, D>>          : eastl::true_type {};
		template <intmax_t N, intmax_t D> struct IsRatio<volatile ratio<N, D>>       : eastl::true_type {};
		template <intmax_t N, intmax_t D> struct IsRatio<const volatile ratio<N, D>> : eastl::true_type {};


		///////////////////////////////////////////////////////////////////////////////
		// IsDuration 
		///////////////////////////////////////////////////////////////////////////////
		template<typename> struct IsDuration                                                            : eastl::false_type{};
		template<typename Rep, typename Period> struct IsDuration<duration<Rep, Period>>                : eastl::true_type{};
		template<typename Rep, typename Period> struct IsDuration<const duration<Rep, Period>>          : eastl::true_type{};
		template<typename Rep, typename Period> struct IsDuration<volatile duration<Rep, Period>>       : eastl::true_type{};
		template<typename Rep, typename Period> struct IsDuration<const volatile duration<Rep, Period>> : eastl::true_type{};


		///////////////////////////////////////////////////////////////////////////////
		// RatioGCD 
		///////////////////////////////////////////////////////////////////////////////
		template <class Period1, class Period2>
		struct RatioGCD
		{
			static_assert(IsRatio<Period1>::value, "Period1 is not a eastl::ratio type");
			static_assert(IsRatio<Period2>::value, "Period2 is not a eastl::ratio type");

			typedef ratio<eastl::Internal::gcd<Period1::num, Period2::num>::value,
			              eastl::Internal::lcm<Period1::den, Period2::den>::value> type;
		};
	};


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.5.7, duration_cast
	///////////////////////////////////////////////////////////////////////////////
	namespace Internal
	{
		template <typename FromDuration,
		          typename ToDuration,
		          typename CommonPeriod =
		              typename ratio_divide<typename FromDuration::period, typename ToDuration::period>::type,
		          typename CommonRep = typename eastl::decay<typename eastl::common_type<typename ToDuration::rep,
		                                                                                 typename FromDuration::rep,
		                                                                                 intmax_t>::type>::type,
		          bool = CommonPeriod::num == 1,
		          bool = CommonPeriod::den == 1>
		struct DurationCastImpl;

		template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
		struct DurationCastImpl<FromDuration, ToDuration, CommonPeriod, CommonRep, true, true>
		{
			inline static ToDuration DoCast(const FromDuration& fd)
			{
				return ToDuration(static_cast<typename ToDuration::rep>(fd.count()));
			}
		};

		template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
		struct DurationCastImpl<FromDuration, ToDuration, CommonPeriod, CommonRep, false, true>
		{
			inline static ToDuration DoCast(const FromDuration& d)
			{
				return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CommonRep>(d.count()) *
				                                                        static_cast<CommonRep>(CommonPeriod::num)));
			}
		};

		template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
		struct DurationCastImpl<FromDuration, ToDuration, CommonPeriod, CommonRep, true, false>
		{
			inline static ToDuration DoCast(const FromDuration& d)
			{
				return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CommonRep>(d.count()) /
				                                                        static_cast<CommonRep>(CommonPeriod::den)));
			}
		};

		template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
		struct DurationCastImpl<FromDuration, ToDuration, CommonPeriod, CommonRep, false, false>
		{
			inline static ToDuration DoCast(const FromDuration& d)
			{
				return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CommonRep>(d.count()) *
				                                                        static_cast<CommonRep>(CommonPeriod::num) /
				                                                        static_cast<CommonRep>(CommonPeriod::den)));
			}
		};
	}; // namespace Internal


	///////////////////////////////////////////////////////////////////////////////
	// duration_cast 
	///////////////////////////////////////////////////////////////////////////////
	template <typename ToDuration, typename Rep, typename Period>
	inline typename eastl::enable_if<Internal::IsDuration<ToDuration>::value, ToDuration>::type 
	duration_cast(const duration<Rep, Period>& d)
	{
		typedef typename duration<Rep, Period>::this_type FromDuration;
		return Internal::DurationCastImpl<FromDuration, ToDuration>::DoCast(d);
	}


	///////////////////////////////////////////////////////////////////////////////
	// duration 
	///////////////////////////////////////////////////////////////////////////////
	template <class Rep, class Period>
	class duration
	{
		Rep mRep;	

	public:
		typedef Rep rep;
		typedef Period period;
		typedef duration<Rep, Period> this_type;

    #if defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS)
		EA_CONSTEXPR duration() 
			: mRep() {}

		duration(const duration& other)
			: mRep(Rep(other.mRep)) {}

		duration& operator=(const duration& other)
			{ mRep = other.mRep; return *this; }
	#else
		EA_CONSTEXPR duration() = default;
		duration(const duration&) = default;
		duration& operator=(const duration&) = default;
    #endif


		///////////////////////////////////////////////////////////////////////////////
		// conversion constructors 
		///////////////////////////////////////////////////////////////////////////////
		template <class Rep2>
		inline EA_CONSTEXPR explicit duration(
		    const Rep2& rep2,
		    typename eastl::enable_if<eastl::is_convertible<Rep2, Rep>::value &&
		                              (treat_as_floating_point<Rep>::value ||
		                               !treat_as_floating_point<Rep2>::value)>::type** = 0)
		    : mRep(static_cast<Rep>(rep2)) {}


		template <class Rep2, class Period2>
		EA_CONSTEXPR duration(const duration<Rep2, Period2>& d2,
		                      typename eastl::enable_if<treat_as_floating_point<Rep>::value ||
		                                                    (eastl::ratio_divide<Period2, Period>::type::den == 1 &&
		                                                     !treat_as_floating_point<Rep2>::value),
		                                                void>::type** = 0)
		    : mRep(duration_cast<duration>(d2).count()) {}

		///////////////////////////////////////////////////////////////////////////////
		// returns the count of ticks 
		///////////////////////////////////////////////////////////////////////////////
		EA_CONSTEXPR Rep count() const { return mRep; }

		///////////////////////////////////////////////////////////////////////////////
		// static accessors of special duration values 
		///////////////////////////////////////////////////////////////////////////////
		EA_CONSTEXPR inline static duration zero() { return duration(duration_values<Rep>::zero()); }
		EA_CONSTEXPR inline static duration min()  { return duration(duration_values<Rep>::min()); }
		EA_CONSTEXPR inline static duration max()  { return duration(duration_values<Rep>::max()); }

		///////////////////////////////////////////////////////////////////////////////
		// const arithmetic operations
		///////////////////////////////////////////////////////////////////////////////
		EA_CONSTEXPR inline duration operator+() const { return *this; }
		EA_CONSTEXPR inline duration operator-() const { return duration(0-mRep); }

		///////////////////////////////////////////////////////////////////////////////
		// arithmetic operations
		///////////////////////////////////////////////////////////////////////////////
		inline duration operator++(int)                  { return duration(mRep++); }
		inline duration operator--(int)                  { return duration(mRep--); }
		inline duration& operator++()                    { ++mRep; return *this; }
		inline duration& operator--()                    { --mRep; return *this; }
		inline duration& operator+=(const duration& d)   { mRep += d.count(); return *this; }
		inline duration& operator-=(const duration& d)   { mRep -= d.count(); return *this; }
		inline duration& operator*=(const Rep& rhs)      { mRep *= rhs; return *this; }
		inline duration& operator/=(const Rep& rhs)      { mRep /= rhs; return *this; }
		inline duration& operator%=(const Rep& rhs)      { mRep %= rhs; return *this; }
		inline duration& operator%=(const duration& d)   { mRep %= d.count(); return *this; }
	};


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.5.5, arithmetic operations with durations as arguments
	///////////////////////////////////////////////////////////////////////////////
	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type EASTL_FORCE_INLINE
	operator+(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() + common_duration_t(rhs).count());
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type EASTL_FORCE_INLINE
	operator-(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() - common_duration_t(rhs).count());
	}

	template <typename Rep1, typename Period1, typename Rep2>
	duration<typename eastl::common_type<Rep1, Rep2>::type, Period1> EASTL_FORCE_INLINE
	operator*(const duration<Rep1, Period1>& lhs, const Rep2& rhs)
	{
		typedef duration<typename eastl::common_type<Rep1, Rep2>::type, Period1> common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() * rhs);
	}

	template <typename Rep1, typename Rep2, typename Period2>
	duration<typename eastl::common_type<Rep1, Rep2>::type, Period2> EASTL_FORCE_INLINE
	operator*(const Rep1& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef duration<typename eastl::common_type<Rep1, Rep2>::type, Period2> common_duration_t;
		return common_duration_t(lhs * common_duration_t(rhs).count());
	}

	template <typename Rep1, typename Period1, typename Rep2>
	duration<typename eastl::common_type<Rep1, Rep2>::type, Period1> EASTL_FORCE_INLINE
	operator/(const duration<Rep1, Period1>& lhs, const Rep2& rhs)
	{
		typedef duration<typename eastl::common_type<Rep1, Rep2>::type, Period1> common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() / rhs);
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type EASTL_FORCE_INLINE
	operator/(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() / common_duration_t(rhs).count());
	}

	template <typename Rep1, typename Period1, typename Rep2>
	duration<typename eastl::common_type<Rep1, Rep2>::type, Period1> EASTL_FORCE_INLINE
	operator%(const duration<Rep1, Period1>& lhs, const Rep2& rhs)
	{
		typedef duration<typename eastl::common_type<Rep1, Rep2>::type, Period1> common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() % rhs);
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type EASTL_FORCE_INLINE
	operator%(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type common_duration_t;
		return common_duration_t(common_duration_t(lhs).count() % common_duration_t(rhs).count());
	}


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.5.6, compares two durations
	///////////////////////////////////////////////////////////////////////////////
	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	EASTL_FORCE_INLINE bool operator==(const duration<Rep1, Period1>& lhs,
	                                                const duration<Rep2, Period2>& rhs)
	{
		typedef typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type common_duration_t;
		return common_duration_t(lhs).count() == common_duration_t(rhs).count();
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	EASTL_FORCE_INLINE bool operator<(const duration<Rep1, Period1>& lhs,
	                                               const duration<Rep2, Period2>& rhs)
	{
		typedef typename eastl::common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type common_duration_t;
		return common_duration_t(lhs).count() < common_duration_t(rhs).count();
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	EASTL_FORCE_INLINE bool operator!=(const duration<Rep1, Period1>& lhs,
	                                                const duration<Rep2, Period2>& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	EASTL_FORCE_INLINE bool operator<=(const duration<Rep1, Period1>& lhs,
	                                                const duration<Rep2, Period2>& rhs)
	{
		return !(rhs < lhs);
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	EASTL_FORCE_INLINE bool operator>(const duration<Rep1, Period1>& lhs,
	                                               const duration<Rep2, Period2>& rhs)
	{
		return rhs < lhs;
	}

	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	EASTL_FORCE_INLINE bool operator>=(const duration<Rep1, Period1>& lhs,
	                                                const duration<Rep2, Period2>& rhs)
	{
		return !(lhs < rhs);
	}


	///////////////////////////////////////////////////////////////////////////////
	// standard duration units
	///////////////////////////////////////////////////////////////////////////////
    typedef duration<long long, nano> nanoseconds;
    typedef duration<long long, micro> microseconds;
    typedef duration<long long, milli> milliseconds;
    typedef duration<long long> seconds;
    typedef duration<int, ratio<60>> minutes;
    typedef duration<int, ratio<3600>> hours;


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.6, time_point
	///////////////////////////////////////////////////////////////////////////////
	template <typename Clock, typename Duration = typename Clock::duration> 
	class time_point
	{
		Duration mDuration;

	public:
		typedef Clock clock;
		typedef Duration duration;
		typedef typename Duration::rep rep;
		typedef typename Duration::period period;

		inline EA_CONSTEXPR time_point() : mDuration(Duration::zero()) {}
		EA_CONSTEXPR explicit time_point(const Duration& other) : mDuration(other) {}

		template <typename Duration2>
		inline EA_CONSTEXPR time_point(
		    const time_point<Clock, Duration2>& t,
		    typename eastl::enable_if<eastl::is_convertible<Duration2, Duration>::value>::type** = 0)
		    : mDuration(t.time_since_epoch()) {}

		EA_CONSTEXPR Duration time_since_epoch() const { return mDuration; }

		time_point& operator+=(const Duration& d) { mDuration += d; return *this; } 
		time_point& operator-=(const Duration& d) { mDuration -= d; return *this; }

		static EA_CONSTEXPR time_point min() { return time_point(Duration::min()); }
		static EA_CONSTEXPR time_point max() { return time_point(Duration::max()); }
	};


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.6.5, time_point arithmetic
	///////////////////////////////////////////////////////////////////////////////
	template <class Clock, class Duration1, class Rep2, class Period2>
	inline EA_CONSTEXPR time_point<Clock, typename eastl::common_type<Duration1, duration<Rep2, Period2>>::type>
	operator+(const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef time_point<Clock, typename eastl::common_type<Duration1, duration<Rep2, Period2>>::type> common_timepoint_t;
		return common_timepoint_t(lhs.time_since_epoch() + rhs);
	}

	template <class Rep1, class Period1, class Clock, class Duration2>
	inline EA_CONSTEXPR time_point<Clock, typename eastl::common_type<Duration2, duration<Rep1, Period1>>::type>
	operator+(const duration<Rep1, Period1>& lhs, const time_point<Clock, Duration2>& rhs)
	{
		typedef time_point<Clock, typename eastl::common_type<Duration2, duration<Rep1, Period1>>::type> common_timepoint_t;
		return common_timepoint_t(lhs + rhs.time_since_epoch());
	}

	template <class Clock, class Duration1, class Rep2, class Period2>
	inline EA_CONSTEXPR time_point<Clock, typename eastl::common_type<Duration1, duration<Rep2, Period2>>::type>
	operator-(const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs)
	{
		typedef time_point<Clock, typename eastl::common_type<Duration1, duration<Rep2, Period2>>::type> common_timepoint_t;
		return common_timepoint_t(lhs.time_since_epoch() - rhs);
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR typename eastl::common_type<Duration1, Duration2>::type operator-(
	    const time_point<Clock, Duration1>& lhs,
	    const time_point<Clock, Duration2>& rhs)
	{
		return lhs.time_since_epoch() - rhs.time_since_epoch();
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR bool operator==(const time_point<Clock, Duration1>& lhs,
	                                    const time_point<Clock, Duration2>& rhs)
	{
		return lhs.time_since_epoch() == rhs.time_since_epoch();
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR bool operator!=(const time_point<Clock, Duration1>& lhs,
	                                    const time_point<Clock, Duration2>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR bool operator<(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
	{
		return lhs.time_since_epoch() < rhs.time_since_epoch();
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR bool operator<=(const time_point<Clock, Duration1>& lhs,
	                                    const time_point<Clock, Duration2>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR bool operator>(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
	{
		return rhs < lhs;
	}

	template <class Clock, class Duration1, class Duration2>
	inline EA_CONSTEXPR bool operator>=(const time_point<Clock, Duration1>& lhs,
	                                    const time_point<Clock, Duration2>& rhs)
	{
		return !(lhs < rhs);
	}


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.6.7, time_point_cast
	///////////////////////////////////////////////////////////////////////////////
	template <typename ToDuration, typename Clock, typename Duration>
	EA_CONSTEXPR time_point<Clock, ToDuration> time_point_cast(
	    const time_point<Clock, Duration>& t,
	    typename eastl::enable_if<Internal::IsDuration<ToDuration>::value>::type** = 0)
	{
		return time_point<Clock, ToDuration>(duration_cast<ToDuration>(t.time_since_epoch()));
	}


	///////////////////////////////////////////////////////////////////////////////
	// 20.12.7, clocks
	///////////////////////////////////////////////////////////////////////////////

	namespace Internal
	{
		#if defined(EA_PLATFORM_MICROSOFT) && !defined(EA_PLATFORM_MINGW)
			#define EASTL_NS_PER_TICK 1 
		#elif defined EA_PLATFORM_SONY
			#define EASTL_NS_PER_TICK 1
		#elif defined EA_PLATFORM_POSIX
			#define EASTL_NS_PER_TICK _XTIME_NSECS_PER_TICK
		#else
			#define EASTL_NS_PER_TICK 100
		#endif

		#if defined(EA_PLATFORM_POSIX) 
			typedef chrono::nanoseconds::period SystemClock_Period;
			typedef chrono::nanoseconds::period SteadyClock_Period;
		#else
			typedef eastl::ratio_multiply<eastl::ratio<EASTL_NS_PER_TICK, 1>, nano>::type SystemClock_Period; 
			typedef eastl::ratio_multiply<eastl::ratio<EASTL_NS_PER_TICK, 1>, nano>::type SteadyClock_Period; 
		#endif


		///////////////////////////////////////////////////////////////////////////////
		// Internal::GetTicks 
		///////////////////////////////////////////////////////////////////////////////
		inline uint64_t GetTicks()
		{
		#if defined EA_PLATFORM_MICROSOFT
			auto queryFrequency = []
			{
				LARGE_INTEGER frequency;
				QueryPerformanceFrequency(&frequency);
				return double(1000000000.0L / (long double)frequency.QuadPart);  // nanoseconds per tick
			};

			auto queryCounter = []
			{
				LARGE_INTEGER counter;
				QueryPerformanceCounter(&counter);
				return counter.QuadPart;
			};

			EA_DISABLE_VC_WARNING(4640)  // warning C4640: construction of local static object is not thread-safe (VS2013)
			static auto frequency = queryFrequency(); // cache cpu frequency on first call
			EA_RESTORE_VC_WARNING()
			return uint64_t(frequency * (double)queryCounter());
		#elif defined EA_PLATFORM_SONY
			auto queryFrequency = []
			{
				// nanoseconds/seconds / ticks/seconds
				return double(1000000000.0L / (long double)sceKernelGetProcessTimeCounterFrequency());  // nanoseconds per tick
			};

			auto queryCounter = []
			{
				return sceKernelGetProcessTimeCounter();
			};

			EA_DISABLE_VC_WARNING(4640)  // warning C4640: construction of local static object is not thread-safe (VS2013)
			static auto frequency = queryFrequency(); // cache cpu frequency on first call
			EA_RESTORE_VC_WARNING()
			return uint64_t(frequency * (double)queryCounter());
		#elif defined(EA_PLATFORM_APPLE)
			auto queryTimeInfo = []
			{
				mach_timebase_info_data_t info;
				mach_timebase_info(&info);
				return info;
			};
			
			static auto timeInfo = queryTimeInfo();
			uint64_t t = mach_absolute_time();
			t *= timeInfo.numer;
			t /= timeInfo.denom;
			return t;
		#elif defined(EA_PLATFORM_POSIX) // Posix means Linux, Unix, and Macintosh OSX, among others (including Linux-based mobile platforms).
			#if (defined(CLOCK_REALTIME) || defined(CLOCK_MONOTONIC))
				timespec ts;
				int result = clock_gettime(CLOCK_MONOTONIC, &ts);

				if (result == -1 && errno == EINVAL)
					result = clock_gettime(CLOCK_REALTIME, &ts);

				const uint64_t nNanoseconds = (uint64_t)ts.tv_nsec + ((uint64_t)ts.tv_sec * UINT64_C(1000000000));
				return nNanoseconds;
			#else
				struct timeval tv;
				gettimeofday(&tv, NULL);
				const uint64_t nMicroseconds = (uint64_t)tv.tv_usec + ((uint64_t)tv.tv_sec * 1000000);
				return nMicroseconds;
			#endif
        #else
			#error "chrono not implemented for platform"
		#endif
		}
	} // namespace Internal


	///////////////////////////////////////////////////////////////////////////////
	// system_clock 
	///////////////////////////////////////////////////////////////////////////////
	class system_clock
	{
	public:
		typedef long long rep; // signed arithmetic type representing the number of ticks in the clock's duration
		typedef Internal::SystemClock_Period period;
		typedef chrono::duration<rep, period> duration; // duration<rep, period>, capable of representing negative durations
		typedef chrono::time_point<system_clock> time_point;

		// true if the time between ticks is always increases monotonically
		EA_CONSTEXPR_OR_CONST static bool is_steady = false;

		// returns a time point representing the current point in time.
		static time_point now() EA_NOEXCEPT 
		{ 
			return time_point(duration(Internal::GetTicks())); 
		}
	};


	///////////////////////////////////////////////////////////////////////////////
	// steady_clock 
	///////////////////////////////////////////////////////////////////////////////
	class steady_clock
	{
	public:
		typedef long long rep; // signed arithmetic type representing the number of ticks in the clock's duration
		typedef Internal::SteadyClock_Period period;
		typedef chrono::duration<rep, period> duration; // duration<rep, period>, capable of representing negative durations
		typedef chrono::time_point<steady_clock> time_point;

		// true if the time between ticks is always increases monotonically
		EA_CONSTEXPR_OR_CONST static bool is_steady = true;

		// returns a time point representing the current point in time.
		static time_point now() EA_NOEXCEPT 
		{ 
			return time_point(duration(Internal::GetTicks())); 
		}
	};


	///////////////////////////////////////////////////////////////////////////////
	// high_resolution_clock 
	///////////////////////////////////////////////////////////////////////////////
	typedef system_clock high_resolution_clock;


} // namespace chrono 


	///////////////////////////////////////////////////////////////////////////////
	// duration common_type specialization 
	///////////////////////////////////////////////////////////////////////////////
	template <typename Rep1, typename Period1, typename Rep2, typename Period2>
	struct common_type<chrono::duration<Rep1, Period1>, chrono::duration<Rep2, Period2>>
	{
	    typedef chrono::duration<typename eastl::decay<typename eastl::common_type<Rep1, Rep2>::type>::type,
	                             typename chrono::Internal::RatioGCD<Period1, Period2>::type> type;
    };


	///////////////////////////////////////////////////////////////////////////////
	// time_point common_type specialization 
	///////////////////////////////////////////////////////////////////////////////
	template <typename Clock, typename Duration1, typename Duration2>
	struct common_type<chrono::time_point<Clock, Duration1>, chrono::time_point<Clock, Duration2>>
	{
		typedef chrono::time_point<Clock, typename eastl::common_type<Duration1, Duration2>::type> type;
	};


	///////////////////////////////////////////////////////////////////////////////
	// chrono_literals  
	///////////////////////////////////////////////////////////////////////////////
	#if EASTL_USER_LITERALS_ENABLED && EASTL_INLINE_NAMESPACES_ENABLED
		// Disabling the Clang/GCC/MSVC warning about using user
		// defined literals without a leading '_' as they are reserved
		// for standard libary usage.
		EA_DISABLE_VC_WARNING(4455)
		EA_DISABLE_CLANG_WARNING(-Wuser-defined-literals)
		EA_DISABLE_GCC_WARNING(-Wliteral-suffix)
		inline namespace literals
		{
			inline namespace chrono_literals
			{
				///////////////////////////////////////////////////////////////////////////////
				// integer chrono literals
				///////////////////////////////////////////////////////////////////////////////
				EA_CONSTEXPR chrono::hours operator"" h(unsigned long long h) { return chrono::hours(h); }
				EA_CONSTEXPR chrono::minutes operator"" min(unsigned long long m) { return chrono::minutes(m); }
				EA_CONSTEXPR chrono::seconds operator"" s(unsigned long long s) { return chrono::seconds(s); }
				EA_CONSTEXPR chrono::milliseconds operator"" ms(unsigned long long ms) { return chrono::milliseconds(ms); }
				EA_CONSTEXPR chrono::microseconds operator"" us(unsigned long long us) { return chrono::microseconds(us); }
				EA_CONSTEXPR chrono::nanoseconds operator"" ns(unsigned long long ns) { return chrono::nanoseconds(ns); }

				///////////////////////////////////////////////////////////////////////////////
				// float chrono literals
				///////////////////////////////////////////////////////////////////////////////
				EA_CONSTEXPR chrono::duration<long double, ratio<3600, 1>> operator"" h(long double h)
					{ return chrono::duration<long double, ratio<3600, 1>>(h); }
				EA_CONSTEXPR chrono::duration<long double, ratio<60, 1>> operator"" min(long double m)
					{ return chrono::duration<long double, ratio<60, 1>>(m); }
				EA_CONSTEXPR chrono::duration<long double> operator"" s(long double s)
					{ return chrono::duration<long double>(s); }
				EA_CONSTEXPR chrono::duration<float, milli> operator"" ms(long double ms)
					{ return chrono::duration<long double, milli>(ms); }
				EA_CONSTEXPR chrono::duration<float, micro> operator"" us(long double us)
					{ return chrono::duration<long double, micro>(us); }
				EA_CONSTEXPR chrono::duration<float, nano> operator"" ns(long double ns)
					{ return chrono::duration<long double, nano>(ns); }

			} // namespace chrono_literals
		}// namespace literals
		EA_RESTORE_GCC_WARNING()	// -Wliteral-suffix
		EA_RESTORE_CLANG_WARNING()	// -Wuser-defined-literals
		EA_RESTORE_VC_WARNING()		// warning: 4455
	#endif

} // namespace eastl


#if EASTL_USER_LITERALS_ENABLED && EASTL_INLINE_NAMESPACES_ENABLED
namespace chrono
{
	using namespace eastl::literals::chrono_literals;
} // namespace chrono
#endif


#endif 
