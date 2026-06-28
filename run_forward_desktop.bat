@echo off
setlocal EnableExtensions

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "JAVA_PROJECT=%ROOT%\java-desktop"
set "SRC_DIR=%JAVA_PROJECT%\src\main\java"
set "BUILD_ROOT=%JAVA_PROJECT%\build"
set "BUILD_DIR=%BUILD_ROOT%\classes"
set "SOURCES_FILE=%BUILD_ROOT%\sources.txt"

if not exist "%BUILD_ROOT%" mkdir "%BUILD_ROOT%"
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Get-ChildItem -Recurse -Path '%SRC_DIR%' -Filter *.java | ForEach-Object { $_.FullName } | Set-Content -Encoding ASCII '%SOURCES_FILE%'"
if errorlevel 1 exit /b %errorlevel%

javac -d "%BUILD_DIR%" @"%SOURCES_FILE%"
if errorlevel 1 exit /b %errorlevel%

pushd "%ROOT%\original\forward"
java "-Dforward.repoRoot=%ROOT%" -cp "%BUILD_DIR%" ForwardDesktopLauncher %*
set "EXIT_CODE=%ERRORLEVEL%"
popd

exit /b %EXIT_CODE%
