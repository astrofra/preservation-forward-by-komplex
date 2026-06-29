@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "APP_NAME=forward-komplex"
set "APP_VERSION=1.0.2"

set "JAVA_PROJECT=%ROOT%\java-desktop"
set "SRC_DIR=%JAVA_PROJECT%\src\main\java"
set "BUILD_ROOT=%JAVA_PROJECT%\build"
set "BUILD_DIR=%BUILD_ROOT%\classes"
set "SOURCES_FILE=%BUILD_ROOT%\sources.txt"

set "DIST_ROOT=%JAVA_PROJECT%\dist\jpackage"
set "INPUT_DIR=%DIST_ROOT%\input"
set "APP_IMAGE_OUT=%DIST_ROOT%\app-image"
set "INSTALLER_OUT=%DIST_ROOT%\installer"
set "TEMP_ROOT=%DIST_ROOT%\temp"
set "JAR_FILE=%INPUT_DIR%\forward-desktop.jar"
set "ASSETS_DIR=%INPUT_DIR%\forward-assets"
set "ICON_PNG=%JAVA_PROJECT%\app-icon.png"
set "ICON_ICO=%INPUT_DIR%\%APP_NAME%.ico"
set "LAUNCHER_INI_SOURCE=%ROOT%\forward-launcher.ini"
set "FORWARD_ROOT=%ROOT%\original\forward"
set "FORWARD_README_SOURCE=%FORWARD_ROOT%\README.TXT"
set "FORWARD_VERSION_SOURCE=%FORWARD_ROOT%\version.txt"
set "APP_IMAGE_DIR=%APP_IMAGE_OUT%\%APP_NAME%"
set "PACKAGE_MODE=%~1"
if "%PACKAGE_MODE%"=="" set "PACKAGE_MODE=app-image"

if /I not "%PACKAGE_MODE%"=="app-image" if /I not "%PACKAGE_MODE%"=="exe" if /I not "%PACKAGE_MODE%"=="all" (
    echo Usage: %~nx0 [app-image^|exe^|all]
    exit /b 1
)

where javac >nul 2>nul || (
    echo javac was not found in PATH.
    exit /b 1
)
where jar >nul 2>nul || (
    echo jar was not found in PATH.
    exit /b 1
)
where jpackage >nul 2>nul || (
    echo jpackage was not found in PATH.
    exit /b 1
)

if not exist "%BUILD_ROOT%" mkdir "%BUILD_ROOT%"
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Get-ChildItem -Recurse -Path '%SRC_DIR%' -Filter *.java | ForEach-Object { $_.FullName } | Set-Content -Encoding ASCII '%SOURCES_FILE%'"
if errorlevel 1 exit /b %errorlevel%

javac -d "%BUILD_DIR%" @"%SOURCES_FILE%"
if errorlevel 1 exit /b %errorlevel%

if exist "%DIST_ROOT%" rmdir /s /q "%DIST_ROOT%"
mkdir "%INPUT_DIR%"
mkdir "%APP_IMAGE_OUT%"
mkdir "%TEMP_ROOT%"

jar --create --file "%JAR_FILE%" --main-class ForwardDesktopLauncher -C "%BUILD_DIR%" .
if errorlevel 1 exit /b %errorlevel%

call :build_icon
if errorlevel 1 exit /b %errorlevel%

for %%D in (asses images meshes mods) do (
    robocopy "%ROOT%\original\forward\%%D" "%ASSETS_DIR%\%%D" /E /NFL /NDL /NJH /NJS /NC /NS >nul
    if errorlevel 8 exit /b !errorlevel!
)

call :build_app_image
if errorlevel 1 exit /b %errorlevel%

call :stage_release_notes
if errorlevel 1 exit /b %errorlevel%

call :stage_launcher_ini
if errorlevel 1 exit /b %errorlevel%

if /I "%PACKAGE_MODE%"=="app-image" goto :success

call :check_wix
if errorlevel 1 (
    if /I "%PACKAGE_MODE%"=="all" goto :success
    exit /b 1
)

call :build_installer
if errorlevel 1 exit /b %errorlevel%

:success
echo.
echo App image:
echo   %APP_IMAGE_OUT%\%APP_NAME%
if exist "%APP_IMAGE_OUT%\%APP_NAME%\%APP_NAME%.exe" (
    echo Launcher:
    echo   %APP_IMAGE_OUT%\%APP_NAME%\%APP_NAME%.exe
)
if exist "%INSTALLER_OUT%" (
    dir /b "%INSTALLER_OUT%"
)
exit /b 0

