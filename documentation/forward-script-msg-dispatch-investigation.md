# Forward `msg` Dispatch Investigation

## Scope

This note answers a specific reverse-engineering question about `reverse/cfr_single/forward.java:49`: are the scene nicknames and event nicknames used by `msg` resolved to internal functions by name, or is something simpler happening?

## Short Answer

`msg` is not implemented as a reflective string-to-method lookup.

What the code actually does is:

1. Parse `msg <target> <payload...>` from the script array.
2. Look up `<target>` in a registry of scene objects and routine objects.
3. Pass the remaining text `<payload...>` as a plain string to that object's message handler, together with a scene-relative time value.

So names such as `mute95`, `saari`, `domina`, `watercube`, and `feta` are registry keys, not Java method names. Names such as `saviour`, `fade2black`, `suh0`, `rok`, `pum`, or `blackfeta` are scene-local message payloads, not globally dispatched functions.

## Direct Evidence In The Reconstructed Tree

The reconstructed desktop tree makes the dispatch path very clear:

- `java-desktop/src/main/java/ForwardDemoApp.java:52` contains the script array as `scriptCommands`.
- `ForwardDemoApp.executeScriptCommand(...)` at `ForwardDemoApp.java:308-403` tokenizes each script line.
- The `msg` branch at `ForwardDemoApp.java:320-327` reads:
  - token 1: `msg`
  - token 2: target name
  - remaining tokens: payload string
- It then calls `ForwardDemoApp.Kamajak(target, payload)` at `ForwardDemoApp.java:327`.
- `ForwardDemoApp.Kamajak(...)` at `ForwardDemoApp.java:424-435` first checks `sceneRegistry`, then `routineRegistry`, and finally calls:
  - `Scene.handleMessage(payload, this.sceneTimeSeconds - this.kkaMAJA)`, or
  - `GraphicsRoutine.handleMessage(payload, this.sceneTimeSeconds - this.kkaMAJA)`
- The registries are populated by `ForwardDemoApp.kAMAjak(...)` and `ForwardDemoApp.KAmajak(...)` at `ForwardDemoApp.java:494-510`, using each object's `scriptName()` return value as the lookup key.

That means the dispatch model is:

`script text -> target name lookup -> object.handleMessage(payload, localTime)`

Not:

`script text -> function name lookup -> invoke matching method`

## Direct Evidence In The Raw CFR Tree

The same structure is already visible in the raw decompiled tree:

- `reverse/cfr_single/forward.java:49` contains the original `kkAmajA` script array.
- `forward.KaMajak(...)` at `reverse/cfr_single/forward.java:288-380` parses each script command.
- Its `msg` branch is at `reverse/cfr_single/forward.java:300-307`.
- `forward.Kamajak(target, payload)` at `reverse/cfr_single/forward.java:401-412` does the same registry lookup as the reconstructed tree.
- For scene-like objects it calls `mmjjmma.MAJakkA(payload, localTime)`.
- For routine-like objects it calls `majjkka.AMaJaKK(payload, localTime)`.
- The registries are filled at `reverse/cfr_single/forward.java:464-479` with:
  - `this.kKAmajA.put(mmjjmma2.majakkA(), mmjjmma2);`
  - `this.KkamajA.put(majjkka2.amAjAkk(), majjkka2);`

The raw abstract bases make the same contract explicit:

- `reverse/cfr_single/mmjjmma.java`
  - `majakkA()` = script-facing scene name
  - `MAJakkA(String, float)` = scene message handler
- `reverse/cfr_single/majjkka.java`
  - `amAjAkk()` = script-facing routine name
  - `AMaJaKK(String, float)` = routine message handler

So the reconstructed names are not changing the behavior here. They only make the already-existing dispatch model easier to read.

## Target Name Mapping

The targets used in the script correspond to concrete registered objects:

| Script target | Reconstructed class | Raw CFR class | Kind |
| --- | --- | --- | --- |
| `mute95` | `Mute95Scene` | `kmjjkmk` | scene |
| `domina` | `DominaRoutine` | `kajakka` | routine |
| `saari` | `SaariScene` | `maajmka` | scene |
| `kukot` | `KukotScene` | `kajjkka` | scene |
| `maku` | `MakuScene` | `kmjjmka` | scene |
| `watercube` | `WatercubeScene` | `kmajmka` | scene |
| `feta` | `FetaScene` | `kmaamka` | scene |
| `uppol` | `UppolRoutine` | `mmaakmk` | routine |

This mapping is directly visible in:

- `ForwardDemoApp.java:503-510`
- `reverse/cfr_single/forward.java:472-479`

## Message Vocabulary By Target

Each target interprets its own payload string inside its own `handleMessage(...)` implementation.

### `mute95`

Source:

- `java-desktop/src/main/java/Mute95Scene.java:160-183`
- `reverse/cfr_single/kmjjkmk.java:149-172`

Accepted payloads:

- `saviour`
- `jmagic`
- `jugi`
- `anis`
- `carebear`

