@echo off
setlocal

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
pushd "%ROOT_DIR%" >NUL

python "cpp-offline\scripts\package_cpp_release.py" %*
set "EXIT_CODE=%ERRORLEVEL%"

popd >NUL
exit /b %EXIT_CODE%
