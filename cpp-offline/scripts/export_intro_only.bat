@echo off
setlocal

set "ROOT_DIR=%~dp0..\.."
pushd "%ROOT_DIR%" >NUL

if not defined BUILD_DIR set "BUILD_DIR=cpp-offline\build"
if not defined CONFIG set "CONFIG=Release"
if not defined OUTPUT_DIR set "OUTPUT_DIR=cpp-offline\output-intro-only"
if not defined SEQUENCE set "SEQUENCE=intro"
if not defined INTRO_FRAMES_PER_ROW set "INTRO_FRAMES_PER_ROW=6"
if not defined INTRO_ROWS_PER_ORDER set "INTRO_ROWS_PER_ORDER=64"
if not defined INTRO_END_POSITION set "INTRO_END_POSITION=0x1024"
if not defined POST_ROLL_FRAMES set "POST_ROLL_FRAMES=12"

set /a "INTRO_END_ORDER=(%INTRO_END_POSITION% >> 8)"
set /a "INTRO_END_ROW=(%INTRO_END_POSITION% & 0xFF)"
set /a "INTRO_TOTAL_ROWS=(INTRO_END_ORDER * INTRO_ROWS_PER_ORDER) + INTRO_END_ROW"
set /a "INTRO_TOTAL_FRAMES=(INTRO_TOTAL_ROWS * INTRO_FRAMES_PER_ROW) + POST_ROLL_FRAMES"

echo [1/4] Configure CMake
cmake -S cpp-offline -B "%BUILD_DIR%"
if errorlevel 1 goto :fail

echo [2/4] Build exporter
cmake --build "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 goto :fail

echo [3/4] Export intro sequence
echo         frames=%INTRO_TOTAL_FRAMES% rows=%INTRO_TOTAL_ROWS% end=%INTRO_END_POSITION%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence %SEQUENCE% ^
  --output "%OUTPUT_DIR%" ^
  --frames %INTRO_TOTAL_FRAMES% ^
  --intro-frames-per-row %INTRO_FRAMES_PER_ROW% ^
  --intro-rows-per-order %INTRO_ROWS_PER_ORDER%
if errorlevel 1 goto :fail

where ffmpeg >NUL 2>NUL
if errorlevel 1 goto :done

echo [4/4] Mux master and h264 copies
call cpp-offline\scripts\mux_master.bat "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_intro_master.mkv" 50
if errorlevel 1 goto :fail
call cpp-offline\scripts\mux_h264.bat "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_intro_h264.mp4" 50
if errorlevel 1 goto :fail

:done
echo Export complete: %OUTPUT_DIR%
popd >NUL
exit /b 0

:fail
echo Export failed.
popd >NUL
exit /b 1
