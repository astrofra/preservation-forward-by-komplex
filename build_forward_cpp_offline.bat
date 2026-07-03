@echo off
setlocal

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
pushd "%ROOT_DIR%" >NUL

if not defined BUILD_DIR set "BUILD_DIR=cpp-offline\build"

if "%~1"=="" (
    if not defined CONFIG set "CONFIG=Release"
) else (
    set "CONFIG=%~1"
)

if /I not "%CONFIG%"=="Release" if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="RelWithDebInfo" if /I not "%CONFIG%"=="MinSizeRel" (
    echo Usage: %~nx0 [Release^|Debug^|RelWithDebInfo^|MinSizeRel]
    popd >NUL
    exit /b 1
)

where cmake >NUL 2>NUL
if errorlevel 1 (
    echo CMake was not found in PATH.
    popd >NUL
    exit /b 1
)

echo [1/2] Configure CMake
cmake -S cpp-offline -B "%BUILD_DIR%"
if errorlevel 1 goto :fail

echo [2/2] Build cpp-offline ^(%CONFIG%^)
cmake --build "%BUILD_DIR%" --config "%CONFIG%"
if errorlevel 1 goto :fail

set "FORWARD_EXPORT=%BUILD_DIR%\%CONFIG%\forward-export.exe"
if exist "%FORWARD_EXPORT%" goto :success

set "FORWARD_EXPORT=%BUILD_DIR%\forward-export.exe"
if exist "%FORWARD_EXPORT%" goto :success

echo Build complete, but forward-export.exe was not found in the expected locations.
echo Checked:
echo   %BUILD_DIR%\%CONFIG%\forward-export.exe
echo   %BUILD_DIR%\forward-export.exe
popd >NUL
exit /b 1

:success
echo Build complete:
echo   %FORWARD_EXPORT%
popd >NUL
exit /b 0

:fail
echo Build failed.
popd >NUL
exit /b 1
