#!/usr/bin/env python3
"""Prepare a standalone Windows release package for the C++ offline exporter."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path
from textwrap import dedent


SCRIPT_DIR = Path(__file__).resolve().parent
CPP_OFFLINE_DIR = SCRIPT_DIR.parent
REPO_ROOT = CPP_OFFLINE_DIR.parent

DEFAULT_BUILD_DIR = CPP_OFFLINE_DIR / "build"
DEFAULT_DIST_DIR = CPP_OFFLINE_DIR / "dist" / "forward-cpp-offline-win64"

SCRIPT_FILES = (
    SCRIPT_DIR / "merge_current_full_outputs.ps1",
    SCRIPT_DIR / "mux_master.bat",
    SCRIPT_DIR / "mux_h264.bat",
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build and package the standalone Forward C++ offline exporter."
    )
    parser.add_argument(
        "--build-dir",
        default=str(DEFAULT_BUILD_DIR),
        help="CMake build directory (default: %(default)s)",
    )
    parser.add_argument(
        "--config",
        default="Release",
        help="CMake build configuration (default: %(default)s)",
    )
    parser.add_argument(
        "--package-dir",
        default=str(DEFAULT_DIST_DIR),
        help="Output package directory (default: %(default)s)",
    )
    parser.add_argument(
        "--package-name",
        default="forward-cpp-offline-win64",
        help="Directory name used in the zip archive (default: %(default)s)",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="Reuse the existing build output instead of running CMake.",
    )
    parser.add_argument(
        "--no-zip",
        action="store_true",
        help="Do not create a zip archive next to the package directory.",
    )
    return parser.parse_args()


def run(command: list[str], cwd: Path) -> None:
    print("+", " ".join(command))
    subprocess.run(command, cwd=str(cwd), check=True)


def configure_and_build(build_dir: Path, config: str) -> None:
    run(["cmake", "-S", "cpp-offline", "-B", str(build_dir)], cwd=REPO_ROOT)
    run(["cmake", "--build", str(build_dir), "--config", config], cwd=REPO_ROOT)


def resolve_exporter_path(build_dir: Path, config: str) -> Path:
    candidates = (
        build_dir / config / "forward-export.exe",
        build_dir / "forward-export.exe",
        build_dir / config / "forward-export",
        build_dir / "forward-export",
    )
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(
        "Unable to locate forward-export in the build directory. "
        "Checked: {}".format(", ".join(str(path) for path in candidates))
    )


def remove_and_recreate_directory(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def copy_release_binaries(exporter_path: Path, package_dir: Path) -> None:
    shutil.copy2(exporter_path, package_dir / exporter_path.name)
    for dll_path in sorted(exporter_path.parent.glob("*.dll")):
        shutil.copy2(dll_path, package_dir / dll_path.name)


def copy_runtime_assets(package_dir: Path) -> None:
    source_root = REPO_ROOT / "original" / "forward"
    if not source_root.is_dir():
        raise FileNotFoundError(f"Original Forward assets were not found: {source_root}")
    shutil.copytree(source_root, package_dir / "original" / "forward")


def copy_support_scripts(package_dir: Path) -> None:
    scripts_dir = package_dir / "scripts"
    scripts_dir.mkdir(parents=True, exist_ok=True)
    for source_path in SCRIPT_FILES:
        shutil.copy2(source_path, scripts_dir / source_path.name)


def write_release_readme(package_dir: Path) -> None:
    readme_text = dedent(
        """\
        Forward C++ offline release package
        ==================================

        This package contains the native C++ offline exporter together with the
        original Forward asset tree under original/forward.

        Quick start
        -----------
        - Double-click render_forward_full.bat
        - Or run: render_forward_full.bat
        - Optional: render_forward_full.bat D:\\some\\other\\output

        Default output
        --------------
        output/
          frames/
          audio/
          manifest.csv
          log.txt

        Optional video mux
        ------------------
        If ffmpeg is available in PATH, the render wrapper also writes:
        - output\\forward_full_current_master.mkv
        - output\\forward_full_current_h264.mp4

        Notes
        -----
        - The exporter expects the original assets at original/forward relative to
          the package root.
        - README.TXT and version.txt are copied from the original release for
          provenance.
        - The packaged exporter is built from the local cpp-offline source tree.
        """
    )
    (package_dir / "RELEASE-README.txt").write_text(readme_text, encoding="ascii")


def write_output_placeholder(package_dir: Path) -> None:
    output_dir = package_dir / "output"
    output_dir.mkdir(parents=True, exist_ok=True)
    placeholder_text = "Rendered frames, audio, manifests, and muxed videos will be written here.\n"
    (output_dir / "README.txt").write_text(placeholder_text, encoding="ascii")


def copy_provenance_notes(package_dir: Path) -> None:
    original_root = REPO_ROOT / "original" / "forward"
    for name in ("README.TXT", "version.txt"):
        source_path = original_root / name
        if source_path.is_file():
            shutil.copy2(source_path, package_dir / name)


def write_render_wrapper(package_dir: Path) -> None:
    wrapper_text = dedent(
        r"""
        @echo off
        setlocal

        set "ROOT_DIR=%~dp0"
        if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
        pushd "%ROOT_DIR%" >NUL

        set "EXPORTER=%ROOT_DIR%\forward-export.exe"
        if not exist "%EXPORTER%" (
            echo forward-export.exe was not found next to this script.
            popd >NUL
            exit /b 1
        )

        if "%~1"=="" (
            if not defined OUTPUT_DIR set "OUTPUT_DIR=%ROOT_DIR%\output"
        ) else (
            set "OUTPUT_DIR=%~f1"
        )

        set "TEMP_DIR=%OUTPUT_DIR%-tmp"
        set "INTRO_OUTPUT=%TEMP_DIR%\intro"
        set "SAARI_OUTPUT=%TEMP_DIR%\saari"
        set "KUKOT_OUTPUT=%TEMP_DIR%\kukot"
        set "MAKU_OUTPUT=%TEMP_DIR%\maku"
        set "WATERCUBE_OUTPUT=%TEMP_DIR%\watercube"
        set "FETA_OUTPUT=%TEMP_DIR%\feta"
        set "UPPOL_OUTPUT=%TEMP_DIR%\uppol"

        if not defined INTRO_END_POSITION set "INTRO_END_POSITION=0x1024"
        if not defined INTRO_POST_ROLL_FRAMES set "INTRO_POST_ROLL_FRAMES=12"
        if not defined SAARI_END_POSITION set "SAARI_END_POSITION=0x0700"
        if not defined SAARI_POST_ROLL_FRAMES set "SAARI_POST_ROLL_FRAMES=0"
        if not defined KUKOT_END_POSITION set "KUKOT_END_POSITION=0x0D00"
        if not defined KUKOT_POST_ROLL_FRAMES set "KUKOT_POST_ROLL_FRAMES=0"
        if not defined MAKU_END_POSITION set "MAKU_END_POSITION=0x1000"
        if not defined MAKU_POST_ROLL_FRAMES set "MAKU_POST_ROLL_FRAMES=0"
        if not defined WATERCUBE_END_POSITION set "WATERCUBE_END_POSITION=0x1300"
        if not defined WATERCUBE_POST_ROLL_FRAMES set "WATERCUBE_POST_ROLL_FRAMES=0"
        if not defined FETA_END_POSITION set "FETA_END_POSITION=0x1600"
        if not defined FETA_POST_ROLL_FRAMES set "FETA_POST_ROLL_FRAMES=0"
        if not defined UPPOL_FRAMES set "UPPOL_FRAMES=1800"

        if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

        echo [1/9] Export intro segment
        "%EXPORTER%" ^
          --sequence intro ^
          --output "%INTRO_OUTPUT%" ^
          --until-song-position %INTRO_END_POSITION% ^
          --post-roll-frames %INTRO_POST_ROLL_FRAMES%
        if errorlevel 1 goto :fail

        echo [2/9] Export saari segment
        "%EXPORTER%" ^
          --sequence saari ^
          --output "%SAARI_OUTPUT%" ^
          --until-song-position %SAARI_END_POSITION% ^
          --post-roll-frames %SAARI_POST_ROLL_FRAMES%
        if errorlevel 1 goto :fail

        echo [3/9] Export kukot segment
        "%EXPORTER%" ^
          --sequence kukot ^
          --output "%KUKOT_OUTPUT%" ^
          --until-song-position %KUKOT_END_POSITION% ^
          --post-roll-frames %KUKOT_POST_ROLL_FRAMES%
        if errorlevel 1 goto :fail

        echo [4/9] Export maku segment
        "%EXPORTER%" ^
          --sequence maku ^
          --output "%MAKU_OUTPUT%" ^
          --until-song-position %MAKU_END_POSITION% ^
          --post-roll-frames %MAKU_POST_ROLL_FRAMES%
        if errorlevel 1 goto :fail

        echo [5/9] Export watercube segment
        "%EXPORTER%" ^
          --sequence watercube ^
          --output "%WATERCUBE_OUTPUT%" ^
          --until-song-position %WATERCUBE_END_POSITION% ^
          --post-roll-frames %WATERCUBE_POST_ROLL_FRAMES%
        if errorlevel 1 goto :fail

        echo [6/9] Export feta segment
        "%EXPORTER%" ^
          --sequence feta ^
          --output "%FETA_OUTPUT%" ^
          --until-song-position %FETA_END_POSITION% ^
          --post-roll-frames %FETA_POST_ROLL_FRAMES%
        if errorlevel 1 goto :fail

        echo [7/9] Export uppol segment
        "%EXPORTER%" ^
          --sequence uppol ^
          --output "%UPPOL_OUTPUT%" ^
          --frames %UPPOL_FRAMES%
        if errorlevel 1 goto :fail

        echo [8/9] Merge current full output
        powershell -NoProfile -ExecutionPolicy Bypass -File ^
          "%ROOT_DIR%\scripts\merge_current_full_outputs.ps1" ^
          -OutputDir "%OUTPUT_DIR%" ^
          -IntroDir "%INTRO_OUTPUT%" ^
          -SaariDir "%SAARI_OUTPUT%" ^
          -KukotDir "%KUKOT_OUTPUT%" ^
          -MakuDir "%MAKU_OUTPUT%" ^
          -WatercubeDir "%WATERCUBE_OUTPUT%" ^
          -FetaDir "%FETA_OUTPUT%" ^
          -UppolDir "%UPPOL_OUTPUT%" ^
          -Fps 50 ^
          -SampleRate 22050
        if errorlevel 1 goto :fail

        if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

        where ffmpeg >NUL 2>NUL
        if errorlevel 1 goto :done

        echo [9/9] Mux master and h264 copies
        call "%ROOT_DIR%\scripts\mux_master.bat" "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_full_current_master.mkv" 50
        if errorlevel 1 goto :fail
        call "%ROOT_DIR%\scripts\mux_h264.bat" "%OUTPUT_DIR%" "%OUTPUT_DIR%\forward_full_current_h264.mp4" 50
        if errorlevel 1 goto :fail

        :done
        echo Render complete: %OUTPUT_DIR%
        popd >NUL
        exit /b 0

        :fail
        echo Render failed.
        popd >NUL
        exit /b 1
        """
    ).lstrip()
    (package_dir / "render_forward_full.bat").write_text(wrapper_text, encoding="ascii")


def create_zip_archive(package_dir: Path, package_name: str) -> Path:
    archive_base = package_dir.parent / package_name
    zip_path = archive_base.with_suffix(".zip")
    if zip_path.exists():
        zip_path.unlink()
    shutil.make_archive(str(archive_base), "zip", root_dir=str(package_dir.parent), base_dir=package_dir.name)
    return zip_path


def main() -> int:
    args = parse_args()

    build_dir = Path(args.build_dir).resolve()
    package_dir = Path(args.package_dir).resolve()

    try:
        if not args.skip_build:
            configure_and_build(build_dir, args.config)

        exporter_path = resolve_exporter_path(build_dir, args.config)

        remove_and_recreate_directory(package_dir)
        copy_release_binaries(exporter_path, package_dir)
        copy_runtime_assets(package_dir)
        copy_support_scripts(package_dir)
        copy_provenance_notes(package_dir)
        write_release_readme(package_dir)
        write_output_placeholder(package_dir)
        write_render_wrapper(package_dir)

        zip_path = None
        if not args.no_zip:
            zip_path = create_zip_archive(package_dir, args.package_name)

    except (FileNotFoundError, subprocess.CalledProcessError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(f"Package directory: {package_dir}")
    print(f"Exporter: {exporter_path}")
    if zip_path is not None:
        print(f"Zip archive: {zip_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
