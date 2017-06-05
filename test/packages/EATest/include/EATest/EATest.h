/////////////////////////////////////////////////////////////////////////////
// EATest.h
//
// Copyright (c) Electronic Arts Inc. All rights reserved.
//
// This is currently based heavily on Java JUnit, Boost Unit testing,
// the Unit module in use by some within EAC, and Maxis Unit testing.
// Each of these three systems has had something to contribute to
// the design.
//
/////////////////////////////////////////////////////////////////////////////
// Example Usage #1
//
// This is for very simple one-off test functions that don't need or 
// want to use the Test class hierarchy.
//     int MathTest(){
//         int errorCount = 0;
//         errorCount += Verify(1 < 2, "Comparison test failure");
//         errorCount += Verify(2 < 3, "Comparison test failure");
//         return errorCount;
//     }
// 
/////////////////////////////////////////////////////////////////////////////
// Example Usage #2
//
// This is a more fancy example which uses the Test class to run a test.
//     class MathTest : public Test
//     {
//         int Run()
//         {
//             Verify(1 < 2, "Comparison test failure");
//             return mnErrorCount ? kTestResultError : kTestResultOK;
//         }
//     };
//
//     MathTest mathTest("Test via MathTest class");
//     mathTest.Run();
// 
/////////////////////////////////////////////////////////////////////////////
// Example Usage #3
//
// This is the most fancy you can get, as it uses a TestSuite.
//     MathTest   mathTest;
//     StringTest stringTest;
//     TestSuite  testSuite("Test suite");
//
//     testSuite.AddTest(&mathTest);
//     testSuite.AddTest(&stringTest);
//     testSuite.Run();
// 
/////////////////////////////////////////////////////////////////////////////


#ifndef EATEST_H
#define EATEST_H


#ifdef _MSC_VER
    #pragma warning(push, 0)        // Microsoft headers generate warnings at our higher warning levels.
    #pragma warning(disable: 4530)  // C++ exception handler used, but unwind semantics are not enabled.
    #pragma warning(disable: 4548)  // Expression before comma has no effect; expected expression with side-effect.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


#include <EATest/internal/Config.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <stddef.h>
#include <limits.h>
#include "EAMain/EAMain.h"

///////////////////////////////////////////////////////////////////////////////
/// EATEST_DEBUG_BREAK_ENABLED
///
/// Defined as 0 or 1. Default is 1 regardless of whether this is debug or release build.
/// This controls whether debugger breaks are enabled. Caller must explicitly
/// enable breaks in DebugRelease and Release builds to have asserts drop
/// into the debugger.  Breaks cause an exception to occur when a debugger is not 
/// present so only use this to debug DebugRelease and Release builds, 
/// and remove them from shipping code.
///
#if !defined(EATEST_DEBUG_BREAK_ENABLED)
    #define EATEST_DEBUG_BREAK_ENABLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
/// EATEST_DEBUG_BREAK
///
/// This function causes an app to immediately stop under the debugger.
/// It is implemented as a macro in order to all stopping at the site 
/// of the call.
///
/// Example usage:
///    EATEST_DEBUG_BREAK();
///
/// The EATEST_DEBUG_BREAK default behaviour here can be disabled or changed by 
/// globally defining EATEST_DEBUG_BREAK_DEFINED and implementing an alternative
/// implementation of it. Our implementation here doesn't simply always have
/// it be defined externally because a major convenience of EATEST_DEBUG_BREAK
/// being inline is that it stops right on the troublesome line of code and
/// not in another function.
///
#if !defined(EATEST_DEBUG_BREAK)
    #if EATEST_DEBUG_BREAK_ENABLED
        #ifndef EATEST_DEBUG_BREAK_DEFINED
            #define EATEST_DEBUG_BREAK_DEFINED

            #if defined(EA_COMPILER_MSVC) && (_MSC_VER >= 1300)
                #define EATEST_DEBUG_BREAK() __debugbreak() // This is a compiler intrinsic which will map to appropriate inlined asm for the platform.
            #elif defined(EA_PLATFORM_SONY) && defined(EA_PROCESSOR_X86_64)
                #define EATEST_DEBUG_BREAK() do { { __asm volatile ("int $0x41"); } } while(0)
            #elif defined(EA_PROCESSOR_ARM) && (defined(__APPLE__) || defined(CS_UNDEFINED_STRING))
                #include <signal.h>
                #include <unistd.h>
                #define EATEST_DEBUG_BREAK() kill(getpid(), SIGINT)   // This lets you continue execution.
            //#elif defined(__APPLE__)
            //  #include <CoreServices/CoreServices.h>  // This works, but int3 works better on x86. Should we use this on Apple+ARM?
            //  #define EATEST_DEBUG_BREAK() Debugger() // Need to link with -framework CoreServices for this to be usable.
            #elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && defined(__APPLE__)
                #define EATEST_DEBUG_BREAK() {__asm__("int $3\n" : : ); }
            #elif defined(EA_PROCESSOR_ARM) && defined(__APPLE__)
                #define EATEST_DEBUG_BREAK() asm("trap") // Apparently __builtin_trap() doesn't let you continue execution, so we don't use it.
            #elif defined(EA_PROCESSOR_ARM64) && defined(__GNUC__)
                #define EATEST_DEBUG_BREAK() asm("brk 10")
            #elif defined(EA_PROCESSOR_ARM) && defined(__GNUC__)
                #define EATEST_DEBUG_BREAK() asm("BKPT 10")     // The 10 is arbitrary. It's just a unique id.
            #elif defined(EA_PROCESSOR_ARM) && defined(__ARMCC_VERSION)
                #define EATEST_DEBUG_BREAK() __breakpoint(10)
            #elif defined(EA_PROCESSOR_POWERPC) // Generic PowerPC. This triggers an exception by executing opcode 0x00000000.
                #define EATEST_DEBUG_BREAK() asm(".long 0")
            #elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && defined(EA_ASM_STYLE_INTEL)
                #define EATEST_DEBUG_BREAK() { __asm int 3 }
            #elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && (defined(EA_ASM_STYLE_ATT) || defined(__GNUC__))
                #define EATEST_DEBUG_BREAK() asm("int3") 
            #else
                #define EATEST_DEBUG_BREAK() *(int*)0=0
            #endif
        #else
            void EATEST_DEBUG_BREAK(); // User must define this externally.
        #endif
    #else
        #define EATEST_DEBUG_BREAK()
    #endif
#else
    #ifndef EATEST_DEBUG_BREAK_DEFINED
        #define EATEST_DEBUG_BREAK_DEFINED
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
/// EA_PREPROCESSOR_JOIN
///
/// This macro joins the two arguments together, even when one of
/// the arguments is itself a macro (see 16.3.1 in C++98 standard).
/// This is often used to create a unique name with __LINE__.
///
#ifndef EA_PREPROCESSOR_JOIN
    #define EA_PREPROCESSOR_JOIN(a, b)  EA_PREPROCESSOR_JOIN1(a, b)
    #define EA_PREPROCESSOR_JOIN1(a, b) EA_PREPROCESSOR_JOIN2(a, b)
    #define EA_PREPROCESSOR_JOIN2(a, b) a##b
