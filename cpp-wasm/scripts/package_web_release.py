"""Package an already-built browser release using only the Python standard library."""
from pathlib import Path
import shutil
import sys
import tempfile
from zipfile import ZIP_DEFLATED, ZipFile


WEB_FILES = (
    "index.html",
    "player.css",
    "player.js",
    "forward.js",
    "forward.wasm",
    "forward.data",
    "art/aijja.gif",
    "art/back.gif",
)


def main():
    repo = Path(__file__).resolve().parents[2]
    source = repo / "build/wasm/web"
    dist = repo / "cpp-wasm/dist"
    package = dist / "forward-web"
    archive = dist / "forward-web.zip"

    # These generated paths are fixed. Never clean a redirected directory.
    if dist.resolve() != dist or package.resolve() != package:
        raise ValueError("Distribution paths must not be symbolic links or junctions.")
    for name in WEB_FILES:
        asset = source / name
        if not asset.is_file() or asset.stat().st_size == 0:
            raise ValueError(f"Missing or empty browser asset: {asset}. Run cpp-wasm/build_web.bat first.")

    dist.mkdir(parents=True, exist_ok=True)
    # Finish copying and archiving before replacing the previous distribution.
    with tempfile.TemporaryDirectory(prefix=".forward-web-", dir=dist) as temporary:
        staging = Path(temporary)
        staged_package = staging / package.name
        for name in WEB_FILES:
            target = staged_package / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source / name, target)

        staged_archive = staging / archive.name
        with ZipFile(staged_archive, "w", ZIP_DEFLATED, compresslevel=9) as bundle:
            for name in WEB_FILES:
                # No enclosing directory: extract straight into the website folder.
                bundle.write(staged_package / name, arcname=name)

        if package.exists():
            shutil.rmtree(package)
        staged_package.replace(package)
        staged_archive.replace(archive)

    size = sum((package / name).stat().st_size for name in WEB_FILES)
    print(f"Website: {package} ({len(WEB_FILES)} files, {size / 1024 / 1024:.2f} MiB)")
    print(f"Archive: {archive} ({archive.stat().st_size / 1024 / 1024:.2f} MiB)")
    print("Upload the contents of forward-web/ into your website folder, keeping art/ intact.")
    print("Or upload and extract forward-web.zip there. Open index.html over HTTPS.")
    print("The web server should serve .wasm as application/wasm. No server-side code is required.")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError) as error:
        print(f"Packaging failed: {error}", file=sys.stderr)
        sys.exit(1)
