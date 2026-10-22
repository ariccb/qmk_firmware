# ariccb_totem

Firmware size: 27504 / 28672 bytes (1168 free) on the Pro Micro.

Totem layout on the Leeloo, so the Totem can be practiced before it arrives.
Source is `ariccb/zmk-config-totem` at `3d729d3`. The original `ariccb` keymap
is not used by this folder.

Blank cells are `KC_NO` on every layer. The 3x5 blocks sit on the inner
columns. The outer pinkies are the outer column of the bottom alpha row.
The three Totem thumbs are the three innermost thumb keys on each side.

```
Colemak (default). QWERTY and Canary keep the same thumbs and pinkies.
.------------------------------------------------.      .------------------------------------------------.
|    |    |    |    |    |    |                  |    |    |    |    |    |    |
|    | Q  | W  | F  | P  | B  |                  | J  | L  | U  | Y  | ;  |    |
|    | A  | R  | S  | T  | G  |                  | M  | N  | E  | I  | O  |    |
|Esc | Z  | X  | C  | D  | V  | Num |    | Sym | K  | H  | ,  | .  | /  |Bspc|
|Hypr|    |    |    |    |    | Spc |    | Ent |    |    |    |    |    |Hypr|
'------------------------------------------------'      '------------------------------------------------'
                    |    |    | Sft | Nav |    | Adj | Del |    |    |
                    |    |    |  *  | Tab |    |Bspc |     |    |    |
                    '---------------------'    '---------------------'
Hold: A=Nav, R=Alt, S=Shift, T=Ctrl, G=Cmd, M=Cmd, N=Ctrl, E=Shift, I=Alt.
O has no mod. * is a one-shot Shift.
```

```
Num (left inner thumb). Home row is transparent, so the mods still chord.
.------------------------------------------------.      .------------------------------------------------.
|    |    |    |    |    |    |                  |    |    |    |    |    |    |
|    |WhUp| Mb2|MsUp| Mb1| Mb3|                  | 7  | 8  | 9  |  - |  / |    |
|    |    |    |    |    |    |                  | 4  | 5  | 6  |  * |  = |    |
|WhDn|MsLf|MsDn|MsRt|    |    |     |    | Ent | 1  | 2  | 3  | .  | 0  |  + |
'------------------------------------------------'      '------------------------------------------------'
                    |    |    |     |     |    |Bspc | Del |    |    |
                    '---------------------'    '---------------------'
```

```
Nav (left home pinky, or the middle left thumb).
.------------------------------------------------.      .------------------------------------------------.
|    |    |    |    |    |    |                  |    |    |    |    |    |    |
|    | F1 | F2 | F3 | F4 | F5 |                  |Ctl+|Home| Up | End|Nav |    |
|    | F6 | F7 | F8 | F9 | F10|                  |C-STb|Lf| Dn | Rt |C-Tb|    |
|    | F11| F12| Z- | Z+ | Z0 | Spc |    | Ent |CtlTb|PgUp|Cur+|PgDn|Fwd |Cmd*|
'------------------------------------------------'      '------------------------------------------------'
                    |    |    |     |     |    |Bspc | Del |    |    |
                    '---------------------'    '---------------------'
Ctl+ is Ctrl+PgUp (Win+PgUp in Windows mode). Z-/Z+/Z0 follow Cmd/Ctrl.
Cmd* is a sticky Cmd, which becomes Ctrl in Windows mode.
```

```
Sym (right inner thumb).
.------------------------------------------------.      .------------------------------------------------.
|    |    |    |    |    |    |                  |    |    |    |    |    |    |
|    | {  | }  | <  | >  | |  |                  |Vol+|Mute|Mic |Ply |Next|    |
|    | (  | )  | [  | ]  | \  |                  |    |    |    |    |    |    |
| /  | =  | +  | -  | _  | *  |     |    |     |Vol-|Wrd |Part|Line|Back|Fwd |
'------------------------------------------------'      '------------------------------------------------'
                    |    |    |     |     |    |Bspc | Del |    |    |
                    '---------------------'    '---------------------'
```

```
Adjust (middle right thumb).
.------------------------------------------------.      .------------------------------------------------.
|    |    |    |    |    |    |                  |    |    |    |    |    |    |
|    |Rbt |    |    |    |    |                  |Col |Qwe |Can | () |Adj |    |
|    |Boot|    |    |    |    |                  |Num |Nav |Sym | `  |Slp |    |
|    |    |Bri+|Bri-|Rpt |Caps| Spc |    | Ent |Ins |Psc | => | [] |Caps|    |
'------------------------------------------------'      '------------------------------------------------'
                    |    |    |     | Tab |    |     | Del |    |    |
                    '---------------------'    '---------------------'
Col / Qwe / Can are saved in EEPROM. Shift changes () to <>, [] to {},
and => to the long arrow snippet.
```

```
System, while both inner thumbs are held.
.------------------------------------------------.      .------------------------------------------------.
|    |    |    |    |    |    |                  |    |    |    |    |    |    |
|    |Mac |Win |    |    |    |                  |    |Save|Find|    |    |    |
|    |Boot|Rbt |    |    |    |                  |    |    |    |    |Slp |    |
|    |    |    |    |    |    | Num |    | Sym |    |    |    |    |    |    |
'------------------------------------------------'      '------------------------------------------------'
Mac is CG_NORM. Win is CG_SWAP. A cleared EEPROM boots into Windows.
```

Encoders: left knob is volume (clockwise quieter, same direction as the
stock Leeloo), right knob is mouse-wheel scroll. Those HID usages are the
same on Mac and Windows.

QMK 0.19.2 has no chordal hold and no flow tap, so home-row mods are a
small engine in this folder:

- Prior-idle, opposite-hand triggers, and balanced hold-on-release match
  the Totem's timeless mods. Two mods on one hand can be held together.
- Left pinky turns Nav on when the other hand presses, so an arrow repeats.
  The Totem's balanced `a_nav` would wait for that key to be released; the
  keymap comment asks for the press.
- Quick-tap windows are the Totem's 175 / 200 / 120 ms.
- Combo keys are delayed by up to 50 ms (70 ms for Num Lock and the
  three-key terminal combo) before the hold-tap timer starts.
- Bluetooth select, clear, next, previous, and the USB/BLE output toggle
  are `KC_NO`.
- One-shot Shift clears when the next key is pressed. ZMK also has
  quick-release.
- `TG()` stays on until it is toggled again. There is no 120 s layer-lock
  timeout, and QMK layers are not refcounted, so releasing a layer-tap can
  also clear a toggle of that same layer.
- Colemak / QWERTY / Canary are saved across reboot. ZMK `&to` is not.
- Adaptive keys are the 2025-07-24 set (sv to sc, pf to pt, fw to fs, and
  the rest in `features/adaptive_keys.def`), 350 ms, suppressed by Ctrl,
  Alt, or Cmd. The QMK engine clears Shift while it substitutes; ZMK does
  not. Key repeat remembers the last unsubstituted basic key.
- Paste-without-formatting is Cmd/Ctrl+Shift+V only.
- Cmd-Tab follows the Ctrl/Cmd swap, so Windows mode sends Ctrl-Tab.
  Ctrl-Tab sends Win-Tab. There is no Alt-Tab app switcher.