#endif


namespace EA
{
    /// namespace UnitTest
    ///
    /// Implements the following:
    ///     Test               -- Base test class
    ///     TestFunction       -- Test class that calls a C function.
    ///     TestMemberFunction -- Test class that calls a C++ object's member function.
    ///     TestSuite          -- Test class that encapsulates a set of child test classes.
    ///     Verify, etc.       -- Set of functions that help individual test evaluations.
    ///     Rand               -- A utility random number generator.
    //
    namespace UnitTest
    {
        // Forward declarations
        class TestSuite;
        class Test;
        class TestFunction;
        template<typename T> class TestMemberFunction;

        /// kTestTraceGroupName
        /// This group name identifies all trace messages coming from
        /// unit tests. It effectively creates a separate 'channel'
        /// for unit test debug traces. This is only applicable to systems 
        /// that have tracing and logging systems that support the concept
        /// of output tagged by channel names or group names.
        /// Nowhere is kTestTraceGroupName used by the EATest, as EATest 
        /// attempts to stay independent of tracing/logging systems.
        extern const char8_t* kTestTraceGroupName;


        /// enum TestResult
        /// Defines standard test result values.
        /// These are used as documented in the various classes.
        enum TestResult
        {
            kTestResultOK       =  0,           /// The test completed OK.
            kTestResultError    =  1,           /// The test completed with error. Any value >= 1 is considered an error.
            kTestResultContinue =  INT_MIN,     /// The test has not yet completed. Allows tests to cooperatively multi-task their running.
            kTestResultNone     =  INT_MIN + 1  /// The test was not run or there is no test result.
        };

        /// IsDebuggerPresent
        ///
        /// Determines if the application is running under a debugger.
        ///
        EATEST_API bool IsDebuggerPresent();

        /// IsUserAdmin
        ///
        /// Determines if the current process has administator/super-user rights at this time.
        /// Note that it's possible a process can be dynamically elevated or deflated prior or 
        /// after this call. On console and hand-held platforms this function typically returns
        /// false, as all users on these platforms are considered un-privileged. Note that 
        /// platforms differ in what unprivileged users can do, and this function doesn't provide
        /// any information about that, as it's very platform-specific.
        ///
        EATEST_API bool IsUserAdmin();

        /// IsRunningUnderValgrind
        ///
        /// Returns true if the application is being run under valgrind.
        ///
        EATEST_API bool IsRunningUnderValgrind();

        /// Report
        ///
        /// Wrapper for EA::EAMain::Report.
        /// 
        EATEST_API void Report(const char8_t* pFormat, ...);

        /// ReportVerbosity
        ///
        /// Wrapper for EA::EAMain::ReportVerbosity.
        ///
        EATEST_API void ReportVerbosity(unsigned minVerbosity, const char8_t* pFormat, ...);

        /// GetVerbosity
        ///
        /// Wrapper for EA::EAMain::GetVerbosity.
        ///
        EATEST_API unsigned GetVerbosity();

        /// ThreadSleep
        ///
        /// Works the same as the EAThread::ThreadSleep function.
        /// Useful for threads and the testing system to intentionally yield time 
        /// without having to work with more elaborate synchronization primitives.
        /// Doesn't guarantee that the thread will resume precisely at the given time.
        ///
        EATEST_API void ThreadSleep(float fTimeMSRelative = 0.f);


        /// ThreadSleepRandom
        ///
        /// Sleeps for a number of milliseconds randomly chosen within the supplied range.
        /// Returns the amount of time it actually slept, though due to how threads
        /// work the actual time might be more than the returned time.
        ///
        EATEST_API unsigned ThreadSleepRandom(unsigned nMinSleepMS, unsigned nMaxSleepMS, bool bVerboseOutput = false);


        /// GetGlobalErrorCount / SetGlobalErrorCount / IncrementGlobalErrorCount
        ///
        /// Gets, sets, or increments the global error count.
        /// Sometimes code that reports errors has no way of getting it back to 
        /// the main app, possibly because it's in third party code, or because 
        /// it's in another thread that the main app can't control, etc.
        /// So we have the concept of a global error count which refers to errors
        /// reported by this mechanism. 
        /// The global error count is reported at the end of the program execution.
        /// These functions are thread-safe; they can be called from multiple threads
        /// simultaneously without problem.
        ///
        EATEST_API int  GetGlobalErrorCount();
        EATEST_API void SetGlobalErrorCount(int count);         // Can be a negative number, for example in the case that the user anticipates the global error count offsetting the main app's maintained error count. 
        EATEST_API int  IncrementGlobalErrorCount(int count);   // Can be a negative number, as with the example above, or if you want to retract a previously reported global error.

        /// Verify
        /// This is a basic stand-alone verification function that allows 
        /// basic test reporting outside of a Test class hierarchy and thus 
        /// is useful for very simple testing.
        /// 
        /// The user does not need nor want to put newline ('\n') characters at the end of 
        /// message strings as that will be done for you during the formatting of the output.
        /// You can use the lower level Report functions if you don't want newlines appended for you.
        /// 
        /// This behaviour applies to all other Verify functions in this module,
        /// but not to the (lower level) Report family of functions in this module.
        /// Returns true (similar to kTestResultOK) or false (similar to kTestResultError),
        /// with the return value being simply an echo of the input bValue.
        /// A lot of the time it's probably simpler to just use the EATEST_VERIFY macro
        /// instead of this function.
        ///
        /// Example usage:
        ///     Verify(list.size() == 17, "List size != 17.");

        EATEST_API bool Verify(bool bValue, const char8_t* pMessage, Test* pTestContext = NULL);


        /// EATEST_VERIFY
        ///
        /// This macro is a very basic test assertion system that acts without the existence of Test objects
        /// and TestSuite objects. It is basically a unit testing system unto itself, though with hardly 
        /// any features. Nevertheless, it is sometimes useful to do unit testing in such a very simple way.
        /// Returns the error count: 0 for expression is true (success) and 1 for the case that the expression is false (error).
        ///
        /// The user does not need nor want to put newline ('\n') characters at the end of 
        /// message strings as that will be done for you during the formatting of the output.
        /// You can use the lower level Report functions if you don't want newlines appended for you.
        /// 
        /// This function reports a failed verification as per the following format:
        ///     printf("%s(%d): %s.\n", __FILE__, __LINE__, expression);
        /// 
        /// Example usage:
        ///    int DoTest() {
        ///        int nErrorCount = 0;             // EATEST_VERIFY requires an 'nErrorCount' variable to be present.
        ///        EATEST_VERIFY(1 + 1 == 2);       // Increments nErrorCount if the test fails, and prints a message to gpReportFunction.
        ///        return nErrorCount;
        ///    }
        ///
        ///    EATEST_VERIFY_MSG(1 + 1 == 2, "1+1 != 2");
        ///    EATEST_VERIFY_F(1 + 1 == 2, "1+1 != %d", 2);
        ///
        
