@echo off
setlocal EnableExtensions

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

if not defined VIDEO_PATH set "VIDEO_PATH=%ROOT%\original\youtube\Komplex - Forward (Java demo, 1998) [QkJK_voQBis].mp4"
if not defined JAVA_MANIFEST set "JAVA_MANIFEST=%ROOT%\documentation\reference-capture\java\manifest.csv"
if not defined OUTPUT_DIR set "OUTPUT_DIR=%ROOT%\documentation\reference-capture\reference"
if not defined VIDEO_OFFSET_MS set "VIDEO_OFFSET_MS=0"
if not defined VIDEO_FILTER set "VIDEO_FILTER=scale=512:256:flags=lanczos"

if exist "%JAVA_MANIFEST%" goto manifest_ok
echo Missing Java manifest:
echo   %JAVA_MANIFEST%
echo Run capture_forward_demo.bat first.
exit /b 1

:manifest_ok
if exist "%VIDEO_PATH%" goto video_ok
echo Missing reference video:
echo   %VIDEO_PATH%
exit /b 1

:video_ok

python --version >nul 2>nul
if errorlevel 1 (
  echo python was not found in PATH.
  exit /b 1
)

ffmpeg -version >nul 2>nul
if errorlevel 1 (
  echo ffmpeg was not found in PATH.
  exit /b 1
)

echo Forward reference video capture
echo   video: %VIDEO_PATH%
echo   manifest: %JAVA_MANIFEST%
echo   output: %OUTPUT_DIR%
echo   offset ms: %VIDEO_OFFSET_MS%
echo   filter: %VIDEO_FILTER%
echo.

python "%ROOT%\tools\forward_reference_compare.py" extract-video ^
  --video "%VIDEO_PATH%" ^
  --java-manifest "%JAVA_MANIFEST%" ^
  --output-dir "%OUTPUT_DIR%" ^
  --video-offset-ms %VIDEO_OFFSET_MS% ^
  --video-filter "%VIDEO_FILTER%" ^
  --overwrite
exit /b %errorlevel%
