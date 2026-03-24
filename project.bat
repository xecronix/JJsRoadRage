@echo off
REM File: project.bat
REM Purpose: JJs Road Rage: The Musical workflow entry point

setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

set "PROJECT_DIR=%~dp0"
if "%PROJECT_DIR:~-1%"=="\" set "PROJECT_DIR=%PROJECT_DIR:~0,-1%"
set "PROJECT_NAME=JJsRoadRage"
set "EXE_NAME=JJsRoadRage.exe"
set "OUTPUT_DIR=output"
set "CRUDE_EXE=C:\CRUDE\crude.exe"
set "CRUDE_REPO=C:\CRUDE"
set "WINMERGE_EXE=C:\Program Files\WinMerge\WinMergeU.exe"

set "CCFLAGS_DEBUG=-Tx86-coff -std:C17 -Zi -Ob0 -fp:precise -W1 -Gd -Ze -Zc-"
set "CCFLAGS_RELEASE=-Tx86-coff -std:C17 -Ot -Ob1 -fp:precise -W1 -Gd -Zc- -Ze"
set "LINKFLAGS_DEBUG=-machine:x86 -subsystem:console -safeseh -debug -debugtype:po kernel32.lib advapi32.lib delayimp.lib raylibdll.lib opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib"
set "LINKFLAGS_RELEASE=-machine:x86 -subsystem:console -safeseh kernel32.lib advapi32.lib delayimp.lib raylibdll.lib opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib"

if "%~1"=="" goto build_debug
if /I "%~1"=="build_debug" goto build_debug
if /I "%~1"=="build_release" goto build_release
if /I "%~1"=="rebuild_debug" goto rebuild_debug
if /I "%~1"=="rebuild_release" goto rebuild_release
if /I "%~1"=="run" goto run
if /I "%~1"=="clean" goto clean
if /I "%~1"=="crude" goto crude
if /I "%~1"=="diff" goto diff
if /I "%~1"=="help" goto help

echo Unknown target: %~1
goto help

:prepare_env
call "%PROJECT_DIR%\env_pelles32.bat"
if errorlevel 1 exit /b 1

pushd "%PROJECT_DIR%"
if errorlevel 1 exit /b 1

set "INCLUDE=%INCLUDE%;%PROJECT_DIR%\deps\include;%PROJECT_DIR%\src\include"
set "LIB=%LIB%;%PROJECT_DIR%\deps\lib"

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
if errorlevel 1 (
    echo ERROR: Failed to create output directory.
    popd
    exit /b 1
)

exit /b 0

:build_debug
call :clean_internal || exit /b 1
set "ACTIVE_CCFLAGS=%CCFLAGS_DEBUG%"
set "ACTIVE_LINKFLAGS=%LINKFLAGS_DEBUG%"
call :do_build || exit /b 1
exit /b 0

:build_release
call :clean_internal || exit /b 1
set "ACTIVE_CCFLAGS=%CCFLAGS_RELEASE%"
set "ACTIVE_LINKFLAGS=%LINKFLAGS_RELEASE%"
call :do_build || exit /b 1
exit /b 0

:rebuild_debug
call :build_debug
exit /b %errorlevel%

:rebuild_release
call :build_release
exit /b %errorlevel%

:do_build
call :prepare_env || exit /b 1

echo Building %PROJECT_NAME%...

call :compile_one "main.c" || goto build_fail

for %%F in (src\*.c) do (
    call :compile_one "%%F" || goto build_fail
)

set "OBJLIST="
for /f "delims=" %%F in ('dir /b "%OUTPUT_DIR%\*.obj" 2^>nul') do (
    set "OBJLIST=!OBJLIST! "%OUTPUT_DIR%\%%F""
)

if not defined OBJLIST (
    echo ERROR: No object files were produced.
    goto build_fail
)

echo Linking %EXE_NAME%...
polink %ACTIVE_LINKFLAGS% -out:"%EXE_NAME%" !OBJLIST!
if errorlevel 1 goto build_fail

echo Build complete: %EXE_NAME%
popd
exit /b 0

:build_fail
echo Build failed.
popd
exit /b 1

:compile_one
echo Compiling %~1 ...
pocc %ACTIVE_CCFLAGS% "%~1" /Fo"%OUTPUT_DIR%\%~n1.obj"
if errorlevel 1 exit /b 1
exit /b 0

:run
pushd "%PROJECT_DIR%"
if not exist "%EXE_NAME%" (
    echo ERROR: %EXE_NAME% not found. Run build_debug or build_release first.
    popd
    exit /b 1
)
start "" "%PROJECT_DIR%\%EXE_NAME%"
popd
exit /b 0

:clean
call :clean_internal
exit /b %errorlevel%

:clean_internal
pushd "%PROJECT_DIR%"
if errorlevel 1 exit /b 1

echo Cleaning...
del /q "%OUTPUT_DIR%\*.obj" 2>nul
del /q "%EXE_NAME%" 2>nul

popd
exit /b 0

:crude
call :clean_internal || exit /b 1
if not exist "%CRUDE_EXE%" (
    echo ERROR: CRUDE not found:
    echo %CRUDE_EXE%
    exit /b 1
)
"%CRUDE_EXE%" "%PROJECT_DIR%" "%CRUDE_REPO%"
exit /b %errorlevel%

:diff
call :clean_internal || exit /b 1

if not exist "%WINMERGE_EXE%" (
    echo ERROR: WinMerge not found:
    echo %WINMERGE_EXE%
    exit /b 1
)

set "REVISIONS_DIR=%CRUDE_REPO%\%PROJECT_NAME%\revisions"
if not exist "%REVISIONS_DIR%" (
    echo ERROR: Revisions folder not found:
    echo %REVISIONS_DIR%
    exit /b 1
)

set "LATEST_REV="
for /f "delims=" %%D in ('dir "%REVISIONS_DIR%" /b /ad /o:-n 2^>nul') do (
    set "LATEST_REV=%%D"
    goto found_latest
)

:found_latest
if not defined LATEST_REV (
    echo ERROR: No revisions found in:
    echo %REVISIONS_DIR%
    exit /b 1
)

set "LATEST_DIR=%REVISIONS_DIR%\%LATEST_REV%"

echo Comparing:
echo   CURRENT: %PROJECT_DIR%
echo   REVISION: %LATEST_DIR%
start "" "%WINMERGE_EXE%" "%PROJECT_DIR%" "%LATEST_DIR%"
exit /b 0

:help
echo.
echo Usage:
echo     %~n0 build_debug
echo     %~n0 build_release
echo     %~n0 rebuild_debug
echo     %~n0 rebuild_release
echo     %~n0 run
echo     %~n0 clean
echo     %~n0 crude
echo     %~n0 diff
echo     %~n0 help
echo.
exit /b 1