        namespace TestInternal{ // This is labelled internal because you usually want to use the macros below.
            EATEST_API int EATEST_VERIFY_IMP(bool bExpression, int& nErrorCount, const char* pFile, int nLine, const char* pMessage);
            EATEST_API int EATEST_VERIFY_F_IMP(bool bExpression, int& nErrorCount, const char* pFile, int nLine, const char* pFormat, ...);
            EATEST_API int EATEST_VERIFY_F_IMP_G(bool bExpression, const char* pFormat, ...);
        }

        #define EATEST_VERIFY(expression)          EA::UnitTest::TestInternal::EATEST_VERIFY_IMP((expression), nErrorCount, __FILE__, __LINE__, (#expression))
        #define EATEST_VERIFY_MSG(expression, msg) EA::UnitTest::TestInternal::EATEST_VERIFY_IMP((expression), nErrorCount, __FILE__, __LINE__, msg)

        #if defined(EA_COMPILER_NO_VARIADIC_MACROS)
            #define EATEST_VERIFY_F EA::UnitTest::TestInternal::EATEST_VERIFY_F_IMP_G
        #else
            #define EATEST_VERIFY_F(bExpression, pFormat, ...) EA::UnitTest::TestInternal::EATEST_VERIFY_F_IMP((bExpression), nErrorCount, __FILE__, __LINE__, pFormat, __VA_ARGS__)
        #endif

		/// EATEST_VERIFY_THROW
		/// EATEST_VERIFY_NOTHROW
		/// 
		/// This macro confirms whether or not an expression throws or doesn't throw. 
		/// 
		/// See EATEST_VERIFY for details about error reporting and the _MSG variants
		#if !defined(EA_COMPILER_NO_EXCEPTIONS)

			#define EATEST_VERIFY_IMPL(expression)                    \
				bool EA_PREPROCESSOR_JOIN(isThrow, __LINE__) = false; \
				try                                                   \
				{                                                     \
					(expression);                                     \
				}                                                     \
				catch (...)                                           \
				{                                                     \
					EA_PREPROCESSOR_JOIN(isThrow, __LINE__) = true;   \
				}

			#define EATEST_VERIFY_THROW(expression)                             \
				do                                                              \
				{                                                               \
					{                                                           \
						EATEST_VERIFY_IMPL(expression)                          \
						EATEST_VERIFY(EA_PREPROCESSOR_JOIN(isThrow, __LINE__)); \
					}                                                           \
				} while (false)

			#define EATEST_VERIFY_NOTHROW(expression)                            \
				do                                                               \
				{                                                                \
					{                                                            \
						EATEST_VERIFY_IMPL(expression)                           \
						EATEST_VERIFY(!EA_PREPROCESSOR_JOIN(isThrow, __LINE__)); \
					}                                                            \
				} while (false)

			#define EATEST_VERIFY_THROW_MSG(expression, msg)                             \
				do                                                                       \
				{                                                                        \
					{                                                                    \
						EATEST_VERIFY_IMPL(expression)                                   \
						EATEST_VERIFY_MSG(EA_PREPROCESSOR_JOIN(isThrow, __LINE__), msg); \
					}                                                                    \
				} while (false)

			#define EATEST_VERIFY_NOTHROW_MSG(expression, msg)                            \
				do                                                                        \
				{                                                                         \
					{                                                                     \
						EATEST_VERIFY_IMPL(expression)                                    \
						EATEST_VERIFY_MSG(!EA_PREPROCESSOR_JOIN(isThrow, __LINE__), msg); \
					}                                                                     \
				} while (false)
		#else
			#define EATEST_VERIFY_THROW(expression)
			#define EATEST_VERIFY_NOTHROW(expression)
			#define EATEST_VERIFY_THROW_MSG(expression, msg)
			#define EATEST_VERIFY_NOTHROW_MSG(expression, msg)
		#endif


		///////////////////////////////////////////////////////////////////////
        /// GetSystemTimeMicroseconds
        ///
        /// While this function could be used for basic benchmarking, the 
        /// EAStdC package's Stopwatch class is better suited towards this 
        /// and provides higher precision time measurement. The return value
        /// is not guaranteed to based on any particular start time (e.g. 1970),
        /// nor is it guaranteed to have microsecond-level precision.
        ///
        EATEST_API uint64_t GetSystemTimeMicroseconds();


        ///////////////////////////////////////////////////////////////////////
        /// MessageBoxAlert
        ///
        /// Displays a simple system message box with the given text and title.
        /// On platforms which don't have system GUIs (e.g. embedded systems
        /// and console systems), the box is displayed on the debugging host
        /// machine if there is such a host detected. Otherwise this function
        /// has no effect.
        ///
        EATEST_API void MessageBoxAlert(const char* pText, const char* pTitle);


        ///////////////////////////////////////////////////////////////////////
        /// DisableOSMessageBoxAlerts
        ///
        /// Disables message box alerts generated by the system. On systems 
        /// that don't generate such boxes, this function has no effect, as it
        /// doesn't need to do anything. The primary use of this function is 
        /// to prevent the system from running automated tests due to it 
        /// expecting there to be a user present to acknowledge error boxes.
        /// Most or all automated test applications should call this function.
        ///
        EATEST_API void DisableOSMessageBoxAlerts();


        ///////////////////////////////////////////////////////////////////////
        /// SetHighThreadPriority
        ///
        EATEST_API void SetHighThreadPriority();


        ///////////////////////////////////////////////////////////////////////
        /// SetNormalThreadPriority
        ///
        EATEST_API void SetNormalThreadPriority();


        ///////////////////////////////////////////////////////////////////////
        /// SetLowProcessPriority
        ///
        EATEST_API void SetLowProcessPriority();


        ///////////////////////////////////////////////////////////////////////////////
        // EnableAlignmentExceptionDetection
        //
        // This is a wrapper for usage of Microsoft's SEM_NOALIGNMENTFAULTEXCEPT.
        // This allows for detecting misaligned memory accesses.
        // By design, Microsoft provides no way to disable it once enabled.
        //
        EATEST_API void EnableAlignmentExceptionDetection();


        ///////////////////////////////////////////////////////////////////////
        /// NonInlinableFunction
        /// 
        /// This is simply a function that is guaranteed not to be inlined by 
        /// the compiler. It's useful for unit testing where you want to break
        /// a chain of inlining in order to do some kind of test. It's also useful
        /// for guaranteeing that a stack frame is made for the caller, since it
        /// cannot be a "leaf" function. This function may call an OS function
        /// or some other function that has side effects beyond a simple no-op.
        ///
        EATEST_API EA_NO_INLINE void NonInlinableFunction();


        ///////////////////////////////////////////////////////////////////////
        /// WriteToEnsureFunctionCalled
        ///
        /// Provides a location to write an integer variable to which the 
        /// compiler won't dismiss or compile away, including in optimized builds.
        /// 
        /// Example usage:
        ///     WriteToEnsureFunctionCalled() = SomeFunctionThatReturnsInt();
        ///
        EATEST_API int& WriteToEnsureFunctionCalled();

