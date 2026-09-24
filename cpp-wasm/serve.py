"""Serve the built browser bundle locally. Run from any working directory."""
import argparse
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=8080)
    parser.add_argument("--directory", type=Path,
                        default=Path(__file__).resolve().parents[1] / "build/wasm/web")
    args = parser.parse_args()
    if not (args.directory / "forward.wasm").is_file():
        parser.error("Browser bundle missing. Build cpp-wasm first.")
    SimpleHTTPRequestHandler.extensions_map[".wasm"] = "application/wasm"
    server = ThreadingHTTPServer(("127.0.0.1", args.port),
                                partial(SimpleHTTPRequestHandler, directory=str(args.directory)))
    print(f"Forward: http://127.0.0.1:{args.port}/", flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