Observed effect:

These do not call separate named functions. They select which pair of loaded portrait images should be crossfaded and reset the message start time.

### `domina`

Source:

- `java-desktop/src/main/java/DominaRoutine.java:78-82`
- `reverse/cfr_single/kajakka.java:78-82`

Accepted payloads:

- `fade2black`

Observed effect:

Starts the routine's fade-to-black phase by storing the incoming local time.

### `saari`

Source:

- `java-desktop/src/main/java/SaariScene.java:95-103`
- `reverse/cfr_single/maajmka.java:95-103`

Accepted payloads:

- `suh`
- `suh0`

Observed effect:

Loads preset values for a scene-local overlay/intensity countdown. The names themselves are opaque nicknames; the behavior is just two hard-coded presets.

### `kukot`

Source:

- `java-desktop/src/main/java/KukotScene.java:102-117`
- `reverse/cfr_single/kajjkka.java:102-117`

Accepted payloads:

- `suh`
- `suh0`
- `suh1`
- `suh2`

Observed effect:

Selects one of several hard-coded flash/noise overlay presets by assigning intensity and decay values.

### `maku`

Source:

- `java-desktop/src/main/java/MakuScene.java:90-117`
- `reverse/cfr_single/kmjjmka.java:90-117`

Accepted payloads:

- `suh`
- `suh0`
- `ksor`
- `low`
- `roll`
- `go <float>`
- `speed <float>`

Observed effect:

- `suh` / `suh0` pick overlay presets.
- `ksor`, `low`, and `roll` toggle scene booleans.
- `go <float>` stores a new track position and also stores the message time as a reference point.
- `speed <float>` changes the playback speed scalar.

This is a good example of why `msg` is just text forwarding: `ForwardDemoApp` does not know anything about `go` or `speed`; only `MakuScene` does.

### `watercube`

Source:

- `java-desktop/src/main/java/WatercubeScene.java:253-284`
- `reverse/cfr_single/kmajmka.java:236-267`

Accepted payloads:

- `suh`
- `suh0`
- `suh1`
- `suh2`
- `rok`
- `pum`
- `tex0`
- `tex1`
- `tex2`
- `tex3`

Observed effect:

- `suh*` selects overlay presets.
- `rok` starts a rocking impulse.
- `pum` starts a flash/impact burst.
- `tex0` to `tex3` choose hard-coded texture offset presets.

Again, these are local event codes interpreted only by `WatercubeScene`.

### `feta`

Source:

- `java-desktop/src/main/java/FetaScene.java:208-220`
- `reverse/cfr_single/kmaamka.java:237-249`

Accepted payloads:

- `1`
- `2`
- `blackfeta`
- `blackmuna`

Observed effect:

- `1` and `2` switch palette setup modes.
- `blackfeta` and `blackmuna` store timestamps used by later blackening/fade logic.

Important detail:

The script sends `msg feta 1` before `show feta`. That proves `msg` can target an initialized object that is not yet the active scene. `show` and `msg` are separate operations.

### `uppol`

Source:

- `java-desktop/src/main/java/UppolRoutine.java:37-49`
- `reverse/cfr_single/mmaakmk.java:37-49`

Observed effect:

`UppolRoutine` exposes a script name (`uppol`) but does not override `handleMessage(...)`, and the main script does not send any `msg uppol ...` command.

## What The Extra `float` Argument Means

`msg` handlers do not just receive a string. They also receive a time value:

- reconstructed tree: `sceneTimeSeconds - kkaMAJA` at `ForwardDemoApp.java:427` and `432`
- raw tree: `kKAMAJA - kkaMAJA` at `forward.java:404` and `409`

This is the local time elapsed since the most recent `show` command switched the active scene/routine.

That explains why some handlers store the incoming `f` value and later compare render-time `f` against it to start or animate an effect from the message timestamp.

## Related Script Semantics Worth Noting

Two nearby details help explain the script language:

- `init <target>` loads resources into an already-registered object; it does not perform dynamic class lookup.
- `show <target>` switches which registered object is currently rendered.
- `kill <target>` disposes and removes a registered object.
- `go <hex-or-decimal>` inside the top-level script parser is currently a no-op in both trees except for parsing the integer:
  - `ForwardDemoApp.java:354-356`
  - `reverse/cfr_single/forward.java:334-336`

## Conclusion

The best current reading is:

- `msg` belongs to a very small custom script interpreter.
- Scene names like `mute95`, `saari`, `maku`, or `watercube` are registry keys returned by each scene or routine.
- Event nicknames like `saviour`, `suh1`, `rok`, `pum`, or `blackfeta` are plain payload strings interpreted by one target object only.
- No evidence in either tree suggests reflection, function-name lookup, or automatic dispatch from payload string to method name.

So if you want to understand what a given `msg` does, the right workflow is:

1. Identify the target object by script name.
2. Open that target's `handleMessage(...)` implementation.
3. Read the payload-specific branches there.

That is the real dispatch mechanism.