        ///////////////////////////////////////////////////////////////////////
        /// GetInteractive / SetInteractive
        ///
        /// GetInteractive returns the value set by SetInteractive. 
        /// Defaults to false if SetInteractive was never called. 
        /// SetInteractive sets whether the test run is "interactive", meaning
        /// the programmer is running it (including under a debugger), as opposed 
        /// to the tests being run unattended. EATest itself does nothing with 
        /// this interactivity setting. This setting is merely a central location
        /// where apps and their unit tests can use to tell if the session is
        /// interactive.
        /// The primary use of this feature is to allow unit test code that 
        /// expects user input during its run or to allow unit test code to
        /// such much longer than usual timeouts because the programmer needs
        /// them during debug sessions in the debugger.
        /// 
        EATEST_API bool GetInteractive();
        EATEST_API void SetInteractive(bool interactive);


        ///////////////////////////////////////////////////////////////////////
        /// GetTestLevel / SetTestLevel
        ///
        /// The test level allows for setting the amount of testing that occurs. 
        /// This allows for scaling back testing, typically to save time with 
        /// otherwise heavy tests. The interpretation of TestLevels is somewhat 
        /// test-dependent, but the default test level is expected to complete
        /// an entire application's unit tests within 3 minutes. This may seem 
        /// unreasonably short for huge packages, but it allows for basic daily 
        /// automation to avoid timing out while not monopolizing testing resources.
        ///
        /// This package itself doesn't use the test level; rather it's expected
        /// that the application set the level on startup based on command line
        /// arguments or environment variables, and the application's runtime 
        /// test code call GetTestLevel.
        ///
        /// The test level is an arbitrary int with no specific limit on its range.
        /// However, a default TestLevel enumeration exists to provide a convention.
        /// 
        enum TestLevel
        {
            kTestLevelLow     =   0,    /// Less testing, shorter testing time.
            kTestLevelDefault =  50,
            kTestLevelHigh    = 100     /// More testing, more testing time.
        };

        EATEST_API int  GetTestLevel();
        EATEST_API void SetTestLevel(int testLevel);



        ///////////////////////////////////////////////////////////////////////
        /// GetSystemSpeed
        ///
        /// Returns a basic measurement of system performance, for the purpose
        /// of tuning tests that may be long running. The return value is a floating
        /// point multiplier which indicates the speed of the system relative to 
        /// a decent desktop PC. It's this way because the point of this function
        /// is to scale tests back based on slower speeds of some systems.
        /// A typical developer PC of the year 2010 is 1.0, while a console 
        /// machine might have a value of 0.4. Values above 1.0 are possible.
        /// 
        enum SpeedType
        {
            kSpeedTypeCPU,  // Encompasses CPU and its memory.
            kSpeedTypeFPU,
            kSpeedTypeGPU,
            kSpeedTypeDisk
        };

        EATEST_API float GetSystemSpeed(SpeedType speedType = kSpeedTypeCPU);


        ///////////////////////////////////////////////////////////////////////
        /// GetSystemMemoryMB
        ///
        /// Returns a measurement of usable system main memory in megabytes, 
        /// for the purpose of tuning tests that may require a lot of memory.
        /// A system with 2 GB of RAM will return a value like 2048.
        /// 
        EATEST_API uint64_t GetSystemMemoryMB();


        ///////////////////////////////////////////////////////////////////////
        /// class Test
        ///
        /// The base class for all tests.
        ///
        /// Example usage:
        ///     class MathTest : public Test {
        ///         int Run() {
        ///             Verify(1 < 2, "Failure of (1 < 2) comparison.");
        ///             Verify(3 < 4, "Failure of (3 < 4) comparison.");
        ///             return mnErrorCount ? kTestResultError : kTestResultSuccess;
        ///         }
        ///     };
        ///
        ///     MathTest mathTest("Test via MathTest class");
        ///     mathTest.Run();
        ///
        class EATEST_API Test
        {
        public:
            /// Test
            ///
            /// Constructs a Test with the given optional name and ReportFunction.
            /// If the test name is not supplied, it is empty. If the ReportFunction is
            /// not supplied, the default global report function is used.
            ///
            Test(const char8_t* pTestName = NULL, EA::EAMain::ReportFunction pReportFunction = NULL);

            /// ~Test
            ///
            /// Standard destructor.
            ///
            virtual ~Test();

            /// Init
            ///
            /// Initializes a test for use. 
            /// Returns kResultOK or kResultError.
            /// This version does nothing and simply returns kResultOK.
            /// The user may choose to override this to do something the user needs.
            ///
            virtual int Init();

            /// Shutdown
            ///
            /// Shuts down the Test. 
            /// Returns kResultOK or kResultError.
            /// This version does nothing and simply returns kResultOK.
            /// The user may choose to override this to do something the user needs.
            ///
            /// If the Test has been Run but returned kTestResultContinue (i.e. it
            /// thinks it is still running), this Shutdown function does nothing
            /// about it. The Test destruction is expected to take care of this situation.
            /// However, the user may override this Shutdown function in their Test
            /// subclass to do something else.
            ///
            virtual int Shutdown();

            /// GetName
            ///
            /// Returns the name of the Test as a string.
            ///
            virtual void GetName(eastl::string& sName) const;

            /// GetParentSuite
            ///
            /// Returns a pointer to the parent TestSuite if one was set. 
            /// Otherwise this function returns NULL. To attach a Test to a TestSuite,
            /// call the TestSuite::AddTest function. That function will add the Test
            /// to the suite and mark the Test as belong to the suite.
            ///
            virtual TestSuite* GetParentSuite() const;

            /// GetReportFunction
            ///
            /// Returns a pointer to the ReportFunction if one was set.
            /// If no ReportFunction was directly set for this Test, this function
            /// looks to see if it is part of a TestSuite and will return that
            /// Report function if it is found. And so on up the TestSuite tree.
            /// Otherwise this function returns NULL.
            ///
            virtual EA::EAMain::ReportFunction GetReportFunction() const;

            /// WriteReport
            ///
            /// Writes a report of the current Test result. 
            /// The report will be a single line written to the ReportFunction and  
            /// will be terminated with a newline character.
            /// This function can be called at any time, including before Run has been
            /// called, while Run is in progress (kTestResultContinue) and after Run 
            /// has completed with a definitive result).
            ///
            virtual void WriteReport();

            /// Run
            ///
            /// Runs the test.
            /// Returns one of enum TestResult.
            /// This version of the function simply returns kTestResultOK; the user is
            /// expected to override this function to do something useful.
            ///
            /// If this function returns kResultContinue, it means that the test is not
            /// complete and that the user should call Run again until a different return
            /// value is given (i.e. until the Test is complete). This functionality is 
            /// useful for tests that need to run asynchronously.
            ///
            virtual int Run();

            /// Verify
            ///
            /// This a simple expression verification function.
            ///
            /// Example usage:
            ///     int Run() {
            ///         Verify(3 < 4, "Failure of (3 < 4) comparison.");
            ///         return mnErrorCount ? kTestResultError : kTestResultSuccess;
            ///     }
            ///
            virtual bool Verify(bool bValue, const char8_t* pMessage);

