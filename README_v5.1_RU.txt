X-SIDE VST3 v5.1 FULL DSP MODE

Base:
stable v5.0 Preset Browser.

Hardware-accurate DSP1000P MIDI functions added:
- CC22 ENGINE:
  0 = Couple
  1 = Left
  2 = Right
- CC30 MIX MODE:
  0 = Mix Internal
  1 = Mix External
- Existing CC20..29 remain active.

Interface:
- STEREO / COUPLE
- LEFT ENGINE
- RIGHT ENGINE
- MIX INTERNAL / MIX EXTERNAL
- Dual-Mode status display.

Dual Mode algorithms 25..32:
25 Pitch / Reverb      LEFT Pitch      RIGHT Reverb
26 Flanger / Reverb    LEFT Flanger    RIGHT Reverb
27 Chorus / Reverb     LEFT Chorus     RIGHT Reverb
28 Tremolo / Reverb    LEFT Tremolo    RIGHT Reverb
29 Delay / Echo        LEFT Delay      RIGHT Echo
30 Pitch / Echo        LEFT Pitch      RIGHT Echo
31 Flanger / Echo      LEFT Flanger    RIGHT Echo
32 Chorus / Echo       LEFT Chorus     RIGHT Echo

Important:
DSP1000P MIDI implementation does NOT expose a separate left-output mute
or right-output mute command. X-Side therefore does not invent such a command.
LEFT ENGINE / RIGHT ENGINE selects which hardware engine is edited.

For mixer AUX send/return operation use MIX EXTERNAL (100% wet).
For serial insert/guitar loop use MIX INTERNAL and control Dry/Wet.

MIDI device opening remains manual and Safe Host behavior is unchanged.
