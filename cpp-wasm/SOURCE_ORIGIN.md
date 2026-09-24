# Source origin

The initial engine in this directory was copied from `cpp-offline/` at repository
revision `148fb742` (engine unchanged from the study's `0e60e500` baseline). The new
project is independent: it neither builds nor includes sources from `cpp-offline/`.
The offline project remains the reference implementation.

Copied components: original asset loaders, software surfaces and rasterizers,
all eight scene/routine implementations, Java-compatible random generator,
XM mixer, script tables, small file utilities, and vendored `stb_image` (whose
license is included at the end of its header).

Real-time orchestration, hosts, build files, and browser shell are authored here.
Subsequent engine changes belong to this copy. Original runtime artwork and music
remain in `../original/forward/` and are staged unchanged for browser builds.

The initial player is restricted to 512x256, 50 visual ticks/s and a 22050 Hz
source audio clock. Display scaling never changes the render resolution.