            /// VerifyFormatted
            ///
            /// This is a version of Verify that accepts printf-like formatting statements.
            ///
            /// Example usage:
            ///     int Run() {
            ///         VerifyFormatted(3 < 4, "Failure of (3 < 4) comparison at file %s and line %d", __FILE__, __LINE__);
            ///         return mnErrorCount ? kTestResultError : kTestResultSuccess;
            ///     }
            ///
            virtual bool VerifyFormatted(bool bValue, const char8_t* pFormat, ...);

        protected:
            friend class TestSuite;

            eastl::string   msTestName;
            TestSuite*      mpParentSuite;
            size_t          mnSuccessCount;
            size_t          mnErrorCount;
            EA::EAMain::ReportFunction  mpReportFunction;
            bool            mbForceReport;
        };



        ///////////////////////////////////////////////////////////////////////
        /// class TestFunction
        ///
        /// A test that is run by a standalone function call.
        ///
        /// The function has a simple return value: int. This return value is 
        /// 0 upon success and non-zero upon test failure. It is common for
        /// such test functions to return the number of test failures.
        /// The reason it returns 0 and non-zero instead of enum TestResult is 
        /// that a primary goal in this case is to allow the member function
        /// to be completely independent of EATest.
        /// 
        /// Example usage:
        ///     int TestMathLib() {
        ///         int nErrorCount = 0;
        ///         if((1 + 1) != 2)
        ///             nErrorCount++;
        ///         if((2 + 2) != 4)
        ///             nErrorCount++;
        ///         EATEST_VERIFY(1 + 1 == 2); // If this code knows about EATest, it can use the global verify and report functions.
        ///         return nErrorCount;
        ///     }
        ///
        ///     TestFunction testMathLib("Test via TestMathLib()", TestMathLib);
        ///     testMathLib.Run();
        ///
        class EATEST_API TestFunction : public Test
        {
        public:
            /// FunctionPtr
            ///
            /// This is a typedef for a C++ class member function used as a test.
            /// It returns 0 if there are no errors and non-zero if there are errors.
            /// It is common to have it simply return the error count, but that is
            /// merely common convention and isn't required by the TestFunction class.
            ///
            /// To do: The declaration below should be fixed to return TestResult instead of int.
            ///
            typedef int (*FunctionPtr)();

            /// TestFunction
            ///
            /// Constructs a TestFunction object with the given name and function.
            /// 
            TestFunction(const char8_t* pTestName = NULL, FunctionPtr pFunction = NULL);

            /// Run
            ///
            /// This is a subclass of the parent Test::Run function.
            /// It calls the designated test function.
            /// As per the documentation on FunctionPtr, the user test
            /// function is expected to return 0 on success or non-zero on failure.
            /// Returns enum TestResult.
            /// If no test function has been designated, the return value is kTestResultOK.
            ///
            virtual int Run();

        protected:
            FunctionPtr mpFunction;
        };



        ///////////////////////////////////////////////////////////////////////
        /// class TestMemberFunction
        ///
        /// A test that is run by a class member function call.
        ///
        /// This class allows any C++ class member function to act as a unit
        /// test, as long as it has the expected function signature. The member
        /// function is called by the TestMemberFunction class in its Run call.
        ///
        /// The class member function has a simple return value: int. This return
        /// value is 0 upon success and non-zero upon test failure. It is common
        /// for such test functions to return the number of test failures.
        /// The reason it returns 0 and non-zero instead of enum TestResult is 
        /// that a primary goal in this case is to allow the member function
        /// to be completely independent of EATest.
        /// 
        /// The advantage to this approach is that the given tested class and
        /// member function do not need to know about the testing system and
        /// can be independent of it. The disadvantage is that the tested 
        /// member function has no direct access to the Test object calling it 
        /// and thus cannot call Test member functions such as Verify. It can 
        /// however use the global Verify functions and macros, as seen in the
        /// example usage below.
        ///
        /// Example usage:
        ///     struct MathLib {
        ///         int DoTest() {
        ///             int nErrorCount = 0;
        ///             if((1 + 1) != 2)
        ///                 nErrorCount++;
        ///             if((2 + 2) != 4)
        ///                 nErrorCount++;
        ///             EATEST_VERIFY(1 + 1 == 2); // If this code knows about EATest, it can use the global verify and report functions.
        ///             return nErrorCount;
        ///         }
        ///     };
        ///
        ///     MathLib mathLib;
        ///     TestMemberFunction<MathLib> mathLibTest("Test of MathLib", &mathLib, MathLib::DoTest);
        ///     mathLibTest.Run();
        ///
        template<typename T>
        class TestMemberFunction : public Test
        {
        public:
            /// MemberFunctionPtr
            ///
            /// This is a typedef for a C++ class member function used as a test.
            /// It returns 0 if there are no errors and non-zero if there are errors.
            /// It is common to have it simply return the error count, but that is
            /// merely common convention and isn't required by the TestMemberFunction class.
            /// This function signature is used for the test function but is also
            /// used for optional init and shutdown functions that the user may provide.
            ///
            /// To do: The declaration below should be fixed to return TestResult instead of int.
            ///
            typedef int (T::*MemberFunctionPtr)();

        public:
            /// TestMemberFunction
            ///
            /// Constructs a TestMemberFunction object with the given name and test
            /// class functions.
            ///
            TestMemberFunction(const char8_t* pTestName = NULL, T* pObject = NULL, MemberFunctionPtr pTestFunction = NULL,
                                MemberFunctionPtr pInitFunction = NULL, MemberFunctionPtr pShutdownFunction = NULL)
                : Test(pTestName),
                  mpObject(pObject),
                  mpTestFunction(pTestFunction),
                  mpInitFunction(pInitFunction),
                  mpShutdownFunction(pShutdownFunction)
            {}

            /// Run
            ///
            /// This is a subclass of the parent Test::Run function.
            /// It calls the designated member class test function.
            /// As per the documentation on MemberFunctionPtr, the user test
            /// function is expected to return 0 on success or non-zero on failure.
            /// Returns enum TestResult.
            /// If no test function has been designated, the return value is kTestResultOK.
            ///
            virtual int Run()
            {
                if(mpObject)
                {
                    const int result = (mpObject->*mpTestFunction)();
                    if(result != 0)
                    {
                        mnErrorCount++;
                        return kTestResultError;
                    }
                    mnSuccessCount++;
                }
                return kTestResultOK;
            }

            /// Init
            ///
            /// This is a subclass of the parent Test::Init function.
            /// It calls the member class init function if one has been designated.
            /// Returns enum TestResult.
            /// If no init function has been designated, the return value is kTestResultOK.
            ///
            virtual int Init()
            {
                if(mpInitFunction)
                {
                    const int result = (mpObject->*mpInitFunction)();
                    if(result != 0)
                        return kTestResultError;
                }
                return kTestResultOK;
            }

            /// Shutdown
            ///
            /// This is a subclass of the parent Test::Init function.
            /// It calls the member class shutdown function if one has been designated.
            /// Returns enum TestResult.
            /// If no shutdown function has been designated, the return value is kTestResultOK.
            ///
            virtual int Shutdown()
            {
                if(mpShutdownFunction)
                {
                    const int result = (mpObject->*mpShutdownFunction)();
                    if(result != 0)
                        return kTestResultError;
                }
                return kTestResultOK;
            }

