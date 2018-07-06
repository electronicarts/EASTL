/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/chrono.h>
#include <EASTL/numeric.h>
#include <EASTL/string.h>


using namespace eastl;
using namespace eastl::chrono;


//////////////////////////////////////////////////////////////////////////////////////////////////
// TestDuration
//
int TestDuration()
{
	int nErrorCount = 0;

	{
		hours h{1}; // 1 hour
		milliseconds ms{3}; // 3 milliseconds
		duration<int, kilo> ks{3}; // 3000 seconds

		duration<double, ratio<1, 30>> hz30{3.5};
		microseconds us = ms; 
		duration<double, milli> ms2 = us; // 3.0 milliseconds

		EA_UNUSED(h);
		EA_UNUSED(ms2);
		EA_UNUSED(ks);
		EA_UNUSED(hz30);
		EA_UNUSED(us);
	}

	{
		typedef duration<double, ratio<1, 30>> dur_t;
		VERIFY(dur_t::min()  < dur_t::zero());
		VERIFY(dur_t::zero() < dur_t::max());
		VERIFY(dur_t::min()  < dur_t::max());
	}

	{
		seconds s1(10);
		seconds s2 = -s1;
		VERIFY(s1.count() ==  10);
		VERIFY(s2.count() == -10);
	}

	{
		{
			hours h(1);
			minutes m = ++h;
			m--;
			VERIFY(m.count() == 119);
		}

		{
			hours h(24);
			minutes m = h;
			seconds s = m; 
			milliseconds ms = s; 

			VERIFY(h.count()  == 24);
			VERIFY(m.count()  == 1440);
			VERIFY(s.count()  == 86400);
			VERIFY(ms.count() == 86400000);
		}

		{    
			minutes m(11);
			m *= 2;
			VERIFY(m.count() == 22);
			m += hours(10); 
			VERIFY(m.count() == 622);
			VERIFY(duration_cast<hours>(m).count() == 10);
			m %= hours(1);
			VERIFY(duration_cast<hours>(m).count() == 0);
			VERIFY(m.count() == 22);
		}

		{
			milliseconds ms(3);       // 3 milliseconds
			VERIFY(ms.count() == 3);

			microseconds us = 2 * ms; // 6000 microseconds constructed from 3 milliseconds
			VERIFY(us.count() == 6000);

			microseconds us2 = us / 2;
			VERIFY(us2.count() == 3000);

			microseconds us3 = us % 2;
			VERIFY(us3.count() == 0);
		}
	}

	return nErrorCount;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// TestTimePoint
//
int TestTimePoint()
{
	int nErrorCount = 0;
	{
		{
			system_clock::time_point t0 = system_clock::now();
			auto tomorrow = t0 + hours(24);
			auto today = tomorrow - system_clock::now();
			auto hours_count = duration_cast<hours>(today).count();

			VERIFY(hours_count == 24 || hours_count == 23);  // account for time flux
		}

		{
			time_point<system_clock, hours> hour1(hours(1));

			auto hour_to_min 	  = time_point_cast<minutes>(hour1);
			auto hour_to_sec      = time_point_cast<seconds>(hour1);
			auto hour_to_millisec = time_point_cast<milliseconds>(hour1);
			auto hour_to_microsec = time_point_cast<microseconds>(hour1);
			auto hour_to_nanosec  = time_point_cast<nanoseconds>(hour1);
		
			VERIFY(hour_to_min.time_since_epoch().count()      == 60);
			VERIFY(hour_to_sec.time_since_epoch().count()      == 3600);
			VERIFY(hour_to_millisec.time_since_epoch().count() == 3600000ll);
			VERIFY(hour_to_microsec.time_since_epoch().count() == 3600000000ll);
			VERIFY(hour_to_nanosec.time_since_epoch().count()  == 3600000000000ll);
		}
	}
	return nErrorCount;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// TestClocks
//
int TestClocks()
{
	int nErrorCount = 0;
	{
		{
			auto sys = system_clock::now();
			VERIFY(sys.time_since_epoch().count() > 0);

			auto stdy = steady_clock::now();
			VERIFY(stdy.time_since_epoch().count() > 0);

			auto hrc = high_resolution_clock::now();
			VERIFY(hrc.time_since_epoch().count() > 0);
		}

		{
			auto start = system_clock::now();
			auto end   = system_clock::now();
			auto d = end - start;
			EA_UNUSED(d);
			VERIFY(d.count() >= 0);
		}

		{
			auto start = steady_clock::now();
			auto end   = steady_clock::now();
			auto d = end - start;
			EA_UNUSED(d);
			VERIFY(d.count() >= 0);
		}

		{
			auto start = high_resolution_clock::now();
			auto end   = high_resolution_clock::now();
			auto d = end - start;
			EA_UNUSED(d);
			VERIFY(d.count() >= 0);
		}

		{
			typedef duration<int, ratio<1, 100000000>> shakes;
			typedef duration<int, centi> jiffies;
			typedef duration<float, ratio<12096, 10000>> microfortnights;
			typedef duration<float, ratio<3155, 1000>> nanocenturies;

			seconds sec(1);

			VERIFY(duration_cast<shakes>(sec).count() == 100000000);
			VERIFY(duration_cast<jiffies>(sec).count() == 100);
			VERIFY(microfortnights(sec).count() > 0.82f);
			VERIFY(nanocenturies(sec).count() > 0.31f);
		}
	}
	return nErrorCount;
}


int TestChrono()
{
	int nErrorCount = 0;
	nErrorCount += TestDuration();
	nErrorCount += TestTimePoint();
	nErrorCount += TestClocks();
	return nErrorCount;
}









