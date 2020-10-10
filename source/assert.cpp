///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/config.h>
#include <EASTL/string.h>
#include <EABase/eabase.h>

#if defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS();
	#if defined(EA_COMPILER_MSVC)
		#include <crtdbg.h>
	#endif
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	EA_RESTORE_ALL_VC_WARNINGS();
#elif defined(EA_PLATFORM_ANDROID)
	#include <android/log.h>
#else
	#include <stdio.h>
#endif




namespace eastl
{

	/// gpAssertionFailureFunction
	/// 
	/// Global assertion failure function pointer. Set by SetAssertionFailureFunction.
	/// 
	EASTL_API EASTL_AssertionFailureFunction gpAssertionFailureFunction        = AssertionFailureFunctionDefault;
	EASTL_API void*                          gpAssertionFailureFunctionContext = NULL;



	/// SetAssertionFailureFunction
	///
	/// Sets the function called when an assertion fails. If this function is not called
	/// by the user, a default function will be used. The user may supply a context parameter
	/// which will be passed back to the user in the function call. This is typically used
	/// to store a C++ 'this' pointer, though other things are possible.
	///
	/// There is no thread safety here, so the user needs to externally make sure that
	/// this function is not called in a thread-unsafe way. The easiest way to do this is 
	/// to just call this function once from the main thread on application startup.
	///
	EASTL_API void SetAssertionFailureFunction(EASTL_AssertionFailureFunction pAssertionFailureFunction, void* pContext)
	{
		gpAssertionFailureFunction        = pAssertionFailureFunction;
		gpAssertionFailureFunctionContext = pContext;
	}



	/// AssertionFailureFunctionDefault
	///
	EASTL_API void AssertionFailureFunctionDefault(const char* pExpression, void* /*pContext*/)
	{
		#if EASTL_ASSERT_ENABLED
			#if defined(EA_PLATFORM_MICROSOFT)
				printf("%s\n", pExpression); // Write the message to stdout
				if( ::IsDebuggerPresent())
				{
					OutputDebugStringA(pExpression);
				}
			#elif defined(EA_PLATFORM_ANDROID)
				__android_log_print(ANDROID_LOG_INFO, "PRINTF", "%s\n", pExpression);
			#else
				printf("%s\n", pExpression); // Write the message to stdout, which happens to be the trace view for many console debug machines.
			#endif
		#else
			EA_UNUSED(pExpression);
		#endif

		EASTL_DEBUG_BREAK();
	}


	/// AssertionFailure
	///
	EASTL_API void AssertionFailure(const char* pExpression)
	{
		if(gpAssertionFailureFunction)
			gpAssertionFailureFunction(pExpression, gpAssertionFailureFunctionContext);
	}


} // namespace eastl















