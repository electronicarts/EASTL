///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EAMAIN_EAENTRYPOINTMAIN_INL
#define EAMAIN_EAENTRYPOINTMAIN_INL

#include <cctype>

namespace EA
{
    namespace EAMain
    {
		namespace Internal
		{
			unsigned gVerbosityLevel = 0;	
		}

		void PlatformStartup() {}
		void PlatformShutdown(int nErrorCount) 
		{
			printf("RETURNCODE=%d\n", nErrorCount);
		}


		CommandLine::CommandLine(int argc, char** argv)
			:m_argc(argc)
			,m_argv(argv)
		{
		}

		namespace
		{
			bool HasPrefix(const char *str, const char *prefix, bool bCaseSensitive)
			{
				for (; *prefix != '\0'; str++, prefix++)
				{
					if (bCaseSensitive)
					{
						if (*str != *prefix)
						{
							return false;
						}
					}
					else
					{
						if (std::tolower(*str) != std::tolower(*prefix))
						{
							return false;
						}
					}
				}
				return true;
			}
		}
		
		int CommandLine::FindSwitch(const char *pSwitch, bool bCaseSensitive, const char **pResult, int nStartingIndex, char delimiter) const
		{
			const size_t switchLen = strlen(pSwitch);
			for (int i = nStartingIndex; i < m_argc; i++)
			{
				const char *arg = m_argv[i];
				bool prefixMatch = false;
				if (HasPrefix(arg, pSwitch, bCaseSensitive) && (arg[switchLen] == '\0' || arg[switchLen] == delimiter))
				{
					if (pResult)
					{
						*pResult = &arg[switchLen + 1];
					}
					return i;
				}
			}
			return -1;
		}

		bool CommandLine::HasHelpSwitch() const
		{
			return FindSwitch("-?") >= 0 || FindSwitch("-h") >= 0 || FindSwitch("-help") >= 0;
		}
	}
}


int EAMain(int argc, char* argv[]);
int main(int argc, char* argv[])
    { return EAMain(argc, argv); } 

#endif
