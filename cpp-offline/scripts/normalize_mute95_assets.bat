@echo off
setlocal

set "ROOT_DIR=%~dp0..\.."
pushd "%ROOT_DIR%" >NUL

set "CLASS_DIR=java-desktop\build\offline-normalizer"
set "OUTPUT_DIR=cpp-offline\assets\mute95"
set "ASSET_ROOT=original\forward"

if not exist "%CLASS_DIR%" mkdir "%CLASS_DIR%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

echo [1/2] Compile Java normalizer
javac -d "%CLASS_DIR%" java-desktop\src\main\java\*.java
if errorlevel 1 goto :fail

echo [2/2] Decode mute95 assets with Java runtime path
java -cp "%CLASS_DIR%" ForwardOfflineAssetNormalizer mute95 "%ASSET_ROOT%" "%OUTPUT_DIR%"
if errorlevel 1 goto :fail

echo Normalized assets ready: %OUTPUT_DIR%
popd >NUL
exit /b 0

:fail
echo Asset normalization failed.
popd >NUL
exit /b 1
