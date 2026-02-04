@echo off
setlocal

set ROOT=%~dp0
set ROOT=%ROOT:~0,-1%

set MINGW=C:\Program Files\JetBrains\CLion 2025.1.1\bin\mingw\bin
set CMAKE=C:\Program Files\JetBrains\CLion 2025.1.1\bin\cmake\win\x64\bin\cmake.exe

set PATH=%MINGW%;%PATH%

mkdir "%ROOT%\cmake-build-debug" 2>nul
cd "%ROOT%\cmake-build-debug"

"%CMAKE%" -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  "%ROOT%"

mingw32-make
