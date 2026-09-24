"""Exercise the real browser host using Playwright and an installed Chromium browser.

Install playwright in a virtualenv, then run from the repository root:
python cpp-wasm/tests/browser_check.py --browser "C:/.../chrome.exe"
"""
import argparse
import base64
import functools
import json
import threading
import time
import wave
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

from playwright.sync_api import sync_playwright


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--browser", required=True)
    run_mode = parser.add_mutually_exclusive_group()
    run_mode.add_argument("--full", action="store_true", help="Replay every visual tick and save checkpoints")
    run_mode.add_argument("--play-through", action="store_true", help="Play the complete demo at normal audio speed")
    parser.add_argument("--reference-intro", type=Path, help="Offline intro export with at least 151 frames")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    repo = root.parent
    output = repo / "build/validation/browser"
    output.mkdir(parents=True, exist_ok=True)

    class QuietHandler(SimpleHTTPRequestHandler):
        extensions_map = {**SimpleHTTPRequestHandler.extensions_map, ".wasm": "application/wasm"}

        def log_message(self, *_):
            pass

    # Serve from the build directory so subdirectory asset URLs are exercised too.
    server = ThreadingHTTPServer(("127.0.0.1", 0),
                                functools.partial(QuietHandler, directory=str(repo / "build/wasm")))
    threading.Thread(target=server.serve_forever, daemon=True).start()
    errors = []
    try:
        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(executable_path=args.browser, headless=True)
            page = browser.new_page(viewport={"width": 1000, "height": 800})
            page.on("pageerror", lambda error: errors.append(str(error)))
            page.goto(f"http://127.0.0.1:{server.server_port}/web/?test", wait_until="domcontentloaded")
            page.wait_for_function("window.forwardDiagnostics && ['ready','error'].includes(forwardDiagnostics().state)", timeout=120000)
            info = page.evaluate("forwardDiagnostics()")
            assert info["state"] == "ready", info
            assert info["audioState"] == "not created", info
            page.screenshot(path=str(output / "intro.png"))
            print("Prepared:", info, flush=True)
            assert page.evaluate("forwardTest.audio().getChannelData(0).some(x => x !== 0)")
            if args.reference_intro:
                pcm64 = page.evaluate("""() => {
                    const a=forwardTest.audio(), l=a.getChannelData(0), r=a.getChannelData(1);
                    const bytes=new Uint8Array(151*441*4), v=new DataView(bytes.buffer);
                    for(let i=0;i<151*441;++i) { v.setInt16(i*4,l[i]*32768,true); v.setInt16(i*4+2,r[i]*32768,true); }
                    let s=''; for(let i=0;i<bytes.length;i+=8192) s+=String.fromCharCode(...bytes.subarray(i,i+8192));
                    return btoa(s);
                }""")
                with wave.open(str(args.reference_intro / "audio/forward.wav"), "rb") as wav:
                    assert base64.b64decode(pcm64) == wav.readframes(151 * 441), "WASM PCM differs from offline baseline"
                # Run exactly the baseline's first 151 visual ticks, without the playback host.
                pixels = page.evaluate("""() => {
                    const m=forwardTest.module();
                    while(!m._forward_advance(150*441,64,0)) {}
                    return forwardTest.pixels();
                }""")
                baseline = (args.reference_intro / "frames/frame_000150.tga").read_bytes()[18:]
                actual = bytes(component for p in pixels for component in (p & 255, (p >> 8) & 255, (p >> 16) & 255))
                assert actual == baseline, "WASM intro framebuffer differs from offline baseline"
                assert page.evaluate("forwardTest.module()._forward_reset()")
                print("PASS: first 3.02 seconds of PCM and 3-second framebuffer exactly match cpp-offline", flush=True)
            page.locator("#start").click()
            page.wait_for_function("forwardDiagnostics().sample > 22050", timeout=15000)
            assert page.evaluate("forwardDiagnostics().audioState") == "running"
            page.locator("#pause").click()
            page.wait_for_function("forwardDiagnostics().state === 'paused'")
            held = page.evaluate("forwardDiagnostics().sample")
            page.wait_for_timeout(200)
            assert page.evaluate("forwardDiagnostics().sample") == held
            page.locator("#mute").click()
            assert page.locator("#mute").get_attribute("aria-pressed") == "true"
            page.locator("#pause").click()
            page.wait_for_function("forwardDiagnostics().sample > " + str(held + 2205))
            page.locator("#restart").click()
            page.wait_for_function("forwardDiagnostics().state === 'playing' && forwardDiagnostics().sample < 22050")
            page.wait_for_timeout(200)
            # Visibility lifecycle via the same event handler, without test-only timing code.
            page.evaluate("Object.defineProperty(document, 'hidden', {configurable:true, get:()=>true}); document.dispatchEvent(new Event('visibilitychange'))")
            page.wait_for_function("forwardDiagnostics().state === 'paused'")
            page.evaluate("delete document.hidden")
            page.locator("#pause").click()
            page.wait_for_function("forwardDiagnostics().state === 'playing'")
            page.wait_for_timeout(150)
            page.locator("#pause").click()
            page.wait_for_function("forwardDiagnostics().state === 'paused'")
            page.screenshot(path=str(output / "playing.png"))
            print("Canvas:", page.evaluate("({width:canvas.width,height:canvas.height,css:canvas.getBoundingClientRect().toJSON()})"), flush=True)
            assert page.locator("#canvas").get_attribute("width") == "512"
            assert page.locator("#canvas").get_attribute("height") == "256"
            page.set_viewport_size({"width": 380, "height": 700})
            assert page.locator("#canvas").bounding_box()["width"] <= 380
            assert page.locator("#canvas").get_attribute("width") == "512"
            page.set_viewport_size({"width": 1000, "height": 800})
            page.locator("#fullscreen").click()
            page.wait_for_function("!!document.fullscreenElement")
            assert page.locator("#canvas").get_attribute("width") == "512"
            assert page.locator("#canvas").get_attribute("height") == "256"
            page.evaluate("document.exitFullscreen()")
            if args.play_through:
                if page.locator("#mute").get_attribute("aria-pressed") == "true":
                    page.locator("#mute").click()
                page.locator("#restart").click()
                deadline = time.monotonic() + info["totalSamples"] / 22050 + 45
                while time.monotonic() < deadline:
                    page.wait_for_timeout(15000)
                    playback = page.evaluate("forwardDiagnostics()")
                    print("Playback:", playback, flush=True)
                    assert playback["state"] in ("playing", "ended"), playback
                    if playback["state"] == "ended":
                        break
                else:
                    raise AssertionError("Full playback did not finish")
                assert playback["overloads"] == 0, playback
            if args.full:
                # The normal host is paused while the deterministic runtime advances.
                result = page.evaluate("""async () => {
                    const m = forwardTest.module();
                    if (!m._forward_reset()) throw Error(m.UTF8ToString(m._forward_error()));
                    const total = m._forward_total_samples();
                    const checkpoints = [];
                    // The paused UI asks for an earlier sample, which cannot advance this forward-only runtime.
                    for (let sample = 0; sample < total; sample += 441 * 50) {
                        let done = 0;
                        while (!done) done = m._forward_advance(Math.min(total - 1, sample), 64, 0);
                        const scene = m.UTF8ToString(m._forward_scene());
                        if (!checkpoints.length || checkpoints.at(-1).scene !== scene) {
                            const p = m._forward_pixels() >>> 2;
                            const pixels = m.HEAPU32.subarray(p, p + 512 * 256);
                            checkpoints.push({sample:m._forward_rendered_sample(), scene,
                                nonzero:pixels.reduce((n,x)=>n+(x!==0),0)});
                        }
                        await new Promise(resolve => setTimeout(resolve, 0));
                    }
                    while (!m._forward_advance(total - 1, 64, 1)) await new Promise(resolve => setTimeout(resolve,0));
                    return {checkpoints, rendered:m._forward_rendered_sample(), total};
                }""")
                assert [x["scene"] for x in result["checkpoints"]] == ["mute95", "domina", "saari", "kukot", "maku", "watercube", "feta", "uppol"], result
                (output / "full-runtime.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
                page.screenshot(path=str(output / "credits.png"))
                print("Full WASM runtime:", result, flush=True)
            assert not errors, errors
            info = page.evaluate("forwardDiagnostics()")
            assert not info["failure"], info
            report = json.dumps({"browser": browser.version, "result": info, "errors": errors}, indent=2)
            mode = "play-through" if args.play_through else "accelerated" if args.full else "controls"
            (output / f"browser-check-{mode}.json").write_text(report, encoding="utf-8")
            (output / "browser-check.json").write_text(report, encoding="utf-8")
            print("PASS: browser load, gesture start, pause/resume, mute, restart, visibility, fixed render dimensions", flush=True)
            browser.close()
    finally:
        server.shutdown()


if __name__ == "__main__":
    main()
