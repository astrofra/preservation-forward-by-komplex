@echo off
setlocal EnableDelayedExpansion

set "ROOT_DIR=%~dp0..\.."
pushd "%ROOT_DIR%" >NUL

set "TOP_INPUT=%~1"
if "%TOP_INPUT%"=="" set "TOP_INPUT=java-desktop\video\forward-by-komplex-java.mp4"

set "BOTTOM_INPUT=%~2"
if "%BOTTOM_INPUT%"=="" set "BOTTOM_INPUT=cpp-offline\output-full-current\forward_full_current_h264.mp4"

set "OUTPUT_FILE=%~3"
if "%OUTPUT_FILE%"=="" set "OUTPUT_FILE=cpp-offline\output-full-current\forward_full_current_top_bottom.mp4"

set "FONT_FILE=%WINDIR:\=/%"
set "FONT_FILE=%FONT_FILE::=\:%/Fonts/arial.ttf"

where ffmpeg >NUL 2>NUL
if errorlevel 1 (
  echo ffmpeg not found in PATH.
  popd >NUL
  exit /b 1
)

where ffprobe >NUL 2>NUL
if errorlevel 1 (
  echo ffprobe not found in PATH.
  popd >NUL
  exit /b 1
)

if not exist "%TOP_INPUT%" (
  echo Top input not found: %TOP_INPUT%
  popd >NUL
  exit /b 1
)

if not exist "%BOTTOM_INPUT%" (
  echo Bottom input not found: %BOTTOM_INPUT%
  popd >NUL
  exit /b 1
)

set "BOTTOM_WIDTH="
set "BOTTOM_HEIGHT="
set "STACK_FPS="
for /f %%A in ('ffprobe -v error -select_streams v:0 -show_entries stream^=width -of default^=noprint_wrappers^=1:nokey^=1 "%BOTTOM_INPUT%"') do (
  set "BOTTOM_WIDTH=%%A"
)
for /f %%A in ('ffprobe -v error -select_streams v:0 -show_entries stream^=height -of default^=noprint_wrappers^=1:nokey^=1 "%BOTTOM_INPUT%"') do (
  set "BOTTOM_HEIGHT=%%A"
)
for /f %%A in ('ffprobe -v error -select_streams v:0 -show_entries stream^=avg_frame_rate -of default^=noprint_wrappers^=1:nokey^=1 "%BOTTOM_INPUT%"') do (
  set "STACK_FPS=%%A"
)

if not defined BOTTOM_WIDTH (
  echo Could not resolve bottom input dimensions.
  popd >NUL
  exit /b 1
)

if not defined BOTTOM_HEIGHT (
  echo Could not resolve bottom input dimensions.
  popd >NUL
  exit /b 1
)

if not defined STACK_FPS (
  echo Could not resolve bottom input frame rate.
  popd >NUL
  exit /b 1
)

if "!STACK_FPS!"=="0/0" (
  echo Invalid bottom input frame rate: !STACK_FPS!
  popd >NUL
  exit /b 1
)

set /a STACK_WIDTH=BOTTOM_WIDTH
set /a STACK_HEIGHT=BOTTOM_HEIGHT

if !STACK_WIDTH! LEQ 0 (
  echo Invalid computed stack width: !STACK_WIDTH!
  popd >NUL
  exit /b 1
)

if !STACK_HEIGHT! LEQ 0 (
  echo Invalid computed stack height: !STACK_HEIGHT!
  popd >NUL
  exit /b 1
)

for %%I in ("%OUTPUT_FILE%") do (
  if not exist "%%~dpI" mkdir "%%~dpI"
)

ffmpeg -y ^
  -i "%TOP_INPUT%" ^
  -i "%BOTTOM_INPUT%" ^
  -filter_complex "[0:v]scale=w=!STACK_WIDTH!:h=!STACK_HEIGHT!:flags=lanczos,drawtext=fontfile='%FONT_FILE%':text='Java version':x=12:y=h-th-10:fontsize=16:fontcolor=white:borderw=2:bordercolor=black@0.8[top];[1:v]scale=w=!STACK_WIDTH!:h=!STACK_HEIGHT!:flags=lanczos,drawtext=fontfile='%FONT_FILE%':text='Cpp version':x=12:y=h-th-10:fontsize=16:fontcolor=white:borderw=2:bordercolor=black@0.8[bottom];[top][bottom]vstack=inputs=2:shortest=1,fps=!STACK_FPS!,format=yuv420p[v]" ^
  -map "[v]" ^
  -map 1:a:0 ^
  -c:v libx264 ^
  -preset slow ^
  -crf 22 ^
  -c:a copy ^
  -shortest ^
  "%OUTPUT_FILE%"

set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
  echo Top-bottom stack failed.
  popd >NUL
  exit /b %EXIT_CODE%
)

echo Top-bottom stack written to: %OUTPUT_FILE%
popd >NUL
exit /b 0
