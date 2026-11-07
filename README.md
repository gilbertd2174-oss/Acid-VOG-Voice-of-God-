# Acid VOG

A new Windows x64 VST3 instrument and standalone acid-bass sequencer inspired by the supplied vintage silver-panel reference. Original UI and original acid voice; no reference-product artwork, branding, samples or proprietary code. This is not a sound-identical circuit emulation.

## Play immediately
- Choose one of 256 factory scenes, then press START / ARM. Use the < and > buttons beside the selector to browse patches; they wrap at the first/last patch.
- Clock = Internal runs at the Tempo control. Clock = DAW transport follows the DAW's tempo and PPQ position when supplied; arm the sequencer and start your DAW transport.
- Play mode = MIDI keyboard accepts notes from an instrument track or the standalone MIDI input. This mode does not require START. Pitch bend is ±2 semitones; modulation wheel adds vibrato.
- Put the VST3 on a MIDI/instrument track. It is not an audio insert effect.
- STOP releases the current note and leaves effects tails. PANIC stops the sequencer and clears notes/effects.

## Voice and effects
Band-limited saw or pulse oscillator, sine sub oscillator, four-pole low-pass response from cascaded state-variable filters, resonant saturation, tuning, decaying filter envelope, accent envelope, per-step slide and vibrato. No sampled sound library is required.

Effects: tube/fuzz/saturation distortion with color and wet/dry mix, stereo chorus, tape-style echo with dotted-eighth sync, algorithmic reverb with room size and pre-delay, output level/meter and soft output protection. Effects run in fixed order: distortion → chorus → echo → reverb → output. The effects use the DSP helper code from the successfully built VOG project; the new acid voice and sequencer need their own build/testing.

## 256 sound presets
Sixteen banks of 16: Classic, Pulse, Grit, Dub, Glide, Hypnotic, Atmosphere, Hard Rave, Deep Sub, Pluck, Metallic, Space, Echo Motion, Minimal, Slow Burn and Peak Time. Each scene stores a complete sound/clock setup and selects one of 16 editable patterns. Selecting a factory scene does not erase edits already made in a pattern slot. Save a .vacid patch to preserve your complete edited bank.

## Pattern editor
- Sixteen pattern slots: A1–A4, B1–B4, C1–C4, D1–D4.
- Sixteen steps per pattern, selectable length 1–16; four clock divisions; shuffle 0–60%; gate 10–95%; transpose ±24 semitones.
- Click a grid cell to set its pitch and enable the step. Click that same note again to toggle its gate. Grid range is C2–B3.
- Click GATE, ACCENT, SLIDE or VIBRATO below a column to toggle that step's flag.
- SLIDE marks the destination step. The previous enabled note is held into that step, pitch glides, and the filter envelope continues. A rest breaks the tie.
- COPY and PASTE use an internal pattern clipboard. SHIFT rotates the pattern by one step. CLEAR disables all gates. RANDOM makes a new minor-pentatonic pattern. Changes are picked up during playback; a note edit triggers when its step is next reached. There is no pattern undo history.
- DAW sessions and SAVE PATCH / LOAD PATCH store all sound settings and all 16 pattern slots. Transport always resumes stopped after loading a patch/session.
- EXPORT MIDI writes note pitches, accent velocities, shuffle, gate timing and a tempo event for the selected pattern. It does not encode slide/vibrato sound behavior or the audio effects.

## Build on GitHub
Use a NEW repository for Acid VOG.
1. Upload Acid-VOG-v1.0.8-Stability.zip intact to the root and commit.
2. Create .github/workflows/main.yml; paste ALL the text from the supplied Acid-VOG-v1.0.8-main.yml, then commit.
3. Actions → Acid VOG Windows Build. The workflow automatically extracts the ZIP, compiles VST3/standalone and runs the engine tests.
4. After a green build, download Acid-VOG-Windows-x64 under Artifacts. Extract it and any inner ZIP. Follow INSTALL.md.

Alternatively extract the source and upload ALL contents, including Source, Tests, scripts and .github. Do not upload only CMakeLists.txt or only the workflow.

## Build locally on Windows
Install Visual Studio 2022 with Desktop development with C++, Windows SDK, CMake tools, and Git for Windows. From the extracted folder:

    ./scripts/build-windows.ps1 -Configuration Release -Parallel 2

First configure downloads JUCE 8.0.6. Output: dist/Acid-VOG-Windows-x64.zip.
Engine-only checks: configure CMake with -DDAVOE_ENGINE_ONLY=ON -DBUILD_TESTING=ON, build and run ctest.

See VALIDATION.md for the actual verification status. The source ZIP is not a compiled plugin.

## 1.0.8 stability update
A reported screaming sound after several minutes has not been reproduced in this environment. This update contains mitigations: an acid-local cutoff/resonance-limited filter, checks on internal states before output clipping, DC removal and damping inside echo feedback, bounded reverb returns, output DC removal/treble roll-off, a 25 ms fade after resetting and a -6 dBFS absolute sample ceiling. Abnormal levels/non-finite states reset the engine and briefly mute it; the UI displays OUTPUT GUARD with a reset count when that happens. These protections apply to restored patches as well as fresh instances.

The upper resonance and feedback ranges are limited internally; high-resonance/high-drive presets and long tails will sound softer than earlier builds. Effect coefficients that were recalculated for every sample are cached during parameter updates.

The Windows build now includes a 600-second audio soak at 48 kHz, 30-second checks at 44.1/96 kHz, rapid notes/slides/scene changes, and a 20-second tail-decay check. These are rendered audio durations; execution time depends on the runner. The new C++ checks have NOT run locally. The package is source, not a tested binary or a confirmed cure for the reported failure.

To update an existing repository, replace its old source ZIP and .github/workflows/main.yml with the supplied 1.0.8 files. The workflow prefers the new stability ZIP when an older extracted source tree is also present. After a green build, close your DAW, replace the old Acid VOG.vst3 bundle, reopen/rescan, and verify the plugin header says 1.0.8 STABILITY UPDATE. Start testing quietly with patch 001. If the issue returns, stop playback and report the preset plus the OUTPUT GUARD count, if displayed.