        protected:
            T*                mpObject;
            MemberFunctionPtr mpTestFunction;
            MemberFunctionPtr mpInitFunction;
            MemberFunctionPtr mpShutdownFunction;
        };


        ///////////////////////////////////////////////////////////////////////
        /// class TestCollection
        ///
        /// A container for multiple tests.  The TestCollection manages the
        /// lifetime of its tests, but it does not call Init(), Shutdown(),
        /// Run(), or other testing-specific functionality.
        ///
        class EATEST_API TestCollection
        {
        public:
            /// TestCollection
            ///
            /// Default constructor.
            ///
            TestCollection();

            /// ~TestCollection
            ///
            /// Standard destructor.
            ///
            virtual ~TestCollection();

            /// AddTest (Test)
            ///
            /// Add a new test (via Test object) to the suite.
            /// If input 'bTakeOwnership' is true, then we take over ownership 
            /// of the input pointer and delete it upon completion of the test.
            /// Returns true if the given test was successfully added.
            /// If the Test is added more than once, the subsequent AddTest 
            /// calls will have no effect and will return false.
            ///
            virtual bool AddTest(Test* pTest, bool bTakeOwnership = false);

            /// AddTest (TestFunction)
            ///
            /// Add a new test (via C function) to the suite.
            /// Returns true if the given test was successfully added.
            /// If the same function is added multiple times, it is tested multiple times.
            /// See the TestFunction class for documentation on now the user-supplied
            /// test function is expected to work.
            ///
            /// This function allocates a TestFunction object from the global heap and 
            /// you should avoid using this function if you want to avoid allocating 
            /// global memory. You can instead call AddTest with your own supplied TestFunction.
            ///
            virtual void AddTest(const char8_t* pTestName, TestFunction::FunctionPtr pFunction);

            /// AddTest (TestMemberFunction)
            ///
            /// Add a new test (via member function) to the suite.
            /// See the TestMemberFunction class for documentation on now the user-supplied
            /// test function is expected to work.
            /// If the same function is added multiple times, it is tested multiple times.
            ///
            template<typename U>
            void AddTest(const char8_t* pTestName, U* pObject,
                typename TestMemberFunction<U>::MemberFunctionPtr pTestFunction,
                typename TestMemberFunction<U>::MemberFunctionPtr pInitFunction = NULL,
                typename TestMemberFunction<U>::MemberFunctionPtr pShutdownFunction = NULL)
            {
                TestMemberFunction<U>* const pTMF = new TestMemberFunction<U>(pTestName, pObject, pTestFunction, pInitFunction, pShutdownFunction);
                AddTest(pTMF, true);
            }

            /// AddTests (TestCollection)
            ///
            /// Add a collection of tests to this collection.  Because the test instances
            /// will be shared between the two collections, we don't inherit the lifetime
            /// management responsibilities (e.g. bDeleteIfOwned) in our collection.
            ///
            virtual void AddTests(const TestCollection *pCollection);

            /// RemoveTest (Test)
            ///
            /// Remove an existing Test from the suite.
            /// The Shutdown function of the Test will be called.
            ///
            virtual bool RemoveTest(Test* pTest, bool bDeleteIfOwned = true);

            /// RemoveTest (test name)
            ///
            /// Remove an existing Test from the suite.
            /// The Shutdown function of the Test will be called.
            ///
            virtual bool RemoveTest(const char8_t* pTestName, bool bDeleteIfOwned = true);

            /// FindTest
            ///
            /// Finds a child test of the given pTestName, case-insensitively.
            /// The pTestName may be of the form <suite>/<suite>/.../<name>
            /// for the case of test suites within test suites.
            ///
            Test* FindTest(const char8_t* pTestName);

            /// EnumerateTests
            ///
            /// Get a list of tests in the suite via a user-supplied array of entries.
            /// Input nTestArrayCapacity is the number of items pTestArray can hold.
            /// Returns the number of tests, which may be >= nTestArrayCapacity.
            /// If pTestArray is NULL, nTestArrayCapacity is presumed to be 0.
            ///
            virtual size_t EnumerateTests(Test* pTestArray[], size_t nTestArrayCapacity) const;

        protected:
            struct TestInfo
            {
                Test* mpTest;        /// Pointer to test object itself.
                bool  mbOwned;       /// If true, we delete mpTest on shutdown or test removal.
            };
            typedef eastl::vector<TestInfo> TestArray;

            TestInfo* FindTestInfo(const char8_t* pTestName, bool bRecursive);

        protected:
            TestArray mTests;        /// All tests in the collection.
        };



        ///////////////////////////////////////////////////////////////////////
        /// class TestSuite
        ///
        /// A container for multiple tests which acts like a single test.
        ///
        /// Example usage:
        ///     int TestMathLib();
        ///     int TestStringLib();
        ///
        ///     TestFunction testMathLib(TestMathLib, "Math Lib Test");
        ///     TestFunction testStringLib(TestStringLib, "Graphics Lib Test");
        ///
        ///     TestSuite testSuite("Test suite");
        ///     testSuite.AddTest(&testMathLib);
        ///     testSuite.AddTest(&testStringLib);
        ///     testSuite.Run();
        ///
        class EATEST_API TestSuite : public Test, public TestCollection
        {
        public:
            /// TestSuite
            ///
            /// Default constructor. Constructs a test suite with a given name.
            /// If the test name is not supplied, it is empty. 
            /// A TestSuite name may not have a '/' character in it, as this 
            /// character is reserved for separating hierarchical test suites.
            /// The test result is initialized to kTestResultNone.
            ///
            TestSuite(const char8_t* pTestName = NULL);

            /// ~TestSuite
            ///
            /// Standard destructor.
            ///
            virtual ~TestSuite();

            /// Run
            ///
            /// Override of Test::Run; see the Test::Run function for details
            /// on how Run is expected to work. In this case, the Run function
            /// runs each of Run functions of the Tests in the TestSuite.
            /// If any individual Test returns kTestResultContinue, this function
            /// will return kTestResultContinue. 
            /// If any individual Test returns kTestResultError, this function will
            /// return kTestResultError after all Tests in the suite have returned
            /// a value other than kTestResultContinue.
            ///
            virtual int Run();

            /// RunTest
            ///
            /// Run a specific test (identified by name) from the suite.
            /// There is no RunTest(Test*) function in this class because such 
            /// a thing would be pointless because you could just call Run on 
            /// the given test object directly. Additionally, you could also 
            /// call this function with the name of the given Test.
            /// This function matches the pName argument to the test name in 
            /// case-insensitive way. 
            /// For the case where you want to run a test from a suite that is
            /// within this suite, you would reference the test by the format:
            ///     <suite_name>/<test_name>
            /// If you want to run an entire suite that is within this suite,
            /// you simply call this function with the suite name.
            ///
            /// If the test is not found, the return value is kTestResultError.
            ///
            /// To do: The declaration below should be fixed to return TestResult instead of int.
            ///
            virtual int RunTest(const char8_t* pName);

