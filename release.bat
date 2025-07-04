@echo off
cls
setlocal enabledelayedexpansion

rem Get the escape character
for /f "delims=" %%A in ('echo prompt $E^| cmd') do set "ESC=%%A"

cmake -S . -B Build\Release -G "MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
	rem Print bright red text
	echo !ESC![91mCMake configuration failed !ESC!
	rem Reset formatting explicitly at end of script
	echo !ESC![0m
    exit /b %ERRORLEVEL%
)

cmake --build Build\Release --config Release

if %ERRORLEVEL% neq 0 (
	rem Print bright red text
	echo !ESC![91mCMake configuration failed !ESC!
	rem Reset formatting explicitly at end of script
	echo !ESC![0m
    exit /b %ERRORLEVEL%
)

cd Build\Release\Run
move /y ..\compile_commands.json ..\..

Rem Lauch
MyProject.exe
cd ..\..\..

endlocal

