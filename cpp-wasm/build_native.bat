@echo off
setlocal
pushd "%~dp0.."
cmake -S cpp-wasm -B build/player
if errorlevel 1 goto :fail
cmake --build build/player --config Release --parallel
if errorlevel 1 goto :fail
echo Native player: build\player\Release\forward-player.exe
popd
exit /b 0
:fail
popd
exit /b 1
