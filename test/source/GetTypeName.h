/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef GETTYPENAME_H
#define GETTYPENAME_H


#include <EABase/eabase.h>
#include <EASTL/type_traits.h>
#include <EASTL/string.h>
#include <stdlib.h>
#include <typeinfo>


///////////////////////////////////////////////////////////////////////////////
// EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE
//
// Defined as 0 or 1. The value depends on the compilation environment.
// Indicates if we can use system-provided abi::__cxa_demangle() at runtime.
//
#if !defined(EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE)
	#if (defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_APPLE)) && defined(EA_PLATFORM_DESKTOP)
		#define EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE 1
	#else
		#define EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE 0
	#endif
#endif


#if EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE
	#include <cxxabi.h>
#elif EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	EA_DISABLE_ALL_VC_WARNINGS();
	#include <Windows.h>
	#include <DbgHelp.h>
	#pragma comment(lib, "dbghelp.lib")
	EA_RESTORE_ALL_VC_WARNINGS();
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTLTEST_GETTYPENAME_AVAILABLE
//
// Defined as 0 or 1. The value depends on the compilation environment.
// Indicates if we can use system-provided abi::__cxa_demangle() at runtime.
//
#if !defined(EASTLTEST_GETTYPENAME_AVAILABLE)
	#if (EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE || EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)) && (!defined(EA_COMPILER_NO_RTTI) || defined(_MSC_VER)) // VC++ works without RTTI enabled.
		#define EASTLTEST_GETTYPENAME_AVAILABLE 1
	#else
		#define EASTLTEST_GETTYPENAME_AVAILABLE 0
	#endif
#endif


/// GetTypeName
///
/// Returns the type name of a templated type.
///
template <typename T>
eastl::string GetTypeName()
{
	eastl::string result;

	#if !defined(EA_COMPILER_NO_RTTI) || defined(_MSC_VER) // VC++ works without RTTI enabled.
		typedef typename eastl::remove_reference<T>::type TR;

		const char* pName = typeid(TR).name();

		#if EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE
			const char* pDemangledName = abi::__cxa_demangle(pName, NULL, NULL, NULL);

		#elif EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			char pDemangledName[1024];
			DWORD count = UnDecorateSymbolName(pName, pDemangledName, (DWORD)EAArrayCount(pDemangledName), UNDNAME_NO_THISTYPE | UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_MEMBER_TYPE);
			if(count == 0)
				pDemangledName[0] = 0;
		#else
			const char* pDemangledName = NULL;
		#endif

		if(pDemangledName && pDemangledName[0])
			result = pDemangledName;
		else
			result = pName;

		if(eastl::is_const<TR>::value)
			result += " const";

		if(eastl::is_volatile<TR>::value)
			result += " volatile";

		if(eastl::is_lvalue_reference<T>::value)
			result += "&";
		else if(eastl::is_rvalue_reference<T>::value)
			result += "&&";

		if(pDemangledName)
		{
			#if EASTL_LIBSTDCPP_DEMANGLE_AVAILABLE
				free((void*)(pDemangledName));
			#endif
		}
	#endif

	return result;
}


#endif // Header include guard







