"use strict";

(() => {
  const rate = 22050;
  const ui = Object.fromEntries(["start", "status", "progress", "player", "canvas", "pause",
    "restart", "mute", "fullscreen", "position", "viewport"].map(id => [id, document.getElementById(id)]));
  let module, buffer, context, gain, source;
  let state = "loading", total = 0, heldSample = 0, lastSample = 0, startTime = 0;
  let muted = false, busy = false, pendingVisibilityPause = false;
  let clockKind = "stopped", overloads = 0, failure = "", worstFrameMs = 0;
  const yieldToBrowser = () => new Promise(resolve => setTimeout(resolve, 0));
  const engineError = () => module.UTF8ToString(module._forward_error()) || "The demo engine could not continue.";

  function setState(next, message) {
    state = next;
    ui.start.disabled = next !== "ready";
    ui.pause.disabled = !["playing", "paused"].includes(next);
    ui.pause.textContent = next === "paused" ? "Resume" : "Pause";
    ui.restart.disabled = !["playing", "paused", "ended"].includes(next);
    if (message) ui.status.textContent = message;
  }

  function fail(error) {
    failure = error instanceof Error ? error.message : String(error);
    if (context) context.suspend().catch(() => {});
    setState("error", failure);
    ui.progress.hidden = true;
    console.error(error);
  }

  function graphSample() {
    return Math.max(0, Math.min(total, Math.floor((context.currentTime - startTime) * rate)));
  }

  function audibleSample() {
    let time;
    if (typeof context.getOutputTimestamp === "function") {
      const stamp = context.getOutputTimestamp();
      if (stamp.performanceTime > 0 && Number.isFinite(stamp.contextTime) &&
          Math.abs(performance.now() - stamp.performanceTime) < 1000) {
        time = Math.min(context.currentTime, stamp.contextTime + (performance.now() - stamp.performanceTime) / 1000);
        clockKind = "output timestamp";
      }
    }
    if (time === undefined) {
      time = context.currentTime - (context.baseLatency || 0) - (context.outputLatency || 0);
      clockKind = "estimated output latency";
    }
    lastSample = Math.max(lastSample, Math.min(total, Math.max(0, Math.floor((time - startTime) * rate))));
    return lastSample;
  }

  async function pause(message = "Paused. Resume when ready.") {
    if (state !== "playing") return;
    setState("pausing", "Pausing…");
    await context.suspend();
    heldSample = graphSample();
    lastSample = heldSample;
    // Drain visual work to the actual graph suspension point, including in a hidden tab.
    let caughtUp = false;
    while (!caughtUp) {
      const result = module._forward_advance(heldSample, 4, document.hidden ? 0 : 1);
      if (result < 0) throw new Error(engineError());
      caughtUp = result === 1;
      if (!caughtUp) await yieldToBrowser();
    }
    setState("paused", message);
  }

  async function resume() {
    setState("starting", "Resuming…");
    await context.resume();
    if (context.state !== "running") throw new Error("Audio is suspended. Click Resume to allow playback.");
    setState("playing", "Playing at 512 × 256.");
    if (document.hidden || pendingVisibilityPause) {
      pendingVisibilityPause = false;
      await pause("Paused while the page is hidden.");
    }
  }

  async function start(restart = false) {
    setState("starting", "Starting…");
    ui.player.hidden = false;
    // Call resume directly from the button event, before yielding for reset work.
    if (!context) {
      context = new AudioContext();
      gain = context.createGain();
      gain.gain.value = muted ? 0 : 1;
      gain.connect(context.destination);
      context.addEventListener("statechange", () => {
        if (state === "playing" && context.state !== "running") {
          heldSample = graphSample();
          lastSample = heldSample;
          setState("paused", "Audio was interrupted. Click Resume to continue.");
        } else if (state === "paused" && context.state === "running") {
          // Some operating systems resume an interrupted context automatically.
          // Keep both transports paused until the explicit Resume gesture.
          context.suspend().catch(fail);
        }
      });
    }
    const resumed = context.resume();
    if (source) {
      source.onended = null;
      source.stop();
      source.disconnect();
    }
    await resumed;
    if (restart && !module._forward_reset()) throw new Error(engineError());
    if (context.state !== "running") throw new Error("The browser did not enable audio playback.");
    heldSample = lastSample = 0;
    module._forward_advance(0, 1, 1);
    source = context.createBufferSource();
    source.buffer = buffer;
    source.connect(gain);
    source.onended = () => {
      heldSample = total;
      setState("ended", "End of the demo. Replay with Restart.");
    };
    startTime = context.currentTime + 0.08;
    source.start(startTime);
    setState("playing", "Playing at 512 × 256.");
    if (document.hidden || pendingVisibilityPause) {
      pendingVisibilityPause = false;
      await pause("Paused while the page is hidden.");
    }
  }

  async function action(callback) {
    if (busy) return;
    busy = true;
    try { await callback(); } catch (error) { fail(error); } finally { busy = false; }
  }
  ui.start.addEventListener("click", () => action(() => start()));
  ui.restart.addEventListener("click", () => action(() => start(true)));
  ui.pause.addEventListener("click", () => action(() => state === "paused" ? resume() : pause()));
  ui.mute.addEventListener("click", () => {
    muted = !muted;
    if (gain) gain.gain.value = muted ? 0 : 1;
    ui.mute.textContent = muted ? "Unmute" : "Mute";
    ui.mute.setAttribute("aria-pressed", String(muted));
  });
  ui.fullscreen.addEventListener("click", () => {
    if (!ui.viewport.requestFullscreen || !document.exitFullscreen) {
      ui.status.textContent = "Fullscreen is unavailable in this browser.";
      return;
    }
    const change = document.fullscreenElement ? document.exitFullscreen() : ui.viewport.requestFullscreen();
    if (change) change.catch(() => { ui.status.textContent = "Fullscreen is unavailable in this browser."; });
  });
  document.addEventListener("visibilitychange", () => {
    if (!document.hidden) return;
    if (state === "starting") pendingVisibilityPause = true;
    else if (state === "playing") action(() => pause("Paused while the page is hidden."));
  });
  ui.canvas.addEventListener("webglcontextlost", event => {
    event.preventDefault();
    fail(new Error("The graphics context was lost. Reload the page to restart the demo."));
  });

  function frame() {
    if (module && ["playing", "paused", "ended"].includes(state)) {
      const begin = performance.now();
      const sample = state === "playing" ? audibleSample() : heldSample;
      const result = module._forward_advance(sample, 4, 1);
      worstFrameMs = Math.max(worstFrameMs, performance.now() - begin);
      if (result < 0) fail(new Error(engineError()));
      else if (state === "playing" && result === 0 && sample - module._forward_rendered_sample() > rate / 5) {
        ++overloads;
        action(() => pause("Playback paused because rendering fell behind. Resume to continue."));
      }
      ui.position.textContent = `${Math.floor(sample / rate)} / ${Math.ceil(total / rate)} s`;
    }
    requestAnimationFrame(frame);
  }

  async function load() {
    if (typeof WebAssembly !== "object" || typeof AudioContext !== "function")
      throw new Error("This edition needs WebAssembly and Web Audio support.");
    module = await createForward({
      canvas: ui.canvas,
      locateFile: name => new URL(name, document.baseURI).href,
      printErr: message => console.warn(message),
      onAbort: reason => fail(new Error(`Demo initialization failed: ${reason}`))
    });
    if (module.UTF8ToString(module._forward_error())) throw new Error(engineError());
    total = module._forward_total_samples();
    if (!(total > 0)) throw new Error("The music timeline is empty.");
    buffer = new AudioBuffer({numberOfChannels: 2, length: total, sampleRate: rate});
    const left = buffer.getChannelData(0), right = buffer.getChannelData(1);
    for (let i = 0; i < 8; ++i) {
      ui.status.textContent = `Preparing scene ${i + 1} of 8…`;
      await yieldToBrowser();
      if (!module._forward_scene_init(i)) throw new Error(engineError());
    }
    while (module._forward_prepared_samples() < total) {
      const begin = performance.now();
      do {
        const count = module._forward_prepare();
        if (count < 0) throw new Error(engineError());
        const offset = module._forward_block_start();
        const ptr = module._forward_audio_block() >>> 1;
        const samples = module.HEAP16; // Reacquire after C++ allocations / heap growth.
        for (let i = 0; i < count; ++i) {
          left[offset + i] = samples[ptr + i * 2] / 32768;
          right[offset + i] = samples[ptr + i * 2 + 1] / 32768;
        }
      } while (module._forward_prepared_samples() < total && performance.now() - begin < 12);
      ui.progress.value = module._forward_prepared_samples() / total;
      ui.status.textContent = `Preparing music… ${Math.round(ui.progress.value * 100)}%`;
      await yieldToBrowser();
    }
    ui.progress.hidden = true;
    setState("ready", "Ready. Click Start to play with sound.");
    requestAnimationFrame(frame);
  }

  // Read-only diagnostics for reproducible browser checks; no timing controls in the public UI.
  window.forwardDiagnostics = () => ({state, totalSamples: total, sample: state === "playing" ? lastSample : heldSample,
    renderedSample: module ? module._forward_rendered_sample() : 0,
    scene: module ? module.UTF8ToString(module._forward_scene()) : "", clockKind, overloads, worstFrameMs, failure,
    renderWidth: 512, renderHeight: 256, audioState: context ? context.state : "not created",
    audioRate: context ? context.sampleRate : null});
  // Deterministic capture access is available only when explicitly requested by a test URL.
  if (new URLSearchParams(location.search).has("test")) {
    window.forwardTest = {
      module: () => module,
      audio: () => buffer,
      pause: () => action(() => pause()),
      pixels: () => Array.from(module.HEAPU32.subarray(module._forward_pixels() >>> 2,
        (module._forward_pixels() >>> 2) + 512 * 256))
    };
  }
  load().catch(fail);
})();
