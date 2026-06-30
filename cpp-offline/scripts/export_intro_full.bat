@echo off
setlocal

set "ROOT_DIR=%~dp0..\.."
pushd "%ROOT_DIR%" >NUL

if not defined BUILD_DIR set "BUILD_DIR=cpp-offline\build"
if not defined CONFIG set "CONFIG=Release"
if not defined OUTPUT_DIR set "OUTPUT_DIR=cpp-offline\output-full-current"
set "TEMP_DIR=%OUTPUT_DIR%-tmp"
set "INTRO_OUTPUT=%TEMP_DIR%\intro"
set "SAARI_OUTPUT=%TEMP_DIR%\saari"

if not defined INTRO_FRAMES_PER_ROW set "INTRO_FRAMES_PER_ROW=6"
if not defined INTRO_ROWS_PER_ORDER set "INTRO_ROWS_PER_ORDER=64"
if not defined INTRO_END_POSITION set "INTRO_END_POSITION=0x1024"
if not defined INTRO_POST_ROLL_FRAMES set "INTRO_POST_ROLL_FRAMES=12"

if not defined SAARI_FRAMES_PER_ROW set "SAARI_FRAMES_PER_ROW=7"
if not defined SAARI_ROWS_PER_ORDER set "SAARI_ROWS_PER_ORDER=64"
if not defined SAARI_END_POSITION set "SAARI_END_POSITION=0x0700"
if not defined SAARI_POST_ROLL_FRAMES set "SAARI_POST_ROLL_FRAMES=0"

set /a "INTRO_END_ORDER=(%INTRO_END_POSITION% >> 8)"
set /a "INTRO_END_ROW=(%INTRO_END_POSITION% & 0xFF)"
set /a "INTRO_TOTAL_ROWS=(INTRO_END_ORDER * INTRO_ROWS_PER_ORDER) + INTRO_END_ROW"
set /a "INTRO_TOTAL_FRAMES=(INTRO_TOTAL_ROWS * INTRO_FRAMES_PER_ROW) + INTRO_POST_ROLL_FRAMES"
set /a "SAARI_END_ORDER=(%SAARI_END_POSITION% >> 8)"
set /a "SAARI_END_ROW=(%SAARI_END_POSITION% & 0xFF)"
set /a "SAARI_TOTAL_ROWS=(SAARI_END_ORDER * SAARI_ROWS_PER_ORDER) + SAARI_END_ROW"
if not defined SAARI_FRAMES set /a "SAARI_FRAMES=(SAARI_TOTAL_ROWS * SAARI_FRAMES_PER_ROW) + SAARI_POST_ROLL_FRAMES"
set /a "TOTAL_FRAMES=INTRO_TOTAL_FRAMES + SAARI_FRAMES"

if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

echo [1/6] Configure CMake
cmake -S cpp-offline -B "%BUILD_DIR%"
if errorlevel 1 goto :fail

echo [2/6] Build exporter
cmake --build "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 goto :fail

echo [3/6] Export intro segment
echo         frames=%INTRO_TOTAL_FRAMES% rows=%INTRO_TOTAL_ROWS% end=%INTRO_END_POSITION%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence intro ^
  --output "%INTRO_OUTPUT%" ^
  --frames %INTRO_TOTAL_FRAMES% ^
  --intro-frames-per-row %INTRO_FRAMES_PER_ROW% ^
  --intro-rows-per-order %INTRO_ROWS_PER_ORDER%
if errorlevel 1 goto :fail

echo [4/6] Export saari segment
echo         frames=%SAARI_FRAMES% rows=%SAARI_TOTAL_ROWS% end=%SAARI_END_POSITION% frames_per_row=%SAARI_FRAMES_PER_ROW%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence saari ^
  --output "%SAARI_OUTPUT%" ^
  --frames %SAARI_FRAMES% ^
  --intro-frames-per-row %SAARI_FRAMES_PER_ROW% ^
  --intro-rows-per-order %SAARI_ROWS_PER_ORDER%
if errorlevel 1 goto :fail

echo [5/6] Merge current full output
powershell -NoProfile -ExecutionPolicy Bypass -File ^
  "cpp-offline\scripts\merge_current_full_outputs.ps1" ^
  -OutputDir "%OUTPUT_DIR%" ^
  -IntroDir "%INTRO_OUTPUT%" ^
  -SaariDir "%SAARI_OUTPUT%" ^
  -Fps 50 ^
  -SampleRate 22050
if errorlevel 1 goto :fail

if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

where ffmpeg >NUL 2>NUL
if errorlevel 1 goto :done

echo [6/6] Mux master and h264 copies
call cpp-offline\scripts\mux_master.bat "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_full_current_master.mkv" 50
if errorlevel 1 goto :fail
call cpp-offline\scripts\mux_h264.bat "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_full_current_h264.mp4" 50
if errorlevel 1 goto :fail

:done
echo Export complete: %OUTPUT_DIR% (%TOTAL_FRAMES% frames)
popd >NUL
exit /b 0

:fail
echo Export failed.
popd >NUL
exit /b 1
