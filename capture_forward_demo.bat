@echo off
setlocal EnableExtensions

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

rem Output is relative to the repository root when passed to run_forward_desktop.bat.
if not defined CAPTURE_DIR set "CAPTURE_DIR=documentation\reference-capture\java"
if not defined CAPTURE_INTERVAL_MS set "CAPTURE_INTERVAL_MS=4000"
if not defined CAPTURE_START_MS set "CAPTURE_START_MS=0"
if not defined CAPTURE_STOP_MS set "CAPTURE_STOP_MS=300000"
if not defined CAPTURE_LIMIT set "CAPTURE_LIMIT=250"
if not defined ENABLE_NOSOUND set "ENABLE_NOSOUND=0"
if not defined ENABLE_CAPTURE_EXIT set "ENABLE_CAPTURE_EXIT=1"

echo Forward Java self-capture
echo   output: %CAPTURE_DIR%
echo   interval ms: %CAPTURE_INTERVAL_MS%
echo   start ms: %CAPTURE_START_MS%
echo   stop ms: %CAPTURE_STOP_MS%
echo   limit: %CAPTURE_LIMIT%
echo   nosound: %ENABLE_NOSOUND%
echo.

if not exist "%ROOT%\documentation\reference-capture" mkdir "%ROOT%\documentation\reference-capture"
if errorlevel 1 exit /b %errorlevel%

set "ARGS="
if "%ENABLE_NOSOUND%"=="1" set "ARGS=%ARGS% nosound 1"
set "ARGS=%ARGS% capture %CAPTURE_DIR%"
set "ARGS=%ARGS% captureintervalms %CAPTURE_INTERVAL_MS%"
set "ARGS=%ARGS% capturestartms %CAPTURE_START_MS%"
set "ARGS=%ARGS% capturestopms %CAPTURE_STOP_MS%"
set "ARGS=%ARGS% capturelimit %CAPTURE_LIMIT%"
if "%ENABLE_CAPTURE_EXIT%"=="1" set "ARGS=%ARGS% captureexit 1"

call "%ROOT%\run_forward_desktop.bat" %ARGS%
exit /b %errorlevel%