            /// GetTestResult
            ///
            /// Returns the current TestResult.
            ///
            int GetTestResult() const;

            /// AddTest (Test)
            ///
            /// Add a new test (via Test object) to the suite.
            /// If input 'bTakeOwnership' is true, then we take over ownership 
            /// of the input pointer and delete it upon completion of the test.
            /// Returns true if the given test was successfully added.
            /// If the Test is added more than once, the subsequent AddTest 
            /// calls will have no effect and will return false.
            ///
            virtual bool AddTest(Test* pTest, bool bTakeOwnership = false);

            /// AddTest (TestFunction)
            ///
            /// Add a new test (via C function) to the suite.
            /// Returns true if the given test was successfully added.
            /// If the same function is added multiple times, it is tested multiple times.
            /// See the TestFunction class for documentation on now the user-supplied
            /// test function is expected to work.
            ///
            /// This function allocates a TestFunction object from the global heap and 
            /// you should avoid using this function if you want to avoid allocating 
            /// global memory. You can instead call AddTest with your own supplied TestFunction.
            ///
            virtual void AddTest(const char8_t* pTestName, TestFunction::FunctionPtr pFunction);

            /// AddTest (TestMemberFunction)
            ///
            /// Add a new test (via member function) to the suite.
            /// See the TestMemberFunction class for documentation on now the user-supplied
            /// test function is expected to work.
            /// If the same function is added multiple times, it is tested multiple times.
            ///
            template<typename U>
            void AddTest(const char8_t* pTestName, U* pObject,
                typename TestMemberFunction<U>::MemberFunctionPtr pTestFunction,
                typename TestMemberFunction<U>::MemberFunctionPtr pInitFunction = NULL,
                typename TestMemberFunction<U>::MemberFunctionPtr pShutdownFunction = NULL)
            {
                TestCollection::AddTest(pTestName, pObject, pTestFunction, pInitFunction, pShutdownFunction);
            }

            /// RemoveTest (Test)
            ///
            /// Remove an existing Test from the suite.
            /// The Shutdown function of the Test will be called.
            ///
            virtual bool RemoveTest(Test* pTest, bool bDeleteIfOwned = true);

            /// RemoveTest (test name)
            ///
            /// Remove an existing Test from the suite.
            /// The Shutdown function of the Test will be called.
            ///
            virtual bool RemoveTest(const char8_t* pTestName, bool bDeleteIfOwned = true);

            /// WriteReport
            ///
            /// This is an override of Test::WriteReport.
            /// This version writes a different version of a report than Test::WriteReport.
            /// It writes a report that identifies the output as coming from a test suite.
            /// The report will be multiple lines written to the ReportFunction and  
            /// each line will be terminated with a newline character.
            ///
            void WriteReport();

        protected:
            /// SetupTest
            ///
            /// Perform any necessary setup work for a test in this suite before
            /// the test is run. This is similar to the test's Init functionality,
            /// but it is implemented at the TestSuite-level.
            ///
            /// Returns kResultOK or kResultError.
            ///
            virtual int SetupTest();

            /// TeardownTest
            ///
            /// Perform any necessary teardown work for a test in this suite before
            /// the test is run. This is similar to the test's Shutdown functionality,
            /// but it is implemented at the TestSuite-level.
            ///
            /// Returns kResultOK or kResultError.
            ///
            virtual int TeardownTest();

        protected:
            struct ResultInfo
            {
                Test* mpTest;       /// Pointer to test object itself.
                int   mnResult;     /// Stores result of last Run of mpTest.
            };
            typedef eastl::vector<ResultInfo> ResultArray;

            void Run(ResultInfo& resultInfo);

        protected:
            int         mnTestResult;     /// Current test result.
            ResultArray mResults;         /// Tests which are currently running and have not yet returned kTestResultOK or kTestResultError.

        }; // class TestSuite



        ///////////////////////////////////////////////////////////////////////////////
        /// TestApplication
        ///
        /// A TestApplication is a TestSuite that has some additional functionality
        /// at the application level, such as argc/argv parsing.
        /// 
        class EATEST_API TestApplication : public TestSuite
        {
        public:
            /// FunctionPtr
            ///
            /// This function type defines the signature for the application's
            /// initialization and shutdown functions.
            ///
            typedef int (*FunctionPtr)();

            TestApplication(const char8_t* pTestApplicationName, int argc = 0, char** argv = NULL, 
                            FunctionPtr pInitFunction = NULL, FunctionPtr pShutdownFunction = NULL);

           ~TestApplication();

            /// SetArg
            ///
            /// Sets the argc/argv for use when Run is called.
            ///
            void SetArg(int argc, char** argv);

            /// SetForceReport
            ///
            /// Sets whether or not the test results are reported.
            ///
            void SetForceReport(bool bReport);

            /// Run
            ///
            /// Runs the tests that were set by the TestSuite (our parent class) AddTest function.
            /// However, our version of Run is different from TestSuite, in that we use the argc/argv
            /// values to decide what to run. The argv array is searched for arguments of the form:
            ///     -run:<testname>
            /// The argv array is processed linearly from beginning to end and tests are run as they
            /// are found in the argv array. If a test is specified multiple times by multiple -run
            /// entries then the test is run multiple times. If there are no -run arguments, then 
            /// all tests are run.
            /// 
            /// Additionally, a -list command in the argv array causes PrintTestNames(false) to
            /// be called and thus results in the test names being printed to the standard output.
            /// A -listDetail command in the argv array causes PrintNames(true) to be called and 
            /// thus results in the name hierarchy to be printed.
            /// 
            /// The above recognized arguments (-run, -list, -listDetail) are case insensitive with
            /// respect to the argument and the tests the argument is referring to.
            ///
            int Run();

            /// PrintUsage
            ///
            /// Prints some basic usage information.
            ///
            void PrintUsage();

            /// PrintTestNames
            ///
            /// Prints a listing of the test names to our standard output. 
            /// If bDetail is true, then the function recursively calls PrintTestNames
            /// on child test suites with the result being output that has a hierarchy
            /// displayed like file system paths (e.g. "Math Suite/Vector Suite/Normalize Test").
            ///
            void PrintTestNames(bool bDetail);

        protected:
            /// Init
            ///
            /// Initializes the test application.  This is run before any of the
            /// application's tests.  If initialization fails, no additional tests
            /// will be run and the application will return failure.
            ///
            virtual int Init();

            /// Shutdown
            ///
            /// Shuts down the test application.  This is run before after of the
            /// application's tests.  If shutdown fails, the application will
            /// return failure (regardless of whther or not the tests passed).
            ///
            virtual int Shutdown();

        protected:
            int         mArgc;
            char**      mArgv;
            FunctionPtr mpInitFunction;
            FunctionPtr mpShutdownFunction;
        };


        /// GetRegistry
        ///
        /// The registry is a global collection of tests.  It exists to facilitate
        /// automatic global test registration.  See below.
        /// 
        EATEST_API TestCollection& GetRegistry();


