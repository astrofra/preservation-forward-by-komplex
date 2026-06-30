@echo off
setlocal

set "OUTPUT_DIR=%~1"
if "%OUTPUT_DIR%"=="" set "OUTPUT_DIR=output"

set "OUTPUT_FILE=%~2"
if "%OUTPUT_FILE%"=="" set "OUTPUT_FILE=%OUTPUT_DIR%\forward_h264.mp4"

set "FPS=%~3"
if "%FPS%"=="" set "FPS=50"

ffmpeg -y ^
  -framerate %FPS% ^
  -i "%OUTPUT_DIR%\frames\frame_%%06d.tga" ^
  -i "%OUTPUT_DIR%\audio\forward.wav" ^
  -c:v libx264 ^
  -preset slow ^
  -crf 12 ^
  -pix_fmt yuv420p ^
  -c:a aac ^
  -b:a 192k ^
  "%OUTPUT_FILE%"
