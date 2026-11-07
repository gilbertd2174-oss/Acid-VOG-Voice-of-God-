# 1.0.8 — long-run stability mitigation
Acid-local bounded filter; reduced extreme resonance/feedback; damped/DC-filtered echo; bounded reverb state; output DC/HF filtering, startup fade, -6 dBFS ceiling and pre-clip fault reset diagnostics. Cached effects coefficients reduce per-sample work. Added 10-minute audio soak and tail-decay build test. Exact reported failure and compiled audio behavior remain unverified locally.

# 1.0.7
Darker walnut main panel and charcoal-black knobs with gold pointers. Cream labels maintain contrast; numeric readouts remain black on light backgrounds. Dark side rails, gold trim and bright playhead retained.

# 1.0.6
Light wood-grain main panel, dark wood side rails and gold top/bottom trim. Cache the generated main-panel texture between repaints. Keep the frame visible while scrolling the controls.

# 1.0.5
Higher-contrast sequencer playhead: yellow column tint and 3-pixel frame across note and flag lanes, dark header with larger yellow step number, and black outline around the active note. Read the active step once per repaint so each frame highlights one consistent column.

# 1.0.4
Add previous/next patch buttons beside the scene selector. Buttons wrap between patch 001 and 256 and immediately update the displayed selection.

# 1.0.3
Add 128 new patches for 256 total. New banks: Deep Sub, Pluck, Metallic, Space, Echo Motion, Minimal, Slow Burn and Peak Time. Existing patch numbers and sound settings remain unchanged.

# 1.0.2
Darker grey panel background; pure black numeric readouts, editable numeric text and sequencer numbers; light number boxes and sequencer headings preserve contrast.

# 1.0.1
Fix MSVC C2666 in the step-flag drawing call. Both sequencer cell drawing calls now use explicit Rectangle<float> arguments to avoid mixed int/float overload selection.

# 1.0.0
Initial Acid VOG: monophonic acid voice, slide/accent/vibrato, 16-slot step sequencer, 128 factory scenes, effects, patch save/load, MIDI export, silver interface and Windows build workflow.
