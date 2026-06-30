@echo off
setlocal

set "OUTPUT_DIR=%~1"
if "%OUTPUT_DIR%"=="" set "OUTPUT_DIR=output"

set "OUTPUT_FILE=%~2"
if "%OUTPUT_FILE%"=="" set "OUTPUT_FILE=%OUTPUT_DIR%\forward_master.mkv"

set "FPS=%~3"
if "%FPS%"=="" set "FPS=50"

ffmpeg -y ^
  -framerate %FPS% ^
  -i "%OUTPUT_DIR%\frames\frame_%%06d.tga" ^
  -i "%OUTPUT_DIR%\audio\forward.wav" ^
  -c:v ffv1 ^
  -level 3 ^
  -g 1 ^
  -c:a pcm_s16le ^
  "%OUTPUT_FILE%"
