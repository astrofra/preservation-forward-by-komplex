# Forward JPackage Workflow

## Goal

Produce a Windows 64-bit desktop package for the Java reconstruction without depending on a local JDK installed on the target machine.

## Current Packaging Path

The repository now ships a packaging wrapper:

- `package_forward_desktop.bat`

This script:

1. recompiles `java-desktop/src/main/java`
2. builds `forward-desktop.jar`
3. stages the original runtime asset directories from `original/forward`:
   `asses`, `images`, `meshes`, `mods`
4. runs `jpackage` to create a self-contained Windows app image
5. optionally builds a Windows installer `exe`

## Why a Dedicated Launcher Exists

The desktop reconstruction still loads assets with the original applet-era URL logic:

- `getDocumentBase()`
- relative paths such as `images/...`, `mods/...`, `meshes/...`, `asses/...`

The development launcher keeps that behavior by starting from `original/forward`.

For a packaged build, relying on the process working directory would be fragile. The packaging path therefore uses a dedicated Java launcher:

- `java-desktop/src/main/java/ForwardDesktopLauncher.java`

That launcher:

- locates the bundled asset root at runtime
- injects `basedir <absolute-path>` before calling `forward.main(...)`
- sets `forward.repoRoot` when possible so relative capture output still works

This keeps the source-faithful asset loading model intact while removing the dependency on a developer checkout layout.

## Usage

From the repository root:

```bat
package_forward_desktop.bat
```

Default output:

- `java-desktop/dist/jpackage/app-image/Forward/Forward.exe`

This `app-image` already contains the Java runtime. It can be copied to another Windows machine without installing a separate JDK.

To build an installer executable as well:

```bat
package_forward_desktop.bat exe
```

Or build both targets:

```bat
package_forward_desktop.bat all
```

## WiX Requirement

`jpackage --type app-image` works with the JDK alone.

`jpackage --type exe` needs WiX Toolset available in `PATH`.

If WiX is missing:

- the script still builds the portable `app-image`
- installer generation is skipped with an explicit message

## Validation Strategy

The recommended smoke test after packaging is:

1. launch the packaged `Forward.exe`
2. confirm the window opens and assets load
3. run a short capture session with `capture ... capturelimit ... captureexit 1`
4. compare the captured PNGs with the normal desktop build if needed

## Known Constraints

- this workflow is currently Windows-oriented
- the package is unsigned
- the asset bundle still mirrors `original/forward` instead of using a cleaned resource layout
- installer generation depends on WiX availability
