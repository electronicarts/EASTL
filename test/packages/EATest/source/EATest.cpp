/////////////////////////////////////////////////////////////////////////////
// EATest.cpp
//
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
    #pragma warning(disable: 4530 4548 4509)
    #pragma warning(disable: 6320)  // Exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER. 
#endif

#include <EATest/EATest.h>
#include <eathread/eathread.h>
#include <eathread/eathread_atomic.h>
#include <eathread/eathread_thread.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EADateTime.h>
#include <EAStdC/EAProcess.h>

#ifdef _MSC_VER
    #pragma warning(push, 0) // Microsoft headers generate warnings at the high warning levels we use.
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

#if defined(EA_PLATFORM_MICROSOFT)
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #include <Windows.h>
        extern "C" WINBASEAPI BOOL WINAPI IsDebuggerPresent();

    #if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) && !defined(EA_COMPILER_CLANG)
        #pragma comment(lib, "Advapi32.lib"); // For CheckTokenMembership and friends.
    #endif

#elif defined(__APPLE__)    // OS X, iPhone, iPad, etc.
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>
    #import <mach/mach.h>
    #import <mach/mach_host.h>
#elif defined(EA_PLATFORM_KETTLE)
    #include <unistd.h>
    #include <sys/types.h>
    #include <sdk_version.h>
    #if (SCE_ORBIS_SDK_VERSION >= 0x00930000u) // SDK 930+
        #include <libdbg.h>
    #endif

#elif defined(EA_PLATFORM_BSD)
    #include <sys/types.h>
    #include <sys/ptrace.h>
    #include <unistd.h> // For getuid, geteuid.
#elif defined(EA_HAVE_SYS_PTRACE_H)
    #include <unistd.h>
    #include <sys/ptrace.h>
#elif defined(EA_PLATFORM_ANDROID)
    #include <unistd.h>
    #include <android/log.h>
#elif defined(EA_PLATFORM_LINUX)
    #include <unistd.h> // For getuid, geteuid.
#endif

#if defined(EA_PLATFORM_UNIX)
    #include <stdlib.h> // For getenv.
#endif




namespace EA
{
namespace UnitTest
{
    namespace TestInternal
    {
 
        /// EATEST_VERIFY_IMP
        ///
        /// This is a helper function for EATEST_VERIFY, which itself is a macro for doing one-off
        /// standalone unit testing, that is, testing without Test objects and TestSuite systems.
        ///
        /// The user does not need nor want to put newline ('\n') characters at the 
        /// end of message strings as that will be done for you during the formatting 
        /// of the output. In particular, this function prints the output as per the 
        /// following format:
        ///     printf("%s(%d): %s.\n", fileName, fileLine, userMessage);
        /// 

        EATEST_API int EATEST_VERIFY_IMP(bool bExpression, int& nErrorCount, const char* pFile, int nLine, const char* pMessage)
        {
            if(!bExpression)
            {
                nErrorCount++;
                EA::EAMain::Report("%s(%d): %s\n", pFile, nLine, pMessage);
            }

            return bExpression ? 0 : 1;
        }


        EATEST_API int EATEST_VERIFY_F_IMP(bool bExpression, int& nErrorCount, const char* pFile, int nLine, const char* pFormat, ...)
        {
            if(!bExpression)
            {
                #if defined(EA_PLATFORM_DESKTOP)
                const int kBufferSize = 2048;
                #else
                const int kBufferSize = 512;
                #endif
                char buffer[kBufferSize];

                va_list arguments;
                va_start(arguments, pFormat);
                const int nReturnValue = EA::StdC::Vsnprintf(buffer, EAArrayCount(buffer), pFormat, arguments);
                #if defined(EA_COMPILER_VA_COPY_REQUIRED)
                    va_list argumentsSaved;
                    va_copy(argumentsSaved, arguments);
                #endif

                if(nReturnValue >= 0)
                    EATEST_VERIFY_IMP(bExpression, nErrorCount, pFile, nLine, buffer);
                else
                {
                    char* pBuffer = new char[nReturnValue + 1];

                    if(pBuffer)
                    {
                        #if defined(EA_COMPILER_VA_COPY_REQUIRED)
                            va_end(arguments);
                            va_copy(arguments, argumentsSaved);
                        #endif
                        EA::StdC::Vsnprintf(pBuffer, nReturnValue + 1, pFormat, arguments);
                        EATEST_VERIFY_IMP(bExpression, nErrorCount, pFile, nLine, pBuffer);
                        delete[] pBuffer;
                    }
                    else
                        EATEST_VERIFY_IMP(bExpression, nErrorCount, pFile, nLine, pFormat); // The format or user data is in error, so we currently echo it back to them.
                }

                va_end(arguments);
                #if defined(EA_COMPILER_VA_COPY_REQUIRED)
                    va_end(argumentsSaved);
                #endif
            }

            return bExpression ? 0 : 1;
        }


        // Used to implement VERIFY for the global error count. Recall that the global error count is used to
        // count errors that the application somehow can't count itself, such as if they are occurring in an
        // inaccessible thread.
        int EATEST_VERIFY_F_IMP_G(bool bExpression, const char8_t* pFormat, ...)
        {
            int nErrorCount = 0;

            if(!bExpression)
            {
                #if defined(EA_PLATFORM_DESKTOP)
                const int kBufferSize = 2048;
                #else
                const int kBufferSize = 512;
                #endif
                char buffer[kBufferSize];

                va_list arguments;
                va_start(arguments, pFormat);
                #if defined(EA_COMPILER_VA_COPY_REQUIRED)
                    va_list argumentsSaved;
                    va_copy(argumentsSaved, arguments);
                #endif

                const int nReturnValue = EA::StdC::Vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), pFormat, arguments);

                if(nReturnValue >= 0)
                    EATEST_VERIFY_IMP(bExpression, nErrorCount, __FILE__, __LINE__, buffer);
                else
                {
                    char* pBuffer = new char[nReturnValue + 1];

                    if(pBuffer)
                    {
                        #if defined(EA_COMPILER_VA_COPY_REQUIRED)
                            va_end(arguments);
                            va_copy(arguments, argumentsSaved);
                        #endif
                        EA::StdC::Vsnprintf(pBuffer, nReturnValue + 1, pFormat, arguments);
                        EATEST_VERIFY_IMP(bExpression, nErrorCount, __FILE__, __LINE__, pBuffer);
                        delete[] pBuffer;
                    }
                    else
                        EATEST_VERIFY_IMP(bExpression, nErrorCount, __FILE__, __LINE__, pFormat); // The format or user data is in error, so we currently echo it back to them.
                }