:build_app_image
jpackage ^
  --type app-image ^
  --name "%APP_NAME%" ^
  --app-version "%APP_VERSION%" ^
  --vendor "Komplex.org" ^
  --copyright "Komplex, desktop build" ^
  --description "Forward demo by Komplex, reactivated Java desktop version." ^
  --icon "%ICON_ICO%" ^
  --input "%INPUT_DIR%" ^
  --main-jar "forward-desktop.jar" ^
  --main-class "ForwardDesktopLauncher" ^
  --dest "%APP_IMAGE_OUT%" ^
  --temp "%TEMP_ROOT%\app-image" ^
  --java-options "-Dfile.encoding=UTF-8"
exit /b %errorlevel%

:build_installer
if not exist "%INSTALLER_OUT%" mkdir "%INSTALLER_OUT%"
jpackage ^
  --type exe ^
  --name "%APP_NAME%" ^
  --app-version "%APP_VERSION%" ^
  --vendor "Komplex.org" ^
  --copyright "Komplex, desktop build" ^
  --description "Forward demo by Komplex, reactivated Java desktop version." ^
  --icon "%ICON_ICO%" ^
  --dest "%INSTALLER_OUT%" ^
  --temp "%TEMP_ROOT%\installer" ^
  --app-image "%APP_IMAGE_OUT%\%APP_NAME%" ^
  --win-dir-chooser ^
  --win-menu ^
  --win-shortcut ^
  --win-menu-group "Forward by Komplex"
exit /b %errorlevel%

:check_wix
where candle >nul 2>nul && where light >nul 2>nul && exit /b 0
where wix >nul 2>nul && exit /b 0
echo WiX Toolset was not found in PATH. App image was built, but installer generation was skipped.
echo Install WiX and re-run %~nx0 exe if you want a Windows installer package.
exit /b 1

:build_icon
if not exist "%ICON_PNG%" (
    echo Icon source was not found: %ICON_PNG%
    exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Add-Type -AssemblyName System.Drawing; $srcPath='%ICON_PNG%'; $dstPath='%ICON_ICO%'; $size=256; $src=[System.Drawing.Image]::FromFile($srcPath); try { $bmp=New-Object System.Drawing.Bitmap($size, $size, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb); try { $g=[System.Drawing.Graphics]::FromImage($bmp); try { $g.Clear([System.Drawing.Color]::Transparent); $g.InterpolationMode=[System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic; $g.PixelOffsetMode=[System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality; $g.SmoothingMode=[System.Drawing.Drawing2D.SmoothingMode]::HighQuality; $scale=[Math]::Min($size / [double]$src.Width, $size / [double]$src.Height); $w=[int][Math]::Round($src.Width * $scale); $h=[int][Math]::Round($src.Height * $scale); $x=[int](($size - $w) / 2); $y=[int](($size - $h) / 2); $g.DrawImage($src, $x, $y, $w, $h); $ms=New-Object System.IO.MemoryStream; try { $bmp.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png); $pngBytes=$ms.ToArray(); $fs=[System.IO.File]::Open($dstPath, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None); try { $bw=New-Object System.IO.BinaryWriter($fs); try { $bw.Write([UInt16]0); $bw.Write([UInt16]1); $bw.Write([UInt16]1); $bw.Write([byte]0); $bw.Write([byte]0); $bw.Write([byte]0); $bw.Write([byte]0); $bw.Write([UInt16]1); $bw.Write([UInt16]32); $bw.Write([UInt32]$pngBytes.Length); $bw.Write([UInt32]22); $bw.Write($pngBytes); } finally { $bw.Dispose() } } finally { $fs.Dispose() } } finally { $ms.Dispose() } } finally { $g.Dispose() } } finally { $bmp.Dispose() } } finally { $src.Dispose() }"
exit /b %errorlevel%

:stage_launcher_ini
if not exist "%LAUNCHER_INI_SOURCE%" exit /b 0
if not exist "%APP_IMAGE_DIR%" exit /b 0
copy /y "%LAUNCHER_INI_SOURCE%" "%APP_IMAGE_DIR%\forward-launcher.ini" >nul
exit /b %errorlevel%

:stage_release_notes
if not exist "%APP_IMAGE_DIR%" exit /b 0
if exist "%FORWARD_README_SOURCE%" (
    copy /y "%FORWARD_README_SOURCE%" "%APP_IMAGE_DIR%\README.TXT" >nul
    if errorlevel 1 exit /b %errorlevel%
)
if exist "%FORWARD_VERSION_SOURCE%" (
    copy /y "%FORWARD_VERSION_SOURCE%" "%APP_IMAGE_DIR%\version.txt" >nul
    if errorlevel 1 exit /b %errorlevel%
)
exit /b 0
