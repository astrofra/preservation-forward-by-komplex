@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%.") do set "REPO_ROOT=%%~fI"

set "SRC_DIR=%REPO_ROOT%\port"
set "BUILD_DIR=%REPO_ROOT%\build\Release64"
set "EXE_DIR=%BUILD_DIR%\Release"
set "EXE_PATH=%EXE_DIR%\forward_native.exe"
set "TOOLCHAIN_FILE="

if not exist "%SRC_DIR%\CMakeLists.txt" (
  call :fail "Could not find %SRC_DIR%\CMakeLists.txt." 1
  exit /b 1
)

call :require_tool cmake.exe CMake || (
  call :fail "CMake [cmake.exe] is required but was not found in PATH." 1
  exit /b 1
)

if defined CMAKE_TOOLCHAIN_FILE (
  if exist "%CMAKE_TOOLCHAIN_FILE%" (
    set "TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE%"
    echo [INFO] Using toolchain from CMAKE_TOOLCHAIN_FILE.
  ) else (
    echo [WARN] CMAKE_TOOLCHAIN_FILE is set but file does not exist:
    echo [WARN]   %CMAKE_TOOLCHAIN_FILE%
  )
)

if not defined TOOLCHAIN_FILE (
  if defined VCPKG_ROOT (
    if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
      set "TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
      echo [INFO] Using toolchain from VCPKG_ROOT.
    )
  )
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo [INFO] Configuring CMake (Visual Studio 2022, x64, Release)...
if defined TOOLCHAIN_FILE (
  cmake -S "%SRC_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DFORWARD_FETCH_SDL2=ON ^
    -DFORWARD_ENABLE_XM_AUDIO=ON ^
    -DFORWARD_FETCH_LIBXMP=ON ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%"
) else (
  cmake -S "%SRC_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DFORWARD_FETCH_SDL2=ON ^
    -DFORWARD_ENABLE_XM_AUDIO=ON ^
    -DFORWARD_FETCH_LIBXMP=ON
)
if errorlevel 1 (
  call :fail "CMake configure failed." 1
  exit /b 1
)

echo [INFO] Building Release...
cmake --build "%BUILD_DIR%" --config Release -- /m
if errorlevel 1 (
  call :fail "Build failed." 1
  exit /b 1
)

if not exist "%EXE_PATH%" (
  call :fail "Build completed, but executable was not found at %EXE_PATH%." 2
  exit /b 2
)

echo [OK] Build complete.
echo [OK] Executable: "%EXE_PATH%"
echo [INFO] Use prepare_release64.bat to assemble a redistributable package under dist\.
exit /b 0

:require_tool
where "%~1" >nul 2>nul
if errorlevel 1 (
  exit /b 1
)
exit /b 0

:fail
echo [ERROR] %~1
echo.
pause
exit /b %~2
