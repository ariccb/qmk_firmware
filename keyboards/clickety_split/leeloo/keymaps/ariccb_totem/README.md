# ariccb_totem

Firmware size: 27876 / 28672 bytes (796 free) on the Pro Micro.

Totem layout on the Leeloo, so the Totem can be practiced before it arrives.
Source is `ariccb/zmk-config-totem` master (Colemak port and the Raise
mouse/media update). The original `ariccb` keymap is not used by this folder.

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
| =  | +  | -  | _  | *  | /  |     |    |     |Vol-|Wrd |Part|Line|Back|Fwd |
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
- Combo keys are delayed by up to 50 ms (70 ms for L+U+Y and the
  three-key terminal combo) before the hold-tap timer starts. On the
  Pro Micro, L+U+Y is Num Lock. On the KB2040 it latches Nav.
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

## KB2040

The Pro Micro build above is unchanged. KB2040 controllers use a separate image. Put them in the Pro Micro sockets face-down, the same orientation as the Pro Micros.

```
qmk compile -kb clickety_split/leeloo -km ariccb_totem -e CONVERT_TO=kb2040
```

Flash `clickety_split_leeloo_rev1_ariccb_totem_kb2040.uf2` to both halves. One file covers both. USB makes that half the left (`MASTER_LEFT`); the other half is the right and uses the single TRRS data wire. Do not mix a Pro Micro half with a KB2040 half.

This image enables the 128x32 OLEDs with the HELL0 NAVI animation by Nicolas Druoton (druotoni), the same port as the daily `ariccb` KB2040 build. One UF2 covers both halves. The left half (USB, `MASTER_LEFT`) shows the scope: burst, WPM, and a three-letter layer name (`COL`, `QWE`, `CAN`, `LOW`, `NAV`, `RSE`, `SYS`). Colemak, QWERTY, and Canary close the frame. Lower, Nav, Raise, and System open it. The right half shows the ring for the last basic key. Each half draws its own screen, and the USB half sends the keycode across the TRRS link, so both halves need this UF2. On power-up each screen plays the boot sequence. After a few seconds idle the art glitches, then the panel turns off until the next key. Settings live in RP2040 flash instead of EEPROM and start empty, so a new controller boots in Windows mode. Mac and Windows are still the System-layer keys, and they persist across reboot.

The KB2040 image is not the diagram above. It follows current Totem: Lower, Raise, and System (on only while Lower and Raise are both held). Layer keys are momentary. The encoders are the key beside V and the outermost right thumb; those positions are blank. The key beside K types Enter on every layer. Bluetooth, the USB/BLE toggle, and the XIAO LED are not on this board.

Thumbs, left to right, tap / hold:

- Enter / Cmd
- Space / Lower
- Tab / Cmd+Alt
- grave (no hold)
- Backspace / Lower
- Delete / Raise

Lower's right hand is `*789+`, `&456-`, `~123/|`. Its right thumbs are next sentence (period, space, one-shot Shift), period, and 0. The left hand is Totem's symbol block, not the old mouse block.

Raise's left hand is the mouse (scroll and buttons on the top row, move keys on the bottom). The right hand is volume on J and L, speaker mute on U, and Y does nothing. The home row is mic mute, previous track, play/pause, next track. The far-left thumb is Cmd+Enter on Mac and Ctrl+Enter on Windows.

Nav shifts F1–F4 one key right (Z is empty, X–V are F1–F4, F5–F8 and F9–F12 fill the rows above). Previous and next tab sit beside the arrows and send Ctrl+Shift+Tab and Ctrl+Tab on both Mac and Windows, so Windows mode does not turn them into Win+Tab. Back and Forward are under those tab keys. The left thumbs are app windows, Space, and the app switcher (Cmd+Tab held on Mac, Alt+Tab held on Windows). L+U+Y latches Nav. Releasing the pinky's Nav hold can also clear that latch, because QMK layers are not refcounted.

Caps Word is both Shift keys held together, same as the daily keymap. S+E still toggles it, which is the Totem combo. Double-tap Shift does not.

System: Mac is Q, Windows is A, and the bootloader is the outer left pinky (Esc). It only resets the half that has the USB cable. Colemak, QWERTY, and Canary on the right top row still change the saved base layout. Screenshot and find are Cmd+S / Cmd+F, which follow the Ctrl/Cmd swap.

### First flash

1. Hold the KB2040 **BOOT** button.
2. Plug in USB while holding BOOT, then release BOOT.
3. Copy the UF2 onto the `RPI-RP2` drive.

### Bootloader once it is socketed face-down

The BOOT button faces the PCB. Plug USB into the half you want to flash and double-tap that half's Leeloo reset button within half a second. That reset is the RP2040 RUN pin, and a double tap opens the UF2 bootloader.

`QK_BOOT` is the outer left pinky (the Esc key) while Lower and Raise are both held, and it only resets the half that has the USB cable. Move the cable to flash the other half. Flash both halves: the OLEDs on each half show the layer name.
