rem Needed to check if a variable contains a string
@setlocal enableextensions enabledelayedexpansion

rem Check if the platform variable contains MinGW
if not [x%PLATFORM:MinGW=%]==[x%PLATFORM%] (
	goto :test-mingw
) else (
	goto :test-msvc
)

:test-msvc
cd build
cd test
ctest -C Release -V || goto error
cd .. 
cd benchmark
ctest -C Release -V || goto error
cd ..
cd ..
goto :end

:test-mingw
rem Test gcc and clang.
set "OLDPATH=%PATH%"

if not [x%PLATFORM:x86=%]==[x%PLATFORM%] (
	set bin_folder=C:/msys64/mingw32/bin
	set "PATH=C:\msys64\mingw32\bin;%OLDPATH%"
) else (
	set bin_folder=C:/msys64/mingw64/bin
	set "PATH=C:\msys64\mingw64\bin;%OLDPATH%"
)

if not [x%PLATFORM:x64=%]==[x%PLATFORM%] (
	set configurations=gcc-Debug gcc-Release clang-Debug clang-Release
) else (
	rem Don't test clang release builds on x86 as they haven't been build.
	set configurations=gcc-Debug gcc-Release clang-Debug
)

for %%c in (%configurations%) do (
	cd build-%PLATFORM%-%%~c
	cd test
	ctest -V || goto error
	cd .. 
	cd benchmark
	ctest -V || goto error
	cd ..
	cd ..
)
goto end

:error
echo Failed!
EXIT /b %ERRORLEVEL%

:end
endlocal
