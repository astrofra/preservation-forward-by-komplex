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
set "KUKOT_OUTPUT=%TEMP_DIR%\kukot"
set "MAKU_OUTPUT=%TEMP_DIR%\maku"

if not defined INTRO_END_POSITION set "INTRO_END_POSITION=0x1024"
if not defined INTRO_POST_ROLL_FRAMES set "INTRO_POST_ROLL_FRAMES=12"

if not defined SAARI_END_POSITION set "SAARI_END_POSITION=0x0700"
if not defined SAARI_POST_ROLL_FRAMES set "SAARI_POST_ROLL_FRAMES=0"

if not defined KUKOT_END_POSITION set "KUKOT_END_POSITION=0x0D00"
if not defined KUKOT_POST_ROLL_FRAMES set "KUKOT_POST_ROLL_FRAMES=0"

if not defined MAKU_END_POSITION set "MAKU_END_POSITION=0x1000"
if not defined MAKU_POST_ROLL_FRAMES set "MAKU_POST_ROLL_FRAMES=0"

if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

echo [1/8] Configure CMake
cmake -S cpp-offline -B "%BUILD_DIR%"
if errorlevel 1 goto :fail

echo [2/8] Build exporter
cmake --build "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 goto :fail

echo [3/8] Export intro segment
echo         end=%INTRO_END_POSITION% post_roll_frames=%INTRO_POST_ROLL_FRAMES%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence intro ^
  --output "%INTRO_OUTPUT%" ^
  --until-song-position %INTRO_END_POSITION% ^
  --post-roll-frames %INTRO_POST_ROLL_FRAMES%
if errorlevel 1 goto :fail

echo [4/8] Export saari segment
echo         end=%SAARI_END_POSITION% post_roll_frames=%SAARI_POST_ROLL_FRAMES%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence saari ^
  --output "%SAARI_OUTPUT%" ^
  --until-song-position %SAARI_END_POSITION% ^
  --post-roll-frames %SAARI_POST_ROLL_FRAMES%
if errorlevel 1 goto :fail

echo [5/8] Export kukot segment
echo         end=%KUKOT_END_POSITION% post_roll_frames=%KUKOT_POST_ROLL_FRAMES%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence kukot ^
  --output "%KUKOT_OUTPUT%" ^
  --until-song-position %KUKOT_END_POSITION% ^
  --post-roll-frames %KUKOT_POST_ROLL_FRAMES%
if errorlevel 1 goto :fail

echo [6/8] Export maku segment
echo         end=%MAKU_END_POSITION% post_roll_frames=%MAKU_POST_ROLL_FRAMES%
"%BUILD_DIR%\%CONFIG%\forward-export.exe" ^
  --sequence maku ^
  --output "%MAKU_OUTPUT%" ^
  --until-song-position %MAKU_END_POSITION% ^
  --post-roll-frames %MAKU_POST_ROLL_FRAMES%
if errorlevel 1 goto :fail

echo [7/8] Merge current full output
powershell -NoProfile -ExecutionPolicy Bypass -File ^
  "cpp-offline\scripts\merge_current_full_outputs.ps1" ^
  -OutputDir "%OUTPUT_DIR%" ^
  -IntroDir "%INTRO_OUTPUT%" ^
  -SaariDir "%SAARI_OUTPUT%" ^
  -KukotDir "%KUKOT_OUTPUT%" ^
  -MakuDir "%MAKU_OUTPUT%" ^
  -Fps 50 ^
  -SampleRate 22050
if errorlevel 1 goto :fail

if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

where ffmpeg >NUL 2>NUL
if errorlevel 1 goto :done

echo [8/8] Mux master and h264 copies
call cpp-offline\scripts\mux_master.bat "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_full_current_master.mkv" 50
if errorlevel 1 goto :fail
call cpp-offline\scripts\mux_h264.bat "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_full_current_h264.mp4" 50
if errorlevel 1 goto :fail

:done
echo Export complete: %OUTPUT_DIR%
popd >NUL
exit /b 0

:fail
echo Export failed.
popd >NUL
exit /b 1
