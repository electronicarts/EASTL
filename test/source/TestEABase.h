/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EABase/version.h>


// What we do here is verify that EA_PRAGMA_ONCE_SUPPORTED works as intended.
// This header file should be #included two times by TestEABase.cpp
// in order to test this.

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
	const int EABaseOncePerTranslationUnitTestVariable = 0;    // This should get compiled only once ever for a compilation unit.
#else
	// Just implement a classic manual header include guard. 
	// In this case we aren't really testing anything.
	#ifndef TESTEABASE_H
	#define TESTEABASE_H
		const int EABaseOncePerTranslationUnitTestVariable = 0;
	#endif
#endif



// EA_EXTERN_TEMPLATE / EA_COMPILER_NO_EXTERN_TEMPLATE

#if defined(__cplusplus)
	template <typename T>
	struct eabase_template
	{
		T value;
		T GetValue() const { return value; }
	};

	EA_EXTERN_TEMPLATE(struct eabase_template<char>);
#endif


