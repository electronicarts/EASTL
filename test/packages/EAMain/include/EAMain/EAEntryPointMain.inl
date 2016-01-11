///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EAMAIN_EAENTRYPOINTMAIN_INL
#define EAMAIN_EAENTRYPOINTMAIN_INL

#define EAMain main

namespace EA
{
    namespace EAMain
    {
		void PlatformStartup() {}
		void PlatformShutdown(int nErrorCount) 
		{
			printf("RETURNCODE=%d\n", nErrorCount);
		}
	}
}

#endif
