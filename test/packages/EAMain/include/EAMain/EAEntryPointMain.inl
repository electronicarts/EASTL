///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EAMAIN_EAENTRYPOINTMAIN_INL
#define EAMAIN_EAENTRYPOINTMAIN_INL


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


int EAMain(int argc, char* argv[]);
int main(int argc, char* argv[])
    { return EAMain(argc, argv); } 

#endif
