# Validation status

Local checks: complete workflow parsed; inline PowerShell syntax checked; actual source ZIP extracted and project found by the workflow scripts; parameter/spec counts, UI parameter coverage, preset count and source structure checked; source archive integrity checked. Interface preview was rendered from the control layout and visually inspected.

This environment has no C++ compiler or CMake. Acid VOG has NOT been compiled here; its new engine tests have NOT been executed here. The preview is an illustrative render, not a screenshot of a running plugin. Listening, MIDI export interoperability, session restore, DAW transport and plugin-validator checks remain to be performed after a successful Windows build.

The included C++ tests check step packing, swing/clock boundaries, scene ranges and uniqueness, finite audio, note/slide/reset behavior and 44.1/48/96 kHz operation. GitHub runs these before packaging.

Manual checks: preview all 256 scenes; test every pattern flag and gate length; start/stop/loop/jump DAW transport; try internal clock and MIDI keyboard modes; save/load edited banks; export/reimport MIDI; test automation, UI scaling and CPU at the buffer size you use. The voice is an original analog-style model and makes no circuit-authenticity claim.

## 1.0.1 compiler correction
The GitHub log reported C2666 for fillRect(float,float,float,int) in PluginEditor.cpp. The flag cells and pitch cells now use explicit Rectangle<float> arguments. Other fillRect calls use either typed rectangles or four integer arguments. Compilation of this correction remains to be verified by GitHub.

## 1.0.8
Source structure and complete ZIP/workflow checks were repeated. A separate AcidLongRun C++ test target now checks 600 seconds of audio at 48 kHz, shorter 44.1/96 kHz runs, factory scene changes, high feedback, DC, peak ceiling, reset and tail decay. These new DSP tests have not run locally. No listening/Reason hardware session was available, and the exact minutes-later scream has not been reproduced. The changes are a mitigation, not a verified diagnosis or confirmed fix.
