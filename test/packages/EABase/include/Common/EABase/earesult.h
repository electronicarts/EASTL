/*-----------------------------------------------------------------------------
 * earesult.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/


#ifndef INCLUDED_earesult_H
#define INCLUDED_earesult_H


#include <EABase/eabase.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once /* Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result. */
#endif



/* This result type is width-compatible with most systems. */
typedef int32_t ea_result_type;


namespace EA
{
	typedef int32_t result_type;

	enum
	{
#ifndef SUCCESS
		// Deprecated
		// Note: a public MS header has created a define of this name which causes a build error. Fortunately they
		// define it to 0 which is compatible.
		// see: WindowsSDK\8.1.51641-fb\installed\Include\um\RasError.h
		SUCCESS =  0,
#endif
		// Deprecated
		FAILURE = -1,

		// These values are now the preferred constants
		EA_SUCCESS =  0,
		EA_FAILURE = -1,
	};
}


/* Macro to simplify testing for success. */
#ifndef EA_SUCCEEDED
	#define EA_SUCCEEDED(result) ((result) >= 0)
#endif

/* Macro to simplfify testing for general failure. */
#ifndef EA_FAILED
	#define EA_FAILED(result) ((result) < 0)
#endif


#endif




