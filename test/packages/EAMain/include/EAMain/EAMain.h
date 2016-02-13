///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EAMAIN_H
#define EAMAIN_H

namespace EA
{
    namespace EAMain
    {
		namespace Internal
		{
			static unsigned gVerbosityLevel = 0;	
		};

        typedef void (*ReportFunction)(const char8_t*);

        inline unsigned GetVerbosity() { return Internal::gVerbosityLevel; }
        inline void SetVerbosity(unsigned level) { Internal::gVerbosityLevel = level; }

        inline ReportFunction GetDefaultReportFunction() 
		{ 
			return static_cast<ReportFunction>([](const char8_t* pOutput)
				{ printf("%s", pOutput); }); 
		}

        inline void VReport(const char8_t* pFormat, va_list arguments) 
		{ 
			vprintf(pFormat, arguments);
		}

		inline void Report(const char8_t* pFormat, ...) 
		{    
			va_list args;
			va_start(args, pFormat);
			VReport(pFormat, args);
			va_end(args);
		}

        inline void VReportVerbosity(unsigned minVerbosity, const char8_t* pFormat, va_list arguments) 
		{ 
			VReport(pFormat, arguments);
		}

        inline void ReportVerbosity(unsigned minVerbosity, const char8_t* pFormat, ...) 
		{ 
			va_list args;
			va_start(args, pFormat);
			VReportVerbosity(minVerbosity, pFormat, args);
			va_end(args);
		}

		static const char DEFAULT_DELIMITER = ':';
		static const int MAX_COMMANDLINE_ARGS = 128;

		class CommandLine 
		{
		public:

            CommandLine(int argc, char** argv) {}
			explicit CommandLine(const char *commandLineString) { }
            CommandLine(const char *commandLineString, unsigned int flags) { }

            int FindSwitch(const char *pSwitch, bool bCaseSensitive = false, const char **pResult = NULL, int nStartingIndex = 0, char delimiter = DEFAULT_DELIMITER) const { return -1; }

            bool HasHelpSwitch() const { return false; }
		};
	}
}

#endif
