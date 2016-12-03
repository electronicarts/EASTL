@echo off

rem Needed to check if a variable contains a string
@setlocal enableextensions enabledelayedexpansion

if [%PLATFORM%]==[MinGW_x86] set platform_string=i686
if [%PLATFORM%]==[MinGW_x64] set platform_string=x86_64

rem Checks if the platform variable contains MinGW
if not [x%PLATFORM:MinGW=%]==[x%PLATFORM%] (
	set "PATH=C:\msys64\usr\bin;%PATH%"
	C:\msys64\usr\bin\pacman --noconfirm --needed -S mingw-w64-%platform_string%-clang mingw-w64-x86_64-ninja
)

endlocal
