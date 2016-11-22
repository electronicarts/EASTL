@echo off

rem Needed to check if a variable contains a string
@setlocal enableextensions enabledelayedexpansion

rem Check if the platform variable contains MinGW
if not [x%PLATFORM:MinGW=%]==[x%PLATFORM%] (
	goto :build-mingw
) else (
	goto :build-msvc
)

:build-msvc
mkdir build
cd build

rem Check if the platform variable contains 2013
if not [x%PLATFORM:2013=%]==[x%PLATFORM%] (
	set vs_generator=Visual Studio 12 2013
) else (
	set vs_generator=Visual Studio 14 2015
)

rem Check if the platform variable contains x64
if not [x%PLATFORM:x64=%]==[x%PLATFORM%] set vs_generator=%vs_generator% Win64
cmake .. -G "%vs_generator%" -DEASTL_BUILD_BENCHMARK:BOOL=ON -DEASTL_BUILD_TESTS:BOOL=ON 
cmake --build . --config Debug || goto error
cmake --build . --config Release || goto error
goto :end


:build-mingw
rem Start build for gcc and clang.
set "OLDPATH=%PATH%"

if not [x%PLATFORM:x86=%]==[x%PLATFORM%] (
	set bin_folder=C:/msys64/mingw32/bin
	set "PATH=C:\msys64\mingw32\bin;%OLDPATH%"
) else (
	set bin_folder=C:/msys64/mingw64/bin
	set "PATH=C:\msys64\mingw64\bin;%OLDPATH%"
)

mkdir build-%PLATFORM%-gcc-Debug
mkdir build-%PLATFORM%-gcc-Release
mkdir build-%PLATFORM%-clang-Release
mkdir build-%PLATFORM%-clang-Debug

rem Build with gcc.
for %%c in (Debug, Release) do (
	cd build-%PLATFORM%-gcc-%%~c
	cmake .. -GNinja -DCMAKE_BUILD_TYPE=%%~c -DEASTL_BUILD_BENCHMARK:BOOL=ON -DEASTL_BUILD_TESTS:BOOL=ON -DCMAKE_C_COMPILER=%bin_folder%/gcc.exe -DCMAKE_CXX_COMPILER=%bin_folder%/g++.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/ninja.exe
	cmake --build . || goto error
	cd ..
)

rem Build with clang.
if not [x%PLATFORM:x64=%]==[x%PLATFORM%] (
	set configurations=Debug Release
) else (
	rem Don't build Release when using clang
	set configurations=Debug
)

for %%c in (%configurations%) do (
	cd build-%PLATFORM%-clang-%%~c
	cmake .. -GNinja -DCMAKE_BUILD_TYPE=%%~c -DEASTL_BUILD_BENCHMARK:BOOL=ON -DEASTL_BUILD_TESTS:BOOL=ON -DCMAKE_C_COMPILER=%bin_folder%/clang.exe -DCMAKE_CXX_COMPILER=%bin_folder%/clang++.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/ninja.exe
	cmake --build . || goto error
	cd ..
)

goto end

:error
echo Failed!
EXIT /b %ERRORLEVEL%

:end
endlocal
