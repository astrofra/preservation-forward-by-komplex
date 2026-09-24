@echo off
setlocal

pushd "%~dp0" >NUL
if errorlevel 1 exit /b 1

call "cpp-wasm\build_web.bat"
if errorlevel 1 goto :fail

python "cpp-wasm\scripts\package_web_release.py"
if errorlevel 1 goto :fail

popd >NUL
exit /b 0

:fail
echo Web distribution preparation failed. See the error above.
popd >NUL
exit /b 1