        /// AutoRegisterTest
        ///
        /// Helper class that automatically registers and unregisters tests
        /// with the global registry while the helper object is in scope.
        ///
        template <typename T>
        class AutoRegisterTest
        {
        public:
            AutoRegisterTest()
            {
                GetRegistry().AddTest(&mTest);
            }

            ~AutoRegisterTest()
            {
                GetRegistry().RemoveTest(&mTest);
            }

        private:
            AutoRegisterTest(const AutoRegisterTest&);
            AutoRegisterTest& operator=(const AutoRegisterTest&);

        private:
            T mTest;
        };


        /// AutoRegisterTestFunction
        ///
        /// Helper class that automatically registers and unregisters test functions
        /// with the global registry while the helper object is in scope.
        ///
        class AutoRegisterTestFunction
        {
        public:
            AutoRegisterTestFunction(const char8_t* pName, TestFunction::FunctionPtr pFunction)
                : mpName(pName)
            {
                GetRegistry().AddTest(pName, pFunction);
            }

            ~AutoRegisterTestFunction()
            {
                GetRegistry().RemoveTest(mpName);
            }

        private:
            AutoRegisterTestFunction(const AutoRegisterTestFunction&);
            AutoRegisterTestFunction& operator=(const AutoRegisterTestFunction&);

        private:
            const char8_t* const mpName;
        };


        /// EATEST_REGISTER_TEST
        ///
        /// Helper macro that can be used to add a Test class to the global registry.
        ///
        /// Examples:
        ///     class ExampleTestClass : public EA::UnitTest::Test { ... };
        ///     EATEST_REGISTER_TEST(ExampleTestClass);
        ///
        ///     class ExampleTestSuite : public EA::UnitTest::TestSuite { ... };
        ///     EATEST_REGISTER_TEST(ExampleTestSuite);
        ///
        #define EATEST_REGISTER_TEST(T) \
            static EA::UnitTest::AutoRegisterTest<T> EA_PREPROCESSOR_JOIN(EATest, __LINE__);

        /// EATEST_REGISTER_TEST_FUNCTION
        ///
        /// Helper macro that can be used to add a TestFunction to the global registry.
        ///
        /// Example:
        ///     int ExampleTestFunction() { ... }
        ///     EATEST_REGISTER_TEST_FUNCTION("ExampleTestFunction", ExampleTestFunction);
        ///
        #define EATEST_REGISTER_TEST_FUNCTION(name, pFunction) \
            static EA::UnitTest::AutoRegisterTestFunction EA_PREPROCESSOR_JOIN(EATest, __LINE__)(name, pFunction);

        ///////////////////////////////////////////////////////////////////////////////
        /// Rand
        ///
        /// Implements a basic random number generator for unit tests. It's not
        /// intended to be a robust random number generator (though it is decent), 
        /// but rather is present so the unit tests can have a portable random number
        /// generator they can rely on being present.
        ///
        /// Example usage:
        ///    Rand     rng(time());
        ///    uint32_t x = rng();                      // Generate value in range of [0, 0xffffffff] (i.e. generate any uint32_t)
        ///    int32_t  y = rng.RandValue();            // Generate value in range of [0, 0xffffffff] (i.e. generate any uint32_t)
        ///    int32_t  z = rng.RandLimit(1000);        // Generate value in range of [0, 1000)
        ///    int32_t  w = rng.RandRange(-50, +30);    // Generate value in range of [-50, +30)
        ///
        /// Example usage with the random_shuffle algorithm:
        ///    Rand rng(time());
        ///    eastl::random_shuffle(first, last, rng);
        ///
        class Rand
        {
        public:
            /// RandValue
            ///
            /// Constructs a Rand object with a given seed.
            /// The user must supply a seed; there is no default value.
            ///
            Rand(uint32_t nSeed) 
                : mnSeed(nSeed) { }

            uint32_t GetSeed() const
            {
                return mnSeed;
            }

            void SetSeed(uint32_t nSeed)
            {
                mnSeed = nSeed;
            }

            /// RandValue
            ///
            /// Returns a pseudorandom value in range of [0, 0xffffffff] (i.e. generate any uint32_t)
            ///
            uint32_t RandValue()
            {
                if(mnSeed == 0)
                    mnSeed = 0xfefe; // Can't have a seed of zero.
                const uint64_t nResult64 = ((mnSeed * (uint64_t)1103515245) + 12345);
                return (mnSeed = (uint32_t)((nResult64 >> 16) & 0xffffffff));
            }  

            /// RandLimit
            ///
            /// Returns a pseudorandom value in range of [0, nLimit)
            ///
            uint32_t RandLimit(uint32_t nLimit)
                { return (uint32_t)((RandValue() * (uint64_t)nLimit) >> 32); }

            /// RandRange
            ///
            /// Returns a pseudorandom value in range of [nBegin, nEnd)
            ///
            int32_t RandRange(int32_t nBegin, int32_t nEnd)
                { return nBegin + (int32_t)RandLimit((uint32_t)(nEnd - nBegin)); }

            /// operator()
            ///
            /// Identical to RandValue().
            /// Returns a pseudorandom value in range of [0, 0xffffffff] (i.e. generate any uint32_t)
            ///
            uint32_t operator()()
                { return RandValue(); }

            /// operator()
            ///
            /// Identical to RandLimit().
            /// Returns a pseudorandom value in range of [0, nLimit)
            ///
            uint32_t operator()(eastl_size_t n)
                { return RandLimit((uint32_t)n); }

        protected:
            uint32_t mnSeed;

        }; // class Rand



        ///////////////////////////////////////////////////////////////////////////////
        /// RandGenT
        ///
        /// A wrapper for Rand which generates values of the given integral
        /// data type. This is mostly useful for cleanly avoiding compiler warnings, 
        /// as we intentionally enable the highest warning levels in these tests.
        ///
        /// Example usage:
        ///    RandGenT<uint32_t>      rng(gRandSeed);
        ///    eastl::vector<uint32_t> intVector(100000);
        ///
        ///    eastl::generate(intVector.begin(), intVector.end(), rng);
        ///
        template <typename Integer>
        struct RandGenT
        {
            // Intentionally don't provide a default constructor; the user must initialize this with some seed.
            RandGenT(uint32_t nSeed)
                : mRand(nSeed) { }

            Integer operator()()
                { return (Integer)mRand.RandValue(); }

            Integer operator()(Integer n) // n must be >= 0.
                { return (Integer)mRand.RandLimit((uint32_t)n); }

            uint32_t GetSeed() const
                { return mRand.GetSeed(); }

            void SetSeed(uint32_t seed)
                { mRand.SetSeed(seed); }

            Rand mRand;
        };


        ///////////////////////////////////////////////////////////////////////////////
        /// gRandSeed
        ///
        /// Defines a global seed to use for all tests. This allows the user to run 
        /// the tests with given random seeds in a repeatable way. Direct access
        /// of gRandSeed is deprecated and GetRandSeed() should be used instead.
        ///
        /// Example usage:
        ///    Rand rng(EA::UnitTest::GetRandSeed());
        /// 
        EATEST_API uint32_t GetRandSeed();
        EATEST_API void     SetRandSeed(uint32_t seed);

    } // namespace UnitTest
} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif
#endif // Header include guard