                IncrementGlobalErrorCount(nErrorCount);

                va_end(arguments);
                #if defined(EA_COMPILER_VA_COPY_REQUIRED)
                    va_end(argumentsSaved);
                #endif
            }

            return nErrorCount;
        }

    } // namespace TestInternal

    using namespace TestInternal;



///////////////////////////////////////////////////////////////////////////////
// ReportFunction
///////////////////////////////////////////////////////////////////////////////
EA::EAMain::ReportFunction gpReportFunction = EA::EAMain::GetDefaultReportFunction(); // ReportDefault is implemented in EAMain


///////////////////////////////////////////////////////////////////////////////
// Report Wrapper
EATEST_API void Report(const char8_t* pFormat, ...)
{
    va_list arguments;
    va_start(arguments, pFormat);
    EA::EAMain::VReport(pFormat, arguments);
    va_end(arguments);
}


///////////////////////////////////////////////////////////////////////////////
// ReportVerbosity Wrapper
EATEST_API void ReportVerbosity(unsigned minVerbosity, const char8_t* pFormat, ...)
{
    va_list arguments;
    va_start(arguments, pFormat);
    EA::EAMain::VReportVerbosity(minVerbosity, pFormat, arguments);
    va_end(arguments);
}


///////////////////////////////////////////////////////////////////////////////
// GetVerbosity Wrapper
EATEST_API unsigned GetVerbosity()
{
    return EA::EAMain::GetVerbosity();
}


///////////////////////////////////////////////////////////////////////////////
// WriteToEnsureFunctionCalled
//
int gWriteToEnsureFunctionCalled = 0;

EATEST_API int& WriteToEnsureFunctionCalled()
{
    return gWriteToEnsureFunctionCalled;
}


///////////////////////////////////////////////////////////////////////////////
// IsDebuggerPresent
//
EATEST_API bool IsDebuggerPresent()
{
    #if defined(EA_PLATFORM_MICROSOFT)
            return ::IsDebuggerPresent() != 0;

    #elif defined(EA_PLATFORM_KETTLE) && (SCE_ORBIS_SDK_VERSION >= 0x00930000u)
        return (sceDbgIsDebuggerAttached() != 0);

    #elif defined(__APPLE__) // OS X, iPhone, iPad, etc.
        // See http://developer.apple.com/mac/library/qa/qa2004/qa1361.html
        int               junk;
        int               mib[4];
        struct kinfo_proc info;
        size_t            size;

        // Initialize the flags so that, if sysctl fails for some bizarre reason, we get a predictable result.
        info.kp_proc.p_flag = 0;

        // Initialize mib, which tells sysctl the info we want, in this case we're looking for information about a specific process ID.
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PID;
        mib[3] = getpid();

        // Call sysctl.
        size = sizeof(info);
        junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
        //assert(junk == 0);

        // We're being debugged if the P_TRACED flag is set.
        return ((info.kp_proc.p_flag & P_TRACED) != 0);
    #elif defined(EA_HAVE_SYS_PTRACE_H)
        // See http://www.phiral.net/other/linux-anti-debugging.txt and http://linux.die.net/man/2/ptrace
        return (ptrace(PT_TRACE_ME, 0, 0, 0) < 0); // A synonym for this is PTRACE_TRACEME.

    #else
        return false;

    #endif
}



///////////////////////////////////////////////////////////////////////////////
// IsUserAdmin
//
EATEST_API bool IsUserAdmin()
{
    #if defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_BSD) || defined(EA_PLATFORM_APPLE) || defined(EA_PLATFORM_ANDROID)
        // https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man2/getuid.2.html
        // http://pubs.opengroup.org/onlinepubs/009695399/functions/geteuid.html 
        // http://pubs.opengroup.org/onlinepubs/009695399/functions/getuid.html
        uid_t id  = getuid(); // Returns the real user ID of the calling process..
        if(id == 0)
            return true;
        id = geteuid();       // Returns the effective user ID of the calling process.
        return (id == 0);

    #elif EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
        // http://msdn.microsoft.com/en-us/library/windows/desktop/aa376389%28v=vs.85%29.aspx
        // Returns TRUE if the caller's process is a member of the Administrators local group. 
        // The caller is not expected to be impersonating anyone but is expected to be able to
        // open its own process and process token. If it can't do that then it probably isn't 
        // running as Administrator anyway.
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        PSID administratorsGroup; 
        BOOL result = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroup);
 
        if(result) 
        {
            if(!CheckTokenMembership(NULL, administratorsGroup, &result)) 
                result = FALSE; 
            // Else leave result as TRUE.

            FreeSid(administratorsGroup); 
        }

        if(!result) // If the above determined that we aren't in the administrator's group...
        {
            // The user isn't part of the administrator's group. Let's see if the user is elevated.
            // To consider: It's not entirely clear if the being elevated in privilege is the same 
            // (for our purposes) as being in the administrator's group.
            HANDLE hToken;

            if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
            {
                TOKEN_ELEVATION elevation;
                DWORD cbSize = sizeof(TOKEN_ELEVATION);

                if(GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize))
                    result = elevation.TokenIsElevated;

                CloseHandle(hToken);
            }
        }

        return (result != FALSE); // Need to check for not FALSE rather than TRUE.
    #else
        return false;
    #endif
}



