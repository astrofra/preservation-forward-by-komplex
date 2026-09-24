@echo off
setlocal
set EMSDK_QUIET=1
pushd "%~dp0.."
if defined EMSDK goto :sdk
if exist ".local\emsdk\emsdk_env.bat" set "EMSDK=%CD%\.local\emsdk"
:sdk
if defined EMSDK call "%EMSDK%\emsdk_env.bat" >NUL
where emcmake >NUL 2>NUL
if errorlevel 1 (
    echo Activate Emscripten 6.0.10 or set EMSDK to your SDK directory first.
    popd
    exit /b 1
)
call emcmake cmake -S cpp-wasm -B build/wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto :fail
cmake --build build/wasm --parallel
if errorlevel 1 goto :fail
echo Browser bundle: build\wasm\web
popd
exit /b 0
:fail
popd
exit /b 1
