/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/finally.h>


int TestFinally()
{
	using namespace eastl;

	int nErrorCount = 0;

	{
		#if defined(EA_COMPILER_CPP17_ENABLED)
		{
			// requires CTAD (class template argument deduction)
			int a = 0;
			{
				VERIFY(a == 0);
				eastl::finally _([&] { a = 42; }); 
				VERIFY(a == 0);
			}
			VERIFY(a == 42);
		}
		#endif

		{
			int a = 0;
			{
				VERIFY(a == 0);
				auto _ = eastl::make_finally([&] { a = 42; });
				VERIFY(a == 0);
			}
			VERIFY(a == 42);
		}

		{
			int a = 0;
			{
				VERIFY(a == 0);
				auto f = eastl::make_finally([&] { a = 42; });
				VERIFY(a == 0);
				f.dismiss();
				VERIFY(a == 0);
			}
			VERIFY(a == 0);
		}

		{
			int a = 0;
			{
				VERIFY(a == 0);
				auto f = eastl::make_finally([&] { a = 42; });
				VERIFY(a == 0);
				f.execute();
				VERIFY(a == 42);
			}
			VERIFY(a == 42);
		}
		
		{
			int a = 0;
			{
				VERIFY(a == 0);
				auto f = eastl::make_finally([&] { a = 42; });
				VERIFY(a == 0);
				f.execute();
				VERIFY(a == 42);

				// verify the finally object doesn't re-run the callback on scope-exit.
				a = -1;  
			}
			VERIFY(a == -1);
		}

		{
			struct local_flag { bool b = false; };

			local_flag lf;
			VERIFY(lf.b == false);

			{ auto _ = eastl::make_finally([&] { lf.b = true; }); }

			VERIFY(lf.b);
		}

		// This currently does not compile by design.
		//
		// {
		//     int a = 0;
		//     auto lbda = [&a] { a = 1234; };
		//     {
		//         VERIFY(a == 0);
		//         auto _ = eastl::make_finally(lbda);  // compiler error
		//         VERIFY(a == 0);
		//     }
		//     VERIFY(a == 1234);
		// }
	}

	return nErrorCount;
}


