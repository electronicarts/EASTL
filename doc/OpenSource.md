=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
HowTo Create the EASTL Open Source repository
=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
This doc outlines how to update the EASTL open source repository on Github.
https://github.com/electronicarts/EASTL/
=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

1) Goto scripts/make_public.bat and update the OPENSOURCE and CODESTRIPPER settings.
	* OPENSOURCE: is the directory that will contain the eastl.template folder and it will generate the EASTL open source folder here.
		* set OPENSOURCE=e:\p4\opensource\
	* CODESTRIPPER: is the directory of the CodeStripper package obtained from perforce 
		* set CODESTRIPPER=e:\p4\packages\CodeStripper\dev
	* NANT_EXE: the complete path to the nant.exe you would like to use.
		* set NANT_EXE=E:\p4\packages\Framework\dev\bin\nant.exe

2) Sync the opensource folder from EAOS to the "OPENSOURCE" location
	* //EAOS_SB/rparolin/opensource/

3) Download cmake.  EASTL requires version at least 3.4. 
	* https://cmake.org/download/

4)  Update the test.bat batch file with the cmake binary folder location
	* set CMAKE_BIN=%~dp0/../cmake/cmake-3.4.1-win32-x86/bin
	* test.bat is located in the opensouce/east.template/test folder

5) Run the make_public.bat

6) Ensure the grep commands do not find any NDA information.
	* grep command found in "grep_for_keywords.bat" 

Once the make_public.bat script completes the open source version of EASTL will be located at '%OPENSOURCE%/eastl'
