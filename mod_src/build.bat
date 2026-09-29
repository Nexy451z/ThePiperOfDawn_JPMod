@echo off
set "VSCMD_START_DIR=%~dp0"
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d "%~dp0"
if not exist "..\dist" mkdir "..\dist"
cl.exe /nologo /O2 /EHsc /MT /utf-8 /D_CRT_SECURE_NO_WARNINGS /LD version.cpp /link /DEF:version.def /OUT:"..\dist\version.dll" user32.lib
if errorlevel 1 exit /b 1
echo [SUCCESS] Built ..\dist\version.dll
