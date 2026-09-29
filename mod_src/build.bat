@echo off
setlocal enabledelayedexpansion
set "VSCMD_START_DIR=%~dp0"

rem ---- Locate Visual Studio (any edition/version) via vswhere ----
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "!VSWHERE!" set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"

set "VSPATH="
if exist "!VSWHERE!" for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%i"

rem ---- Fallback: common default install locations ----
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\2022\Community"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\2022\Professional"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\18\Community"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\18\Professional"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\18\Enterprise"
if not defined VSPATH call :TryPath "%ProgramFiles%\Microsoft Visual Studio\18\BuildTools"

if not defined VSPATH (
    echo [ERROR] Visual Studio with the C++ toolset was not found.
    echo         Install "Desktop development with C++" ^(Visual Studio or Build Tools^).
    exit /b 1
)

echo [INFO] Using MSVC from: !VSPATH!
call "!VSPATH!\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo [ERROR] Failed to initialize the MSVC x64 environment.
    exit /b 1
)

cd /d "%~dp0"
if not exist "..\dist" mkdir "..\dist"
cl.exe /nologo /O2 /EHsc /MT /utf-8 /D_CRT_SECURE_NO_WARNINGS /LD version.cpp /link /DEF:version.def /OUT:"..\dist\version.dll" user32.lib
if errorlevel 1 exit /b 1
echo [SUCCESS] Built ..\dist\version.dll
exit /b 0

:TryPath
if not exist "%~1\VC\Auxiliary\Build\vcvars64.bat" exit /b 0
set "VSPATH=%~1"
exit /b 0
