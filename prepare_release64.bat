@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%.") do set "REPO_ROOT=%%~fI"

set "BUILD_DIR=%REPO_ROOT%\build\Release64"
set "DIST_DIR=%REPO_ROOT%\dist\forward-native-win64"
set "PACKAGE_EXE=%DIST_DIR%\forward_native.exe"
set "PACKAGE_MOD=%DIST_DIR%\original\forward\mods\kuninga.xm"

call "%REPO_ROOT%\build_release64.bat"
if errorlevel 1 (
  exit /b 1
)

if exist "%DIST_DIR%" (
  echo [INFO] Removing previous package at "%DIST_DIR%".
  rmdir /s /q "%DIST_DIR%"
  if errorlevel 1 (
    call :fail "Failed to remove previous package directory %DIST_DIR%." 1
    exit /b 1
  )
)

echo [INFO] Installing release package into "%DIST_DIR%".
cmake --install "%BUILD_DIR%" --config Release --prefix "%DIST_DIR%"
if errorlevel 1 (
  call :fail "CMake install failed." 1
  exit /b 1
)

if not exist "%PACKAGE_EXE%" (
  call :fail "Package completed, but %PACKAGE_EXE% is missing." 2
  exit /b 2
)

if not exist "%PACKAGE_MOD%" (
  call :fail "Package completed, but expected asset %PACKAGE_MOD% is missing." 2
  exit /b 2
)

echo [OK] Release package ready.
echo [OK] Folder: "%DIST_DIR%"
exit /b 0

:fail
echo [ERROR] %~1
echo.
pause
exit /b %~2