///////////////////////////////////////////////////////////////////////////////
// IsRunningUnderValgrind
//
bool IsRunningUnderValgrind()
{
    // Valgrind runs only on Unix platforms, and so we need test only on them.
    #if defined(EA_PLATFORM_UNIX)
        const char* pValue = getenv("RUNNING_ON_VALGRIND");
        if(pValue)
            return strcmp(pValue, "0") != 0;
    #endif

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// ThreadSleep
//
EATEST_API void ThreadSleep(float fTimeMilliseconds)
{
    EA::Thread::ThreadSleep((EA::Thread::ThreadTime)(int)fTimeMilliseconds);
}


///////////////////////////////////////////////////////////////////////////////
// ThreadSleepRandom
//
EATEST_API unsigned ThreadSleepRandom(unsigned nMinSleepMS, unsigned nMaxSleepMS, bool bVerboseOutput)
{
    using namespace EA::Thread;

    unsigned nSleepTime = nMinSleepMS;
    if(((int)nMaxSleepMS - (int)nMinSleepMS) > 0)
        nSleepTime += (rand() % (nMaxSleepMS - nMinSleepMS));

    ThreadTime nCurrentTime;
    ThreadTime nInitialTime         = GetThreadTime();
    ThreadTime nTimeEnd             = nInitialTime + nSleepTime;
    ThreadTime nLastReportTime      = nInitialTime;
    ThreadTime nThreadSleepDuration = 25;

    if(nSleepTime > 100)
        nThreadSleepDuration = nSleepTime / 4;

    if(nThreadSleepDuration > 2000)
        nThreadSleepDuration = 2000;

    while((nCurrentTime = GetThreadTime()) < nTimeEnd)
    {
        if((nCurrentTime - nLastReportTime) > 10000)
        {
            nLastReportTime = nCurrentTime;
            if(bVerboseOutput)
                EA::EAMain::Report(".\n");
        }

        EA::Thread::ThreadSleep(nThreadSleepDuration);
    }

    return nSleepTime;
}


///////////////////////////////////////////////////////////////////////////////
// GlobalErrorCount
//
EA::Thread::AtomicInt32 gGlobalErrorCount = 0;

EATEST_API int GetGlobalErrorCount()
{
    return gGlobalErrorCount.GetValue();
}

EATEST_API void SetGlobalErrorCount(int count)
{
    gGlobalErrorCount.SetValue((int32_t)count);
}

EATEST_API int IncrementGlobalErrorCount(int count)
{
    return (int)gGlobalErrorCount.Add((int32_t)count);
}

///////////////////////////////////////////////////////////////////////////////
// Verify
//
EATEST_API bool Verify(bool bValue, const char8_t* pMessage, Test* pTestContext)
{
    if(pTestContext)
        pTestContext->Verify(bValue, pMessage);
    else if(!bValue && gpReportFunction)
    {
        gpReportFunction(pMessage);
        gpReportFunction("\n");
    }

    return bValue;
}


///////////////////////////////////////////////////////////////////////
/// GetSystemTimeMicroseconds
///
/// To consider: Implement a GetSystemTime Nanoseconds function.
///
EATEST_API uint64_t GetSystemTimeMicroseconds()
{
    return EA::StdC::GetTime() / 1000;
}


///////////////////////////////////////////////////////////////////////////////
// MessageBoxAlert
//
EATEST_API void MessageBoxAlert(const char* pText, const char* pTitle)
{
    #if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
        MessageBoxA(0, pText, pTitle, MB_OK);
    #else
        // Possibly do something for other platforms here.
        EA_UNUSED(pText);
        EA_UNUSED(pTitle);
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// DisableOSMessageBoxAlerts
//
void DisableOSMessageBoxAlerts()
{
    #if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    #else
        // Possibly do something for other platforms here.
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// SetHighThreadPriority
//
EATEST_API void SetHighThreadPriority()
{
    #if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_MICROSOFT) && !(defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)) && !defined(EA_PLATFORM_WINDOWS_PHONE) 
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    #else
        EA::Thread::SetThreadPriority(EA::Thread::kThreadPriorityDefault + 1);
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// SetNormalThreadPriority
//
EATEST_API void SetNormalThreadPriority()
{
    #if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_MICROSOFT) && !(defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)) && !defined(EA_PLATFORM_WINDOWS_PHONE) 
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    #else
        EA::Thread::SetThreadPriority(EA::Thread::kThreadPriorityDefault);
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// SetLowProcessPriority
//
EATEST_API void SetLowProcessPriority()
{
    // Under Windows (which is where a lot of our unit testing is likely to be done), 
    // we might want to run at a lower priority so that this test can run for hours 
    // on-end in the background.
    #if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
        SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
    #else
        EA::Thread::SetThreadPriority(EA::Thread::kThreadPriorityDefault - 1);
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// EnableAlignmentExceptionDetection
//
// This is a wrapper for usage of Microsoft's SEM_NOALIGNMENTFAULTEXCEPT.
// This allows for detecting misaligned memory accesses.
// By design, Microsoft provides no way to disable it once enabled.
//
EATEST_API void EnableAlignmentExceptionDetection()
{
    #if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // Windows desktop platforms only.
        HMODULE kernelModule = ::GetModuleHandleA("kernel32.dll");
        if(kernelModule)
        {
            typedef UINT (WINAPI *PFNGETERRORMODE)(void);
            static PFNGETERRORMODE pfnGetErrorMode = 0;

            pfnGetErrorMode = (PFNGETERRORMODE)(::GetProcAddress(kernelModule, "GetErrorMode"));
            if(pfnGetErrorMode)
            {
                SetErrorMode(pfnGetErrorMode() | SEM_NOALIGNMENTFAULTEXCEPT);
            }
        }
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// NonInlinableFunction
//
EATEST_API EA_NO_INLINE void NonInlinableFunction()
{
    char buffer[2];
    sprintf(buffer, " ");
}


///////////////////////////////////////////////////////////////////////////////
// GetInteractive / SetInteractive
///////////////////////////////////////////////////////////////////////////////

bool gInteractive = false;

EATEST_API bool GetInteractive()
{
    return gInteractive;
}

EATEST_API void SetInteractive(bool interactive)
{
    gInteractive = interactive;
}



///////////////////////////////////////////////////////////////////////////////
// GetTestLevel / SetTestLevel
///////////////////////////////////////////////////////////////////////////////

int gTestLevel = kTestLevelDefault;

int GetTestLevel()
{
    return gTestLevel;
}

void SetTestLevel(int testLevel)
{
    gTestLevel = testLevel;
}



///////////////////////////////////////////////////////////////////////////////
// GetSystemSpeed / GetSystemMemoryMB
///////////////////////////////////////////////////////////////////////////////

EATEST_API float GetSystemSpeed(SpeedType speedType)
{
    // To consider: do some very basic startup benchmark.
    switch (speedType)
    {
        // Currently we boneheadedly just make gross assumptions about all types.
        case kSpeedTypeFPU:
        case kSpeedTypeCPU:
        case kSpeedTypeGPU:
        {
            #if   defined(EA_PLATFORM_DESKTOP)      // Generic desktop catchall, which includes Unix and OSX.
                return 1.0f;
            #elif defined(EA_PLATFORM_CONSOLE)
                return 0.2f;                        // Conservative estimate.
            #elif defined(EA_PLATFORM_MOBILE)
                return 0.2f;                        // Conservative estimate.

            #else
                return 0.15f;                       // Final catchall, with a fairly conservative guess.
            #endif
        }

        case kSpeedTypeDisk:
        {
            #if defined(EA_PLATFORM_DESKTOP)        // Generic desktop catchall, which includes Unix and OSX.
                return 1.0f;
            #elif defined(EA_PLATFORM_APPLE)        // iOS platform.
                return 0.2f;
            #elif defined(EA_PLATFORM_ANDROID)
                return 0.2f;
            #elif defined(EA_PLATFORM_MOBILE)
                return 0.2f;                        // Generic mobile catchall.
            #else
                return 0.2f;                         // Final catchall, with a fairly conservative guess.
            #endif
        }
    }

    return 1.f;
}


///////////////////////////////////////////////////////////////////////
/// GetSystemMemoryMB
///
/// Returns a measurement of usable system main memory (in megabytes), 
/// for the purpose of tuning tests that may require a lot of memory.
/// 
EATEST_API uint64_t GetSystemMemoryMB()
{
    #if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
        MEMORYSTATUSEX memoryStatusEx;

        memoryStatusEx.dwLength = sizeof(memoryStatusEx);
        GlobalMemoryStatusEx(&memoryStatusEx);

        return memoryStatusEx.ullTotalPhys / (1024 * 1024);

    #elif defined(EA_PLATFORM_APPLE)    // Any Apple platform that doesn't fall under the generic Unix above.
        host_basic_info_data_t hostinfo;
        mach_msg_type_number_t count     = HOST_BASIC_INFO_COUNT;
        kern_return_t          kr        = host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostinfo, &count);

        if(kr == KERN_SUCCESS)
            return (uint64_t)hostinfo.max_mem / (1024 * 1024);

        return 1024;

    #elif defined(EA_PLATFORM_UNIX) && defined(_SC_PHYS_PAGES) && defined(_SC_PAGE_SIZE)
        // This  covers at least Linux, Apple, Android, Palm.
        const long pageCount = sysconf(_SC_PHYS_PAGES);
        const long pageSize  = sysconf(_SC_PAGE_SIZE);

        return (uint64_t)(pageCount * pageSize) / (1024 * 1024);

    #elif defined(EA_PLATFORM_PS4)      // As of SDK 1.6 (May 2014), the normal max direct memory available to applications is 4.5 GiB. On dev kits the max direct memory available is 5.25 GiB when enabled in debug settings.
        return 4096;
    #elif defined(EA_PLATFORM_DESKTOP)  // Generic desktop catchall, which includes Unix and OSX.
        return 2048;
    #elif defined(EA_PLATFORM_MOBILE)
        return  128;                    // Generic mobile catchall.
    #else
        return  200;                    // Final catchall, with a fairly conservative guess.
    #endif
}



///////////////////////////////////////////////////////////////////////////////
// Rand
///////////////////////////////////////////////////////////////////////////////

uint32_t gRandSeed = 0;

EATEST_API uint32_t GetRandSeed()
{
    return gRandSeed;
}

EATEST_API void SetRandSeed(uint32_t seed)
{
    gRandSeed = seed;
}

///////////////////////////////////////////////////////////////////////////////
// Test
///////////////////////////////////////////////////////////////////////////////

Test::Test(const char8_t* pTestName, EA::EAMain::ReportFunction pReportFunction)
  : msTestName(pTestName), 
    mpParentSuite(NULL),
    mnSuccessCount(0),
    mnErrorCount(0),
    mpReportFunction(pReportFunction),
    mbForceReport(false)
{
    // Empty
}

Test::~Test()
{
    // Empty
}


int Test::Init()
{
    return kTestResultOK;
}


int Test::Shutdown()
{
    return kTestResultOK;
}


TestSuite* Test::GetParentSuite() const
{
    return mpParentSuite;
}


EA::EAMain::ReportFunction Test::GetReportFunction() const
{
    // We walk up the test hierarchy and try to find a report function.
    const Test* pTest = this;
    EA::EAMain::ReportFunction pReportFunction = mpReportFunction;

    while(!pReportFunction)
    {
        pTest = pTest->mpParentSuite;

        if(pTest)
            pReportFunction = pTest->mpReportFunction;
        else
            break;
    }

    if(!pReportFunction)
        pReportFunction = gpReportFunction;

    return pReportFunction;
}


void Test::GetName(eastl::string& sName) const
{
    sName.assign(msTestName.data(), msTestName.length());
}


int Test::Run()
{
    // Normally you need to subclass this function.
    WriteReport();
    return kTestResultOK;
}


bool Test::Verify(bool bValue, const char8_t* pMessage)
{
    if(!bValue)
    {
        mnErrorCount++;

        if(gpReportFunction)
        {
            // Print out a string like this:
            //     MathTest: Failure of quaternion initialization.
            gpReportFunction(msTestName.c_str());
            gpReportFunction(": ");
            gpReportFunction(pMessage);
            gpReportFunction("\n");
        }
        return false;
    }

    mnSuccessCount++;
    return true;
}


bool Test::VerifyFormatted(bool bValue, const char8_t* pFormat, ...)
{
    if(!bValue)
    {
        char buffer[384];

        va_list arguments;
        va_start(arguments, pFormat);
        const int nReturnValue = EA::StdC::Vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), pFormat, arguments);
        va_end(arguments);

        if((nReturnValue >= 0) && (nReturnValue < (int)(sizeof(buffer)/sizeof(buffer[0]))))
            return Verify(bValue, buffer);
        else
            return Verify(bValue, pFormat); // We didn't have enough space, but we can print this.
    }
    return true;
}


void Test::WriteReport()
{
    // If we have a parent suite, we depend on it to write the 
    // report for us, so that they are all grouped together.
    if(!mpParentSuite || mbForceReport)
    {
        char buffer[384];
        EA::EAMain::ReportFunction pReportFunction = GetReportFunction();
        EA::StdC::Sprintf(buffer, "%-24s - %s\n", msTestName.c_str(), mnErrorCount ? "FAILED" : "PASSED");
        pReportFunction(buffer);
    }
}


///////////////////////////////////////////////////////////////////////////////
// TestFunction
///////////////////////////////////////////////////////////////////////////////

TestFunction::TestFunction(const char8_t* pTestName, FunctionPtr pFunction)
    : Test(pTestName), mpFunction(pFunction)
{
    // Empty
}


int TestFunction::Run()
{
    int nTestResult = kTestResultOK;

    if(mpFunction)
    {
        #ifdef _MSC_VER
            __try {
                nTestResult = (*mpFunction)();
            }
            __except(true) {
                nTestResult = kTestResultError;
                VerifyFormatted(false, "Unhandled exception in test %s.\n", msTestName.c_str());
            }
        #else
            nTestResult = (*mpFunction)();
        #endif

        if((nTestResult != kTestResultOK) && (nTestResult != kTestResultContinue) && (nTestResult != kTestResultContinue))
            mnErrorCount++;
        else
            mnSuccessCount++;
    }

    WriteReport();

    return nTestResult;
}



///////////////////////////////////////////////////////////////////////////////
// TestCollection
///////////////////////////////////////////////////////////////////////////////

TestCollection::TestCollection()
  : mTests()
{
    // Empty
}


TestCollection::~TestCollection()
{
    while(!mTests.empty())
        RemoveTest((*mTests.rbegin()).mpTest, true);
}


bool TestCollection::AddTest(Test* pTest, bool bTakeOwnership)
{
    const TestInfo testInfo = { pTest, bTakeOwnership };

    // We could use the standard 'find' algorithm with a special comparison, but what we do here is fine.
    for(TestArray::iterator it(mTests.begin()); it != mTests.end(); ++it)
    {
        TestInfo& temp = *it;
        if(temp.mpTest == pTest)
            return false; // Already present
    }

    mTests.push_back(testInfo);
    return true;
}


void TestCollection::AddTest(const char8_t* pTestName, TestFunction::FunctionPtr pFunction)
{
    // The user may want to avoid the use of global memory allocation here and instead 
    // use the general AddTest function with their own supplied TestFunction object.
    TestFunction* const pTF = new TestFunction(pTestName, pFunction);
    AddTest(pTF, true);
}


void TestCollection::AddTests(const TestCollection *pCollection)
{
    const eastl_size_t nTotalTests = (eastl_size_t)pCollection->EnumerateTests(NULL, 0);

    if(nTotalTests)
    {
        eastl::vector<Test*> tests(nTotalTests);
        pCollection->EnumerateTests(tests.data(), nTotalTests);

        for(eastl_size_t i = 0; i < nTotalTests; ++i)
            AddTest(tests[i]);
    }
}


bool TestCollection::RemoveTest(Test* pTest, bool bDeleteIfOwned)
{
    for(TestArray::iterator itA(mTests.begin()); itA != mTests.end(); ++itA)
    {
        TestInfo& testInfoA = *itA;

        if(testInfoA.mpTest == pTest)
        {
            if(bDeleteIfOwned && testInfoA.mbOwned)
                delete testInfoA.mpTest;

            mTests.erase(itA);
            return true;
        }
    }

    return false;
}


bool TestCollection::RemoveTest(const char8_t* pTestName, bool bDeleteIfOwned)
{
    eastl::string sName;

    for(TestArray::iterator it(mTests.begin()); it != mTests.end(); ++it)
    {
        TestInfo& testInfo = *it;

        testInfo.mpTest->GetName(sName);
        if(sName == pTestName)
            return RemoveTest(testInfo.mpTest, bDeleteIfOwned); // Not extremely efficient, but it doesn't need to be.
    }

    return false;
}


Test* TestCollection::FindTest(const char8_t* pTestName)
{
    TestInfo* const pTestInfo = FindTestInfo(pTestName, true);
    return pTestInfo ? pTestInfo->mpTest : NULL;
}


TestCollection::TestInfo* TestCollection::FindTestInfo(const char8_t* pTestName, bool bRecursive)
{
    eastl::string sNameCurrent;

    // Check each of our child tests directly against pTestName.
    for(TestArray::iterator it(mTests.begin()); it != mTests.end(); ++it)
    {
        TestInfo& testInfo = *it;
        testInfo.mpTest->GetName(sNameCurrent);

        if(sNameCurrent.comparei(pTestName) == 0)
            return &testInfo;
    }

    // At this point, none of the children matched.
    if(bRecursive) // If we allow checking child tests...
    {
        // None of the tests matched pTestName, so we check to see if any
        // of our tests were test suites and if the given pName was referring
        // to a child test of one of our tests.
        const char8_t* pSeparator = strchr(pTestName, '/');

        if(pSeparator)
        {
            // In this case, pTestName was passed in as a name like "Math Suite/Vector Test"
            // and we may have a child test called "Math Suite" which itself is a suite
            // and has a child test called "Vector Test". So we recursively call ourself.
            sNameCurrent.assign(pTestName, (eastl_size_t)(pSeparator - pTestName));

            TestInfo* const pTestInfo = FindTestInfo(sNameCurrent.c_str(), false); // Use 'false' here to try to match just "Math Suite", for example.

            if(pTestInfo)
            {
                // ** Bug ** Bug ** Bug **
                // We need to do a downcast from Test to TestSuite only if we
                // can be sure the Test is a TestSuite. We don't have such an 
                // RTTI mechanism now but need to add it. enum TestType?
                TestSuite* const pTestSuite = static_cast<TestSuite*>(pTestInfo->mpTest);

                return pTestSuite->FindTestInfo(pSeparator + 1, true); // pSeparator now points to something like "Vector Test", for example.
            }
        }
    }

    return NULL;
}


size_t TestCollection::EnumerateTests(Test* pTestArray[], size_t nTestArrayCapacity) const
{
    if(pTestArray)
    {
        if(nTestArrayCapacity > mTests.size())
            nTestArrayCapacity = mTests.size();

        memset(pTestArray, 0, nTestArrayCapacity * sizeof(Test*));

        for(eastl_size_t i = 0; i < nTestArrayCapacity; ++i)
            pTestArray[i] = mTests[i].mpTest;
    }

    return mTests.size();
}



///////////////////////////////////////////////////////////////////////////////
// TestSuite
///////////////////////////////////////////////////////////////////////////////

TestSuite::TestSuite(const char8_t* pTestName)
  : Test(pTestName),
    TestCollection(),
    mnTestResult(kTestResultNone),
    mResults()
{
    // Empty
}


TestSuite::~TestSuite()
{
    Shutdown();
}


// This is a separate function because we use __try/__except, which has limitations on
// how it can be used and requires us to put it in an independent function.
void TestSuite::Run(ResultInfo& resultInfo)
{
    int nErrorCount = 0;

    #ifdef _MSC_VER
        __try
    #endif
        {
            // If this is a new test (and not a continuation), initialize it.
            // Initialization failures are considered test failures.
            if(resultInfo.mnResult == kTestResultNone)
            {
                // Really I would like to report at level 1 so this trace does not
                // appear by default but all users trace is already at 1 and 
                // would need changing as well.
                EA::EAMain::ReportVerbosity(0, "Running test [%s]\n", resultInfo.mpTest->msTestName.c_str());

                if(SetupTest() != kTestResultOK)
                {
                    resultInfo.mnResult = kTestResultError;
                    EATEST_VERIFY_F(false, "Setup failure for test %s.\n", resultInfo.mpTest->msTestName.c_str());
                }

                if(resultInfo.mpTest->Init() != kTestResultOK)
                {
                    resultInfo.mnResult = kTestResultError;
                    EATEST_VERIFY_F(false, "Initialization failure in test %s.\n", resultInfo.mpTest->msTestName.c_str());
                }
            }

            // As long as we're not in an error state, we can now run the
            // test itself.
            if(resultInfo.mnResult != kTestResultError)
                resultInfo.mnResult = resultInfo.mpTest->Run();

            if(resultInfo.mnResult != kTestResultContinue)
            {
                if(resultInfo.mpTest->Shutdown() != kTestResultOK)
                {
                    resultInfo.mnResult = kTestResultError;
                    EATEST_VERIFY_F(false, "Shutdown failure in test %s.\n", resultInfo.mpTest->msTestName.c_str());
                }

                if(TeardownTest() != kTestResultOK)
                {
                    resultInfo.mnResult = kTestResultError;
                    EATEST_VERIFY_F(false, "Teardown failure for test %s.\n", resultInfo.mpTest->msTestName.c_str());
                }
            }
        }
    #ifdef _MSC_VER
        __except(true) {
            resultInfo.mnResult = kTestResultError;
            EATEST_VERIFY_F(false, "Unhandled exception in test %s.\n", resultInfo.mpTest->msTestName.c_str());
        }
    #endif

    resultInfo.mpTest->mnErrorCount += nErrorCount;
}


int TestSuite::Run()
{
    bool bContinueRequested = false;

    #if EATEST_ALIGNMENT_EXCEPTION_DETECTION_CALLED
        EnableAlignmentExceptionDetection();
    #endif

    // Set the test result to OK for now.
    mnTestResult = kTestResultOK;

    for(ResultArray::iterator it(mResults.begin()); it != mResults.end(); ++it)
    {
        ResultInfo& resultInfo = *it;

        // If we already have a result for this test, we don't need to run it again.
        if(resultInfo.mnResult != kTestResultNone)
            continue;

        Run(resultInfo);

        if(resultInfo.mnResult == kTestResultContinue)
        {
            bContinueRequested = true;

            // We might want to sleep here in order to yield the processor to the test(s).
            ThreadSleep(100);
        }
        else // Else the test is complete.
        {
            if(resultInfo.mnResult >= kTestResultError)
                mnTestResult = kTestResultError;
        }
    }

    // If we still have some tests that want to run, signal a continuation.
    if(bContinueRequested)
        return kTestResultContinue;

    WriteReport();

    // EA_ASSERT(mnTestResult != kTestResultContinue);
    return mnTestResult;
}


int TestSuite::RunTest(const char8_t* pName)
{
    TestInfo* const pTestInfo = FindTestInfo(pName, true);
    if(!pTestInfo)
    {
        EA::EAMain::Report("Error: Unable to recognize test %s\n", pName);
        return kTestResultError;
    }

    // Find the matching result info for this test.
    ResultInfo* pResultInfo = NULL;
    for(ResultArray::iterator it(mResults.begin()); it != mResults.end(); ++it)
    {
        ResultInfo& resultInfo = *it;
        if(resultInfo.mpTest == pTestInfo->mpTest)
        {
            pResultInfo = &resultInfo;
            break;
        }
    }

    if(pResultInfo)
    {
        // Note that we don't deal with the case whereby the test result
        // is kTestResultContinue. Until we need that functionality we will
        // call it a known bug.
        Run(*pResultInfo);
        return pResultInfo->mnResult;
    }

    return kTestResultError;
}


int TestSuite::GetTestResult() const
{
    return mnTestResult;
}


bool TestSuite::AddTest(Test* pTest, bool bTakeOwnership)
{
    if(!TestCollection::AddTest(pTest, bTakeOwnership))
        return false;

    pTest->mpParentSuite = this;

    const ResultInfo resultInfo = { pTest, kTestResultNone };
    mResults.push_back(resultInfo);

    return true;
}


void TestSuite::AddTest(const char8_t* pTestName, TestFunction::FunctionPtr pFunction)
{
    TestCollection::AddTest(pTestName, pFunction);
}


bool TestSuite::RemoveTest(Test* pTest, bool bDeleteIfOwned)
{
    // Remove it from mTestArrayRunning if present. 
    for(ResultArray::iterator it(mResults.begin()); it != mResults.end(); ++it)
    {
        ResultInfo& runInfo = *it;

        if(runInfo.mpTest == pTest)
        {
            mResults.erase(it);
            break;
        }
    }

    pTest->Shutdown();

    return TestCollection::RemoveTest(pTest, bDeleteIfOwned);
}

bool TestSuite::RemoveTest(const char8_t* pTestName, bool bDeleteIfOwned)
{
    return TestCollection::RemoveTest(pTestName, bDeleteIfOwned);
}

void TestSuite::WriteReport()
{
    // If we have a parent suite, we depend on it to write the 
    // report for us, so that they are all grouped together.
    if(!mpParentSuite || mbForceReport)
    {
        char buffer[384];
        EA::EAMain::ReportFunction pReportFunction = GetReportFunction();

        // Print header
        const size_t kMaxHeaderWidth = 70;
        const size_t kBaseHeaderWidth = msTestName.length() + 17;

        EA::StdC::Sprintf(buffer, "\n-- Test Suite: %s ", msTestName.c_str());
        memset(buffer + kBaseHeaderWidth, '-', kMaxHeaderWidth - kBaseHeaderWidth + 1);
        buffer[kMaxHeaderWidth + 1] = '\n';
        buffer[kMaxHeaderWidth + 2] = 0;

        pReportFunction(buffer);

        // Print test results
        for(ResultArray::iterator it(mResults.begin()); it != mResults.end(); ++it)
        {
            ResultInfo& resultInfo = *it;
            Test* const pTest  = resultInfo.mpTest;

            if(resultInfo.mnResult == kTestResultNone)
            {
                eastl::string sName;
                pTest->GetName(sName);
                EA::EAMain::Report("Test not run: %s\n", sName.c_str());
            }
            else
            {
                const bool bForceReportSaved(pTest->mbForceReport);
                pTest->mbForceReport = true;
                pTest->WriteReport();
                mbForceReport = bForceReportSaved;
            }
        }

        // Print footer
        size_t i(0);
        for(i = 0; i < kMaxHeaderWidth; i++)
            buffer[i] = '-'; // Buffer overflow checking not done here.
        buffer[i++] = '\n';
        buffer[i] = 0;
        pReportFunction(buffer);
    }
}

int TestSuite::SetupTest()
{
    return kTestResultOK;
}

int TestSuite::TeardownTest()
{
    return kTestResultOK;
}


///////////////////////////////////////////////////////////////////////////////
// TestApplication
///////////////////////////////////////////////////////////////////////////////

TestApplication::TestApplication(const char8_t* pTestApplicationName, int argc, char** argv, FunctionPtr pInitFunction, FunctionPtr pShutdownFunction)
  : TestSuite(pTestApplicationName),
    mArgc(argc),
    mArgv(argv),
    mpInitFunction(pInitFunction),
    mpShutdownFunction(pShutdownFunction)
{
}


TestApplication::~TestApplication()
{
}


void TestApplication::SetArg(int argc, char** argv)
{
    mArgc = argc;
    mArgv = argv;
}


void TestApplication::SetForceReport(bool bReport)
{
    mbForceReport = bReport;
}


int TestApplication::Init()
{
    if(TestSuite::Init() < 0)
        return kTestResultError;

    if(mpInitFunction)
    {
        const int nTestResult = (*mpInitFunction)();
        if (nTestResult < 0)
            return kTestResultError;
    }

    return kTestResultOK;
}


int TestApplication::Shutdown()
{
    if(mpShutdownFunction)
    {
        const int nTestResult = (*mpShutdownFunction)();

        if(nTestResult < 0)
            return kTestResultError;
    }

    return TestSuite::Shutdown();
}


int TestApplication::Run()
{
    EA::EAMain::CommandLine  commandLine(mArgc, mArgv);
    const char *             sResult = NULL;
    int                      nTestResult = 0;              // This will hold a value of 0 (kResultResultOK) or greater (error).
    size_t                   nTestCount  = 0;
    bool                     bReport     = mbForceReport;
    bool                     bWait       = false;

    // Example usage: -?
    if(commandLine.HasHelpSwitch())
    {
        PrintUsage();
    }

    // Example usage: -v
    if((commandLine.FindSwitch("-verbose", false, &sResult, 0) >= 0) || 
       (commandLine.FindSwitch("-v",       false, &sResult, 0) >= 0))
    {
        SetForceReport(true);

        if(EA::StdC::Strlen(sResult) > 0)
            EA::EAMain::SetVerbosity(EA::StdC::AtoU32(sResult));
    }

    // Example usage: -debugWait
    // An alternative to this is -debugBreak, though you can combine them to cause the 
    // debugger to break right after it's attached (assuming the time loop below doesn't
    // accomplish that already for the given platform/debugger).
    if(commandLine.FindSwitch("-debugWait", false, NULL, 0) >= 0)
    {
        // We run in a loop waiting for the user to attach a debugger.
        bool            debuggerPresent = IsDebuggerPresent();
        uint64_t        tNow  = GetSystemTimeMicroseconds();
        uint64_t        tPrev = tNow;
        const uint64_t  kPauseTimeUs = 1000000; // 1 second
        bool            bPrintfOccurred = false;

        // We break out of the loop if the debugger appears to now be present or if 
        // the process pauses for a long time (indicating that the attachment of a 
        // debugger resulted in the process being stopped during the loop below).
        while(!debuggerPresent && ((tNow - tPrev) < kPauseTimeUs)) 
        {
            tPrev = tNow;

            if(!bPrintfOccurred)
            {
                EA::EAMain::Report("Waiting for debugger to attach...\n");
                bPrintfOccurred = true;
            }

            ThreadSleep(100); // 100 milliseconds.
            debuggerPresent = IsDebuggerPresent();
            tNow = GetSystemTimeMicroseconds();
        }

        // If the user wants a debug break to occur here, the user should supply 
        // the -debugBreak argument as it will be processed next.
    }

    // Example usage: -debugBreak
    // An alternative to this is -debugWait, though you can combine them to cause the 
    // debugger to break right after it's attached (assuming the time loop in debugWait 
    // handling doesn't accomplish that already for the given platform/debugger).
    if(commandLine.FindSwitch("-debugBreak", false, NULL, 0) >= 0)
    {
        // The handling of this argment is intentionally after the -debugWait argument.
        // On platforms that don't support a formal debugger break, EATEST_DEBUG_BREAK
        // will loop forever or intentionally create an exception, depending on the platform.
        EATEST_DEBUG_BREAK();
    }

    // Example usage: -interactive
    if((commandLine.FindSwitch("-interactive", false, NULL, 0) >= 0) || 
       (commandLine.FindSwitch("-i",           false, NULL, 0) >= 0))
    {
        SetInteractive(true);
    }

    // Example usage: -testLevel:70
    if((commandLine.FindSwitch("-testLevel", false, &sResult, 0) >= 0) && EA::StdC::Strlen(sResult) > 0)
    {
        if(EA::StdC::Stricmp(sResult, "low") == 0)
            SetTestLevel(kTestLevelLow);
        else if(EA::StdC::Stricmp(sResult, "high") == 0)
            SetTestLevel(kTestLevelHigh);
        else if(EA::StdC::Stricmp(sResult, "default") == 0)
            SetTestLevel(kTestLevelDefault);
        else
        {
            const int32_t testLevel = EA::StdC::AtoI32(sResult);
            SetTestLevel((int)testLevel);
        }
    }

    // Example usage: -wait
    if((commandLine.FindSwitch("-wait", false, NULL, 0) >= 0) || 
       (commandLine.FindSwitch("-w",    false, NULL, 0) >= 0))
    {
        bWait = true;
    }

    // Example usage: -r:23456
    if(((commandLine.FindSwitch("-randSeed", false, &sResult, 0) >= 0) || 
        (commandLine.FindSwitch("-r",        false, &sResult, 0) >= 0)) &&
        EA::StdC::Strlen(sResult) > 0)
    {
        const uint32_t seed = EA::StdC::AtoU32(sResult);
        SetRandSeed(seed);
    }

    // If the user passes in -list or -listDetail, then we write to our 
    // standard output a list of all tests which can be run.
    // Example usage: -listDetail
    if(commandLine.FindSwitch("-listDetail",  false, NULL, 0) >= 0)
        PrintTestNames(true);
    else if(commandLine.FindSwitch("-list",  false, NULL, 0) >= 0)
        PrintTestNames(false);

    if(Init() < 0)
    {
        nTestResult = kTestResultError;
        bReport = true;
        goto Cleanup;
    }

    // If the user doesn't supply any -run (or its pseudonym -test), then we run all tests.
    // Example usage: -run:StackAllocator
    if((commandLine.FindSwitch("-run",  false, NULL, 0) < 0) &&
       (commandLine.FindSwitch("-test", false, NULL, 0) < 0)) // If no -run argument is specified....
    {
        nTestCount  += mTests.size();
        nTestResult += TestSuite::Run();
    }
    else
    {
        // Run a specific test. Multiple tests are allowed, same test run multiple times is allowed.
        const char *sTestName;

        int nArgIndex = commandLine.FindSwitch("-run",  false, &sTestName, 0);
        if(nArgIndex < 0)
            nArgIndex = commandLine.FindSwitch("-test", false, &sTestName, 0);

        while(nArgIndex >= 0) 
        {
            nArgIndex++;

            // If we have an empty switch - simply run the entire test suite
            if(EA::StdC::Strlen(sTestName) == 0)
            {
                nTestCount  += mTests.size();
                nTestResult += TestSuite::Run();  // This will return the sum of errors from all tests in the suite.
                break;
            }
            else // Else just run a single test.
            {
                const int result = RunTest(sTestName);
                if(result != kTestResultOK)
                    nTestResult++;
                bReport = true;
                nTestCount++;
            }

            // We have to do the logic below like so because nArgIndex is used as both an argument and a return value to FindSwitch().
            int nArgIndexNew = commandLine.FindSwitch("-run",  false, &sTestName, nArgIndex);
            if(nArgIndexNew < 0)
                nArgIndexNew = commandLine.FindSwitch("-test", false, &sTestName, nArgIndex);
            nArgIndex = nArgIndexNew;
        }
    }

    // Add in the global error count to nTestResult;
    nTestResult += GetGlobalErrorCount();

Cleanup:
    if(Shutdown() < 0)
    {
        nTestResult += kTestResultError;
        bReport      = true;
    }

    // Report on all the tests we run.
    if(bReport)
    {
        EA::EAMain::Report("\n");
        EA::EAMain::Report("Tests are complete.\n");
        EA::EAMain::Report("Run count: %d.\n", nTestCount);
        EA::EAMain::Report("Result: %s (code %d).\n", (nTestResult == kTestResultOK) ? "success" : "failure", nTestResult);
    }
    else if(nTestCount == 0)
        EA::EAMain::Report("Run count: 0 (nothing was run).\n");

    if(bWait)
    {
        EA::EAMain::Report("\nPress any key to exit.\n");
        char c = (char)getchar();
        EA_UNUSED(c);
    }

    return nTestResult;
}


void TestApplication::PrintUsage()
{
    eastl::string sTestName1("MyTest1"), sTestName2("MyTest2");
    eastl::string sAppName("Tests.exe");

    if(mTests.size() >= 1)
        mTests[0].mpTest->GetName(sTestName1); 
    if(mTests.size() >= 2)
        mTests[1].mpTest->GetName(sTestName2); 

    EA::EAMain::Report("%s\n\n", msTestName.c_str());
    EA::EAMain::Report("Available arguments:\n");
    EA::EAMain::Report("    -help / -h / -?         Displays this usage information\n");
    EA::EAMain::Report("    -list                   Displays a list of available tests\n");
    EA::EAMain::Report("    -listDetail             Displays a detailed list of available tests\n");
    EA::EAMain::Report("    -run:<TestName>         Runs a specific test\n");
    EA::EAMain::Report("    -verbose / -v           Causes all debug traces to be visible, including those that are merely informational.\n");
    EA::EAMain::Report("    -interactive / -i       Sets the test as being an attended run as opposed to being automated and unattended.\n");
    EA::EAMain::Report("    -randSeed / -r:<value>  Sets the global random number seed value which test code can use.\n");
    EA::EAMain::Report("    -debugBreak             Causes EATEST_DEBUG_BREAK to immediately be called. Useful for\n");
    EA::EAMain::Report("                            debugging in some situations. Can be combined with -debugWait.\n");
    EA::EAMain::Report("    -debugWait              Causes the app to loop before proceeding, while waiting for a\n");
    EA::EAMain::Report("                            debugger to attach. Can be combined with -debugBreak.\n");
    EA::EAMain::Report("    -wait / -w              Waits for user confirmation before app exit. Shouldn't be used if the\n");
    EA::EAMain::Report("                            run is unattended (i.e. non-interactive)\n");
    EA::EAMain::Report("\n");
    EA::EAMain::Report("Example usage:\n");
    EA::EAMain::Report("    %s -list -wait -randSeed:12345\n", sAppName.c_str());
    EA::EAMain::Report("    %s -run:%s -run:%s\n", sAppName.c_str(), sTestName1.c_str(), sTestName2.c_str());
    EA::EAMain::Report("    %s -run:%s -run:%s -verbose -i\n", sAppName.c_str(), sTestName1.c_str(), sTestName2.c_str());
    EA::EAMain::Report("\nAvailable tests:\n");
    PrintTestNames(true);
    EA::EAMain::Report("\n");
}


void TestApplication::PrintTestNames(bool /*bDetail*/)
{
    eastl::string sName;

    // To do: We need top support the bDetailArgument. In doing so, 
    // we may want to have them print using our '/' notation to denote the 
    // hierarchy (e.g. "Math Suite/Vector Test")
    // To do this, we'd need to identify pTest as a TestSuite and 
    // recursively dive into its hierarchy, using '/' to separate
    // entities much like a file system path. Example output:
    //     Audio Suite/Setup Test
    //     Audio Suite/Sound Test/Beep Test
    //     Audio Suite/Sound Test/Bot Test
    //     Math Suite/Vector Test
    //     Math Suite/Matrix Test
    //     Math Suite/Quaternion Test

    for(eastl_size_t i = 0, iEnd = mTests.size(); i < iEnd; ++i)
    {
        Test* const pTest = mTests[i].mpTest;

        pTest->GetName(sName);
        EA::EAMain::Report("   %s\n", sName.c_str());
    }
}



EATEST_API TestCollection& GetRegistry()
{
    static TestCollection sRegistry;
    return sRegistry;
}

} // namespace UnitTest
} // namespace EA
