/* Copyright 2022 Clickety Split Ltd.
 *                https://clicketysplit.ca
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include <stdio.h>

// #include "ps2_mouse.h"
// #include "mousekey.h"

#include "features/select_word.h"
#include "features/adaptive_keys.h"
#include "features/layer_lock.h"
#include "features/os_mode.h"
#ifdef OLED_ENABLE
#include "navi/oled_navi.h"
#endif

/*
 * Mac / Windows mode follows the Raise-layer Ctrl/Cmd swap and persists in
 * EEPROM (keymap_config.swap_lctl_lgui, saved by CG_NORM / CG_SWAP).
 *
 * Home-row and thumb mods are written as a Mac board: Cmd is MOD_LGUI, Ctrl
 * is MOD_LCTL, Alt stays Alt. is_mac() is true when the swap is off.
 *
 *   Raise, top-left outer key (CG_NORM) — Mac. Shortcuts send Cmd, and the
 *   Cmd thumb sends Cmd / the Ctrl thumb sends Ctrl.
 *   Key directly below it (CG_SWAP) — Windows. Shortcuts send Ctrl, and the
 *   swap makes the Cmd thumb send Ctrl / the Ctrl thumb send Win.
 *
 * A fresh EEPROM boots in Windows mode (eeconfig_init_user). An EEPROM that
 * was already initialized keeps its saved swap until you press one of those
 * keys or clear EEPROM.
 *
 * Mod-taps cannot choose a modifier at runtime, so their holds stay
 * Mac-labeled and QMK's CG swap (mod_config) retargets Ctrl <-> GUI. The ")"
 * key is a tap dance, which skips mod_config; its hold calls
 * keycode_config(KC_LGUI) so it follows the same swap. Chords sent with
 * register_code / tap_code16 also skip mod_config, so each one branches on
 * is_mac() and emits the real Cmd or Ctrl chord.
 *
 * mod_config's Ctrl/GUI swap is if/else-if. A hold that sets both LCTL and
 * LGUI hits the GUI test, clears GUI, and never runs the Ctrl test, so GUI
 * is dropped instead of swapped. HYPERESC is the only hold that sets both.
 * process_record_user registers MOD_HYPR (Ctrl+Alt+Shift+GUI) directly, in
 * both modes, and returns false so the swapped mod-tap action does not run.
 * CO_TAB is written as Cmd+Alt. Only GUI is set, so the swap turns it into
 * Ctrl+Alt on Windows and leaves Alt. OS_LBRC / OS_RBRC / OS_ENT are the
 * Raise bracket and Raise/Adjust Enter chords: Cmd on Mac, Ctrl on Windows.
 * Matrix G() / LGUI() chords do not go through mod_config.
 */

// #include "features/caps_word.h"
#ifdef CONVERT_TO_KB2040
#    include "features/autocorrection.h"
#endif

// Variables for repeat key functionality
static uint16_t last_keycode = KC_NO;
static keyrecord_t last_record;

// Variables for super app tab functionality
bool super_app_tabbing = false;
fast_timer_t super_app_timer = 0;
#define SUPER_APP_TAB_TIMEOUT 2500  // 2.5 seconds

// Function declarations
void super_app_tab_start(void);
void super_app_tab_forward(void);
void super_app_tab_backward(void);
void super_app_tab_stop(void);
void repeat_key_invoke(keyevent_t* event);

// Tap Dance declarations
enum {
    TD_LSFT_LPRN,  // Left Shift on hold, ( on tap
    TD_LGUI_RPRN,  // Left GUI on hold, ) on tap
};

// Tap Dance function declarations
void lsft_lprn_finished(qk_tap_dance_state_t *state, void *user_data);
void lsft_lprn_reset(qk_tap_dance_state_t *state, void *user_data);
void lgui_rprn_finished(qk_tap_dance_state_t *state, void *user_data);
void lgui_rprn_reset(qk_tap_dance_state_t *state, void *user_data);

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skiep them
// entirely and just use numbers.
enum layer_names { _COLEMAKDH, _QWERTY, _CANARY, _LOWER, _RAISE, _ADJUST };
#ifdef OLED_ENABLE
// navi/layer_frame.c labels these numbers CDH QWE CAN LOW RSE ADJ.
_Static_assert(_COLEMAKDH == 0 && _QWERTY == 1 && _CANARY == 2 && _LOWER == 3 && _RAISE == 4 && _ADJUST == 5, "navi layer map");
#endif

#define XXXXX KC_NO
#define _____ KC_TRNS
// Esc on tap. Hold is registered in process_record_user as MOD_HYPR
// (Ctrl+Alt+Shift+GUI) in both modes. Leaving it on the stock MT path runs
// mod_config, which drops GUI when Ctrl is also set.
#define HYPERESC MT(MOD_LCTL | MOD_LGUI | MOD_LALT | MOD_LSFT, KC_ESC)
#define RSE_SPC LT(_RAISE, KC_SPC)                                     // Raise on hold, One Shot Shift on tap
#define LOW_SPC LT(_LOWER, KC_SPC)                                     // lower on hold, One Shot Shift on tap
#define RSE_DEL LT(_RAISE, KC_DEL)                                     // raise on hold, Delete on tap
#define L_ADJ_0 LT(_ADJUST, KC_0)                                  // Adjust Layer on hold, 0 on tap
#define L_ADJ_SPC LT(_ADJUST, KC_SPC)                                  // Adjust Layer on hold, Space on tap
#define ALT_GRV MT(MOD_LALT, KC_GRV)                                   // alt on hold, grave ` on tap
#define MTLGUI_STAB MT(MOD_LGUI, LSFT(KC_TAB))                         // GUI on hold, Shift Tab on tap
#define MTRSFTBSLS MT(MOD_RSFT, KC_BSLS)                               // Shift on hold, \ on tap
#define MTRCTLQUO MT(MOD_RCTL, KC_QUOT)                                // Ctrl on hold, " on tap
// Cmd+Alt on hold, Tab on tap. CG swap replaces GUI with Ctrl and keeps Alt,
// so the Windows hold is Ctrl+Alt. Only one of Ctrl/GUI is set, so the
// mod_config if/else-if does not drop a modifier. The thumb MTALT_TS is
// Alt on hold and Ctrl+Tab on tap; it is not this chord.
#define CO_TAB MT(MOD_LGUI | MOD_LALT, KC_TAB)
#define FN_A LT(_ADJUST, KC_A)
#define FN_C LT(_ADJUST, KC_C)
#define FN_O LT(_ADJUST, KC_O)
#define FN_SCLN LT(_ADJUST, KC_SCLN)

// Single-mod holds are Mac-labeled. CG_SWAP turns a GUI hold into Ctrl and a
// Ctrl hold into Win. Alt holds are not part of that swap. CO_TAB is Cmd+Alt
// so that same swap yields Ctrl+Alt on Windows. HYPERESC, OS_LBRC, OS_RBRC,
// OS_ENT, BWSRLEFT, BWSRRHGT, CRSR_UP, CRSR_DN, and ZOOM_RESET are handled
// in process_record_user.
#define MTCMD_ENT MT(MOD_LGUI, KC_ENT)        // Cmd on hold, Enter on tap
#define MTCTL_OSS MT(MOD_LCTL, KC_F24)        // Ctrl on hold, one-shot shift on tap
#define MTLALT_Z MT(MOD_LALT, KC_Z)           // Alt on hold, Z on tap
#define MTLALT_W MT(MOD_LALT, KC_W)           // Alt on hold, W on tap
#define ALT_SLSH MT(MOD_LALT, KC_SLSH)        // Alt on hold, / on tap
#define MTALT_TS MT(MOD_LALT, CTRL_TAB)       // Alt on hold, Ctrl+Tab on tap
#define MTLALT_LRBC MT(MOD_LALT, KC_LBRC)     // Alt on hold, [ on tap
#define MTLSFT_LPRN TD(TD_LSFT_LPRN)          // Shift on hold, ( on tap
#define MTLGUI_RPRN TD(TD_LGUI_RPRN)          // Cmd on hold, ) on tap (tap dance; see keycode_config)
#define MTLCTL_RBRC MT(MOD_LCTL, KC_RBRC)     // Ctrl on hold, ] on tap
#define MT_REP MT(MOD_LCTL, KC_0)             // Ctrl on hold, repeat on tap
#define MTALT_F5 MT(MOD_LALT, KC_F5)          // Alt on hold, F5 on tap
#define MTLSFT_F6 MT(MOD_LSFT, KC_F6)         // Shift on hold, F6 on tap
#define MTGUI_F7 MT(MOD_LGUI, KC_F7)          // Cmd on hold, F7 on tap
#define MTCTL_F8 MT(MOD_LCTL, KC_F8)          // Ctrl on hold, F8 on tap

enum planck_keycodes {
    COLEMAKDH = SAFE_RANGE,
    QWERTY,
    CANARY,
    LOWER,
    RAISE,
    ADJUST,
    NUMPAD,
    GAMING,
    EXT_NUM,
    SELPART,
    SELWORD,
    BRACES,
    BRACES2,
    ARROW,
    CTRL_TAB,
    CTRL_S_TAB,
    CMD_TAB,
    CMD_S_TAB,
    LLOCK,
    SELWLEFT,
    BWSRLEFT,    // Mac: Cmd+Left. Windows: Alt+Left.
    BWSRRHGT,    // Mac: Cmd+Right. Windows: Alt+Right.
    CRSR_UP,     // Mac: Cmd+Opt+Up. Windows: Ctrl+Alt+Up.
    CRSR_DN,     // Mac: Cmd+Opt+Down. Windows: Ctrl+Alt+Down.
    ZOOM_RESET,  // Mac: Cmd+0. Windows: Ctrl+0.
    OS_LBRC,     // Mac: Cmd+[ . Windows: Ctrl+[ .
    OS_RBRC,     // Mac: Cmd+] . Windows: Ctrl+] .
    OS_ENT,      // Mac: Cmd+Enter. Windows: Ctrl+Enter.
};
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* MIT Layout (COLEMAKH-DH)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |NON-US#|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0    |QWERTY|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC| Q   |  W   |  F   |  P   |  B   |                                      |  J   |  L   |  U   |  Y   |  ;   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CO_TAB| FN_A|  R   |  S   |  T   |  G   |-------.                      .-------|  M   |  N   |  E   |  I   |  O   |CTRL,'|
 * |------+------+------+------+------+------|Select |                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| ALT_Z|  X   |  C   |  D   |  V   |Words l/r|-->Toggle Microphone| GRV`  |  K   |  H   |  ,   |  .   |ALT,/ |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                       \Repeat   \  LOW  |RAISE/ |Volume Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                          \Key      \ SPACE| DEL   |DIAL2 | -> TOGGLE play/pause On Press
 *                      `-------------------------'                            '--------------------------'
 */
  [_COLEMAKDH] = LAYOUT(
  KC_NUHS,  KC_1,     KC_2,   KC_3,      KC_4,      KC_5,                                KC_6,    KC_7,    KC_8,    KC_9,   KC_0,     QWERTY,
  HYPERESC, KC_Q,     KC_W,   KC_F,      KC_P,      KC_B,                                KC_J,    KC_L,    KC_U,    KC_Y,   KC_SCLN,  KC_BSPC,
  CO_TAB,   FN_A,     KC_R,   KC_S,      KC_T,      KC_G,                                KC_M,    KC_N,    KC_E,    KC_I,   KC_O,     MTRCTLQUO,
  KC_LSFT,  MTLALT_Z, KC_X,   KC_C,      KC_D,      KC_V,   C(A(G(KC_M))),     ALT_GRV, KC_K,    KC_H,    KC_COMM, KC_DOT, ALT_SLSH, MTRSFTBSLS,
                              MTALT_TS, MTCMD_ENT, LOW_SPC, MTCTL_OSS,         MT_REP,  LOW_SPC, RSE_DEL, KC_MPLY
),

/* MIT Layout (QWERTY)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |NON-US#|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0    |CANARY|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC| Q   |  W   |  E   |  R   |  T   |                                      |  Y   |  U   |  I   |  O   |  P   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CO_TAB|FN_A |  S   |  D   |  F  -@-  G  |-------.                      .-------|  H   |  J   |  K   |  L   | FN_; |CTRL,'|
 * |------+------+------+------+------+------|Select |                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| ALT_Z|  X   |  C   |  V   |  B   |Words l/r|-->Toggle Microphone| GRV`  |  N   |  M   |  ,   |  .   |ALT,/ |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                      \Repeat \  LOW  |RAISE/ |Volume Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                         \Key    \ SPACE| DEL   |DIAL2 | -> TOGGLE play/pause On Press
 *                      `-------------------------'                           '-------------------------'
 */
  [_QWERTY] = LAYOUT(
  KC_NUHS,  KC_1,     KC_2,   KC_3,      KC_4,      KC_5,                                 KC_6,    KC_7,    KC_8,    KC_9,   KC_0,    CANARY,
  HYPERESC, KC_Q,     KC_W,   KC_E,      KC_R,      KC_T,                                KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,     KC_BSPC,
  CO_TAB,   FN_A,     KC_S,   KC_D,      KC_F,      KC_G,                                KC_H,    KC_J,    KC_K,    KC_L,   KC_SCLN,  MTRCTLQUO,
  KC_LSFT,  MTLALT_Z, KC_X,   KC_C,      KC_V,      KC_B,   C(A(G(KC_M))),     ALT_GRV,  KC_N,    KC_M,    KC_COMM, KC_DOT, ALT_SLSH, MTRSFTBSLS,
                              MTALT_TS, MTCMD_ENT, LOW_SPC, MTCTL_OSS,         MT_REP,   LOW_SPC, RSE_DEL, KC_MPLY // Toggle microphone
),
/* MIT Layout (CANARY)
 * .-----------------------------------------.                                      .-----------------------------------------.          Slightly modified: Q and W swap
 * |NON-US#|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0    |COLEMAKDH|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC|  Q   |  L   |  Y   |  P   |  B   |                                      |  Z   |  F   |  O   |  U   |  ;   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CO_TAB| FN_C |  R   |  S   |  T   |  G   |-------.                      .-------|  M   |  N   |  E   |  I   |  A   |CTRL,'|
 * |------+------+------+------+------+------|Select |                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| ALT_W|  J   |  V   |  D   |  K   |Words l/r|-->Toggle Microphone| GRV`  |  X   |  H   |  ,   |  .   |ALT,/ |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                      \Repeat \  LOW  |RAISE/ |Volume Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                         \Key    \ SPACE| DEL   |DIAL2 | -> TOGGLE play/pause On Press
 *                      `-------------------------'                           '-------------------------'
 */
  [_CANARY] = LAYOUT(
  KC_NUHS,  KC_1,     KC_2,   KC_3,      KC_4,      KC_5,                                 KC_6,    KC_7,    KC_8,    KC_9,   KC_0,    COLEMAKDH,
  HYPERESC, KC_Q,     KC_L,   KC_Y,      KC_P,      KC_B,                                KC_Z,    KC_F,    KC_O,    KC_U,   KC_SCLN,  KC_BSPC,
  CO_TAB,   FN_C,     KC_R,   KC_S,      KC_T,      KC_G,                                KC_M,    KC_N,    KC_E,    KC_I,   KC_A,     MTRCTLQUO,
  KC_LSFT,  MTLALT_W, KC_J,   KC_V,      KC_D,      KC_K,   C(A(G(KC_M))),     ALT_GRV,  KC_X,    KC_H,    KC_COMM, KC_DOT, ALT_SLSH, MTRSFTBSLS,
                              MTALT_TS, MTCMD_ENT, LOW_SPC, MTCTL_OSS,         MT_REP,   LOW_SPC, RSE_DEL, KC_MPLY // Toggle Microphone
),



/* MIT Layout (LOWER) // couldn't get mod-tap to work with LPRN and RPRN
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |NON-US#|  F1 |  F2  |  F3  |  F4  |  F5  |                                      |  F6  |  F7  |  F8  |  F9  |  F10 |LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
*  |  `   |  !   |  <   |  >   |  =   |  ^   |                                      |   *  |   7  |   8  |   9  |   :  | Bsp  |
 * |------+------+------+------+-------------|                                      |------+------+------+------+------+------|
 * |S(TAB)|  #   |  [Alt| ( Sft| ) Cmd| ] Ctl|-------.                      .-------|   &  |   4  |   5  |   6  |   -  |  +   |
 * |------+------+------+------+------+------|Brightness|Haze(shift)        |       |------+------+------+------+------+------|
 * |SHIFT |  @   |  $   |  {   |  }   |  %   | DIAL1 |--> Press for         |   ~   |   0  |   1  |   2  |   3  |   /   |  |  |
 * .-----------------------------------------|-------|   haze On/Off        |-------|-----------------------------------------'
 *                      | ALT | CMD  | ADJUST/ LCTRL/                       \  Alt  \ ADJUST|     |ZOOM IN/OUT|
 *                      | APP | ENTER| ***** / OSSft/                         \Repeat\  .  | DOT | DIAL2|--> Next Song on Press
 *                      `-------------------------'                            '-------------------------'
 */
  [_LOWER] = LAYOUT(
  KC_NUHS,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                               KC_F6,     KC_F7,  KC_F8, KC_F9, KC_F10,  LLOCK,
  KC_TILD,   KC_EXLM, KC_LABK, KC_RABK, KC_EQL,  KC_CIRC,                             KC_ASTR,   KC_7,   KC_8,  KC_9,  KC_COLN, KC_BSPC,
  S(KC_TAB), KC_HASH, MTLALT_LRBC, MTLSFT_LPRN, MTLGUI_RPRN, MTLCTL_RBRC,             KC_AMPR,   KC_4,   KC_5,  KC_6,  KC_PMNS, KC_PPLS,
  KC_LSFT,   KC_AT,   KC_DLR,  KC_LCBR, KC_RCBR, KC_PERC, C(A(KC_Q)),        KC_TILD, KC_0,      KC_1,   KC_2,  KC_3,  KC_PSLS, KC_PIPE,
                               _____,   _____,   L_ADJ_SPC,   _____,           _____, L_ADJ_0, KC_DOT, KC_MNXT // Next Song on press
),
/* MIT Layout (RAISE)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * | MAC  |  F1  |  F2  |  F3  |  F4  |  F5  |                                      |  F6  |  F7  |  F8  |  F9  |  F10 |LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * | WIN  |  Mb3 |  Mb2 | MsUp |  Mb1 |      |                                      | Mute |  Mb1 | Mb3 | Mb2  |  Mb3 | Bksp |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |      |      | MsLft| MDown|Mright|      |-------.                      .-------| Vol+ |SELPART|SELWORD|OS '['|OS ']'|****|
 * |------+------+------+------+------+------|Undo/Redo|                    |       |------+------+------+------+------+------|
 * |      |ScLeft| ScUp |ScDown|ScRight|     | DIAL1 |--> Press for         |       | Vol- |BRACES|BRACES2|ARROW|      |      |
 * .-----------------------------------------|-------|  Next Track          |-------|-----------------------------------------'
 *                      | ALT | OS   |  ADJUST  / LCTRL/                     \Repeat\       |******|Zoom in/out|
 *                      | APP | ENT  | ***** / OSSft/                         \ key  \      |******| DIAL2|--> //RESET ZOOM on Press
 *                      `-------------------------'                            '--------------------------'
 * MAC is CG_NORM (Cmd shortcuts, mods unswapped). WIN is CG_SWAP (Ctrl shortcuts).
 * OS '[' / OS ']' / OS ENT are Cmd on Mac and Ctrl on Windows.
 */
  [_RAISE] = LAYOUT(
  CG_NORM, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                                  KC_F6,   KC_F7,   KC_F8,   KC_F9,      KC_F10,     LLOCK,
  CG_SWAP, KC_BTN3, KC_BTN2, KC_MS_U, KC_BTN1, _____,                                  KC_MUTE, KC_BTN1, KC_BTN3, KC_BTN2,    KC_BTN3,    KC_BSPC,
  _____,   _____,   KC_MS_L, KC_MS_D, KC_MS_R, _____,                                  KC_VOLU, SELPART, SELWORD, OS_LBRC,    OS_RBRC,    _____,
  _____,   KC_MS_L, KC_MS_U, KC_MS_D, KC_MS_R, _____,   ZOOM_RESET,           _____,   KC_VOLD, BRACES2, BRACES,  ARROW,      _____,      _____,
                            _____, OS_ENT, L_ADJ_SPC, _____,                  _____, _____, _____,   ZOOM_RESET
),

/* MIT Layout (ADJUST)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |      |  F1  |  F2  |  F3  |  F4  |  F5  |                                      |TabUp | Back |C+A_Up|Frward|SclLok|LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |      |      |  F9  |  F10 | F11  |  F12 |                                      |TabDn | Home |  ↑  |  End |NumLck| Bksp |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CAPSLK|******|Alt,F5|Sft,F6|Cmd,F7|Ctl,F8|--------.                     .-------|BackApp|  ←  |  ↓  |  →  |APP SWITCH|APPBACK|
 * |------+------+------+------+------+------| Program|                     |Back Tab|-----+------+------+------+------+------|
 * |      |      |  F1  |  F2  |  F3  |  F4  |Switcher|-->  Press For       |SWITCH |TAB SW|PageUp|C+A_Dn|PageDn|Forwrd|INSERT|
 * .-----------------------------------------|--------|                     |-------|-----------------------------------------'
 *                      | ALT | OS   | ***** / Tab    /                     \  Repeat\  *****|***** | Cursor Left/Right |
 *                      | APP | ENT  | ***** / Switcher /                     \  key  \*****|***** | DIAL2 |
 *                      `--------------------------'                           '----------------------------'
 */
  [_ADJUST] = LAYOUT(
  _____,   KC_F1, KC_F2,    KC_F3,     KC_F4,    KC_F5,                                C(KC_PGUP), BWSRLEFT, CRSR_UP, BWSRRHGT, KC_SCRL,  LLOCK,
  _____,   _____, KC_F9,    KC_F10,    KC_F11,   KC_F12,                               C(KC_PGDN), KC_HOME,  KC_UP,   KC_END,   KC_NUM,   KC_BSPC,
  KC_CAPS, _____, MTALT_F5, MTLSFT_F6, MTGUI_F7, MTCTL_F8,                             CMD_S_TAB,  KC_LEFT,  KC_DOWN, KC_RGHT,  CMD_TAB,  CMD_S_TAB,
  _____,   _____, KC_F1,    KC_F2,     KC_F3,    KC_F4,    _____,          CTRL_S_TAB, CTRL_TAB,   KC_PGUP,  CRSR_DN, KC_PGDN,  BWSRRHGT, KC_INS,
                            _____,     OS_ENT, _____, CTRL_TAB,              _____,   _____,      _____,    _____
)
};
// clang-format on
static fast_timer_t       last_encoding_time = 0;
static const fast_timer_t ENCODER_DEBOUNCE   = 50;

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (TIMER_DIFF_FAST(timer_read_fast(), last_encoding_time) >= ENCODER_DEBOUNCE) {
        last_encoding_time = timer_read_fast();
    } else {
        return false;
    }

    switch (biton32(layer_state)) {
        case _LOWER:
            if (index == 0) { // LEFT ENCODER at LOWER layer
                uint8_t mod_state = get_mods();
                if (mod_state & MOD_MASK_SHIFT) {
                    unregister_mods(mod_state);
                    wait_ms(100);
                    if (clockwise) {
                        tap_code16(LALT(LCTL(KC_COMM)));
                    } else {
                        tap_code16(LALT(LCTL(KC_DOT)));
                    }
                    register_mods(mod_state);
                } else {
                    unregister_mods(mod_state);
                    if (clockwise) {
                        tap_code(KC_BRIGHTNESS_DOWN);
                    } else {
                        tap_code(KC_BRIGHTNESS_UP);
                    }
                    register_mods(mod_state);
                }
            } else if (index == 1) { // RIGHT ENCODER at LOWER layer
                if (is_mac()) {
                    tap_code16(clockwise ? LGUI(LSFT(KC_Z)) : LGUI(KC_Z)); // redo / undo
                } else {
                    tap_code16(clockwise ? LCTL(KC_Y) : LCTL(KC_Z)); // redo / undo
                }
            }
            break;

        case _ADJUST:
            if (index == 0) { // LEFT ENCODER at ADJUST layer
                if (clockwise) {
                    tap_code16(CTRL_S_TAB);
                } else {
                    tap_code16(CTRL_TAB);
                }
            } else if (index == 1) { // RIGHT ENCODER at ADJUST layer
                if (is_mac()) {
                    tap_code16(clockwise ? LGUI(LCTL(LSFT(KC_LEFT))) : LGUI(LCTL(LSFT(KC_RGHT)))); // expand selection in vsCode
                } else {
                    tap_code16(clockwise ? LALT(LGUI(LSFT(KC_LEFT))) : LALT(LGUI(LSFT(KC_RGHT))));
                }
            }
            break;

        case _RAISE:
            if (index == 0) { // Left encoder at RAISE layer
                // unbound
            } else if (index == 1) { // Right encoder at RAISE layer
                if (is_mac()) {
                    tap_code16(clockwise ? LGUI(KC_PPLS) : LGUI(KC_PMNS)); // zoom in and out
                } else {
                    tap_code16(clockwise ? LCTL(KC_PPLS) : LCTL(KC_PMNS));
                }
            }
            break;

        default:
            if (index == 0) { // Default handling for LEFT ENCODER - Super App Tab
                if (!clockwise) {
                    super_app_tab_forward();   // Scroll forward through applications
                } else {
                    super_app_tab_backward();  // Scroll backward through applications
                }
            } else if (index == 1) { // Default handling for RIGHT ENCODER
                tap_code16(clockwise ? KC_VOLU : KC_VOLD);
            }
            break;
    }
    return false;
}
#endif

bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
  if (keycode == MT_REP) { return false; }
  return true;
}

// Simple repeat key implementation
void repeat_key_invoke(keyevent_t* event) {
    if (last_keycode != KC_NO) {
        // Create a new record with the last keycode but current event
        keyrecord_t repeat_record = last_record;
        repeat_record.event = *event;
        process_record(&repeat_record);
    }
}

// Modifier held for the encoder app switcher. register_code does not apply
// CG swap, so this is the real Cmd (Mac) or Alt (Windows) key.
static uint8_t super_app_mod = KC_NO;

// register_code / tap_code16 skip mod_config, so these chords are the keys
// the host should see. CG swap does not rewrite them.
static void tap_mod_key(uint8_t modifier, uint8_t key) {
    register_code(modifier);
    register_code(key);
    unregister_code(modifier);
    unregister_code(key);
}

static void tap_primary(uint8_t key) {
    tap_mod_key(is_mac() ? KC_LGUI : KC_LCTL, key);
}

void eeconfig_init_user(void) {
#if (EECONFIG_USER_DATA_SIZE) == 0
    eeconfig_update_user(0);
#endif
    // QMK just wrote the default keymap word (oneshot + autocorrect, swap off).
    // Windows is the default machine, so turn Ctrl/GUI swap on before magic()
    // reloads keymap_config from EEPROM. CG_NORM / CG_SWAP keep writing it.
    keymap_config.raw            = eeconfig_read_keymap();
    keymap_config.swap_lctl_lgui = true;
    keymap_config.swap_rctl_rgui = true;
    eeconfig_update_keymap(keymap_config.raw);
}

// Super app tab functions
void super_app_tab_start(void) {
    if (!super_app_tabbing) {
        super_app_mod = is_mac() ? KC_LGUI : KC_LALT;
        register_code(super_app_mod);
        super_app_tabbing = true;
    }
    super_app_timer = timer_read_fast();  // Reset/start timer
}

void super_app_tab_forward(void) {
    super_app_tab_start();
    tap_code(KC_TAB);  // Tab forward through apps
}

void super_app_tab_backward(void) {
    super_app_tab_start();
    tap_code16(LSFT(KC_TAB));  // Shift+Tab backward through apps
}

void super_app_tab_stop(void) {
    if (super_app_tabbing) {
        unregister_code(super_app_mod);
        super_app_tabbing = false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef OLED_ENABLE
    navi_process_record(keycode, record);
#endif
#ifdef CONVERT_TO_KB2040
    if (!process_autocorrection(keycode, record)) {
        return false;
    }
#endif
    if (!process_layer_lock(keycode, record, LLOCK)) {
        return false;
    }
    if (!process_select_word(keycode, record, SELWORD)) {
        return false;
    }
    if (!process_adaptive_key(keycode, record)) {
        return false;
    }

    // Remember the last key for repeat functionality (exclude MT_REP itself)
    if (record->event.pressed && keycode != MT_REP && keycode >= KC_A && keycode <= KC_Z) {
        last_keycode = keycode;
        last_record = *record;
    }

    // Stop super app tab on any key press (except volume and encoder-related keys)
    if (record->event.pressed && super_app_tabbing) {
        if (keycode != KC_VOLU && keycode != KC_VOLD && keycode != KC_MPLY) {
            super_app_tab_stop();
        }
    }

    const uint8_t mods         = get_mods();
    const uint8_t oneshot_mods = get_oneshot_mods();

    switch (keycode) {
        case MT_REP: // Repeat the last key, letting it be used with a modifier
        if (record->tap.count) {  // On tap.
          // For Alternate Repeat Key, replace the next line with
          // alt_repeat_key_invoke(&record->event);
          repeat_key_invoke(&record->event);  // Repeat the last key.
          return false;  // Skip default handling.
        }
        return true;  // Continue default handling.

        // Layer switching with feedback
        case COLEMAKDH:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_COLEMAKDH);
                SEND_STRING("Colemak-DH");
            }
            return false;
            break;
        case QWERTY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTY);
                SEND_STRING("QWERTY");
            }
            return false;
            break;
        case CANARY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_CANARY);
                SEND_STRING("Canary");
            }
            return false;
            break;

        case SELPART: // Selects the next part of the word in vsCode
            if (record->event.pressed) {
                clear_mods(); // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                    tap_code16(LALT(LCTL(LSFT(KC_LEFT))));
                } else {
                    tap_code16(LALT(LCTL(LSFT(KC_RGHT))));
                }
                set_mods(mods); // Restore mods.
            }
            return false;
            break;
        case BRACES: // Types (), or {}, and puts cursor between braces.
            if (record->event.pressed) {
                clear_mods(); // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                    SEND_STRING("<>");
                } else {
                    SEND_STRING("()");
                }
                tap_code(KC_LEFT); // Move cursor between braces.
                set_mods(mods);    // Restore mods.
            }
            return false;
            break;
        case BRACES2: // Types [], or <>, and puts cursor between braces.
            if (record->event.pressed) {
                clear_mods(); // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                    SEND_STRING("{}");
                } else {
                    SEND_STRING("[]");
                }
                tap_code(KC_LEFT); // Move cursor between braces.
                set_mods(mods);    // Restore mods.
            }
            return false;
            break;
        case ARROW: // Arrow macro, types -> or =>.
            if (record->event.pressed) {
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) { // Is shift held?
                    del_mods(MOD_MASK_SHIFT);                 // Temporarily delete shift.
                    del_oneshot_mods(MOD_MASK_SHIFT);
                    SEND_STRING("const yourFuncVariable = (x,y) => x + y.method;");
                    for (int counter = 0; counter <= 24; counter++) {
                        tap_code(KC_LEFT);
                    }
                    tap_code16(is_mac() ? LALT(LSFT(KC_LEFT)) : LCTL(LSFT(KC_LEFT)));
                    set_mods(mods); // Restore mods.
                } else {
                    SEND_STRING("() => ");
                    tap_code(KC_LEFT);
                    tap_code(KC_LEFT);
                    tap_code(KC_LEFT);
                    tap_code(KC_LEFT);
                    tap_code(KC_LEFT);
                    set_mods(mods); // Restore mods.
                }
            }
            return false;
            break;
        case CTRL_TAB:
            if (record->event.pressed) {
                ctrl_tabtimer = timer_read_fast();
                if (!ctrl_tabbing) {
                    register_code(KC_LCTL);
                    ctrl_tabbing = true;
                }
                tap_code(KC_TAB);
            }
            return false;
            break;
        case CTRL_S_TAB:
            if (record->event.pressed) {
                ctrl_tabtimer = timer_read_fast();
                if (!ctrl_tabbing) {
                    register_code(KC_LCTL);
                    ctrl_tabbing = true;
                }
                tap_code16(LSFT(KC_TAB));
            }
            return false;
            break;
        case CMD_TAB:
            if (record->event.pressed) {
                tabtimer = timer_read_fast();
                if (!tabbing) {
                    tab_hold_mod = is_mac() ? KC_LGUI : KC_LALT;
                    register_code(tab_hold_mod);
                    tabbing = true;
                }
                tap_code(KC_TAB);
            }
            return false;
            break;
        case CMD_S_TAB:
            if (record->event.pressed) {
                tabtimer = timer_read_fast();
                if (!tabbing) {
                    tab_hold_mod = is_mac() ? KC_LGUI : KC_LALT;
                    register_code(tab_hold_mod);
                    tabbing = true;
                }
                tap_code16(LSFT(KC_TAB));
            }
            return false;
            break;
        case MTCTL_OSS: // MT(MOD_LCTL, KC_F24). Tap is one-shot shift; hold follows CG swap.
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    set_oneshot_mods(MOD_LSFT);
                }
                return false;
            }
            break;
        case ZOOM_RESET:
            if (record->event.pressed) {
                tap_code16(is_mac() ? LGUI(KC_0) : LCTL(KC_0));
            }
            return false;
            break;
        case BWSRLEFT:
            if (record->event.pressed) {
                tap_code16(is_mac() ? LGUI(KC_LEFT) : LALT(KC_LEFT));
            }
            return false;
            break;
        case BWSRRHGT:
            if (record->event.pressed) {
                tap_code16(is_mac() ? LGUI(KC_RGHT) : LALT(KC_RGHT));
            }
            return false;
            break;
        case CRSR_UP:
            if (record->event.pressed) {
                tap_code16(is_mac() ? LGUI(LALT(KC_UP)) : LCTL(LALT(KC_UP)));
            }
            return false;
            break;
        case CRSR_DN:
            if (record->event.pressed) {
                tap_code16(is_mac() ? LGUI(LALT(KC_DOWN)) : LCTL(LALT(KC_DOWN)));
            }
            return false;
            break;
        case OS_LBRC: // Mac: Cmd+[ . Windows: Ctrl+[ . tap_primary skips mod_config.
            if (record->event.pressed) {
                tap_primary(KC_LBRC);
            }
            return false;
            break;
        case OS_RBRC:
            if (record->event.pressed) {
                tap_primary(KC_RBRC);
            }
            return false;
            break;
        case OS_ENT:
            if (record->event.pressed) {
                tap_primary(KC_ENT);
            }
            return false;
            break;
        case HYPERESC:
            // mod_config swaps LCTL and LGUI with if/else-if. Both bits are
            // set here, so the GUI test clears GUI and the Ctrl test never
            // runs. Register the four mods directly. false skips that action.
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    register_code(KC_ESC);
                } else {
                    unregister_code(KC_ESC);
                }
            } else if (record->event.pressed) {
                register_mods(MOD_HYPR);
            } else {
                unregister_mods(MOD_HYPR);
            }
            return false;
            break;
    }
    return true;
};

enum combo_events {ARROW_FUNCTION, CONSOLE_LOG, HTML_P, HTML_TITLE, HTML_DIV, HTML_HTML, HTML_HEAD, HTML_BODY, HTML_FOOTER, HTML_A_HREF, HTML_IMG, CSS_STYLE, HTML_GENERIC_TAG, UNDO, REDO, CUT, COPY, PASTE, PASTECLIPBOARD, PASTETEXT, SELECTALL, QUESTIONMARK, EXCLAMATIONMARK, DQUOTE, UNDERSCORE, DASH, EQUALSIGN, ATSYMB, QUOTES, LOWERTOGGLE, SLEEP, RESETKEY, NUMLOCKC, F12COMBO, F11COMBO, OPENTERMINAL, NEXTTERMINAL, PREVTERMINAL, NEWTERMINAL, CAPSWORD, COMBO_LENGTH };

// Combos
uint16_t COMBO_LEN = COMBO_LENGTH; // remove the COMBO_COUNT define and use this instead


const uint16_t PROGMEM arrow_function_combo[]   = {FN_A, KC_DOT, COMBO_END};
const uint16_t PROGMEM console_log_combo[]      = {KC_H, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_p_combo[]           = {KC_P, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_title_combo[]       = {KC_T, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_div_combo[]         = {KC_D, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_html_combo[]        = {KC_Q, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_head_combo[]        = {KC_W, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_body_combo[]        = {KC_R, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_footer_combo[]      = {KC_X, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_a_href_combo[]      = {KC_F, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_img_combo[]         = {KC_I, KC_DOT, COMBO_END};
const uint16_t PROGMEM css_style_combo[]        = {KC_S, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_generic_tag_combo[] = {KC_G, KC_DOT, COMBO_END};
const uint16_t PROGMEM undo_combo[]             = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM redo_combo[]             = {KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM cut_combo[]              = {MTLALT_Z, KC_X, COMBO_END};
const uint16_t PROGMEM copy_combo[]             = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM paste_combo[]            = {KC_C, KC_D, COMBO_END};  // C+D positions from Colemak-DH layer
const uint16_t PROGMEM pasteclip_combo[]        = {KC_X, KC_D, COMBO_END};
const uint16_t PROGMEM pastetxt_combo[]         = {KC_X, KC_V, COMBO_END};
const uint16_t PROGMEM selectall_combo[]        = {MTLALT_Z, KC_D, COMBO_END};
const uint16_t PROGMEM questionmark_combo[]     = {KC_DOT, ALT_SLSH, COMBO_END};
const uint16_t PROGMEM exclamationmark_combo[]  = {ALT_SLSH, MTRSFTBSLS, COMBO_END};
const uint16_t PROGMEM dquote_combo[]           = {KC_O, MTRCTLQUO, COMBO_END};
const uint16_t PROGMEM underscore_combo[]       = {KC_COMMA, KC_DOT, COMBO_END};
const uint16_t PROGMEM dash_combo[]             = {KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM equalsign_combo[]        = {KC_E, KC_I, COMBO_END};
const uint16_t PROGMEM atsymb_combo[]           = {KC_T, KC_G, COMBO_END};
const uint16_t PROGMEM QUOTES_combo[]           = {KC_H, KC_COMMA, COMBO_END};
const uint16_t PROGMEM lowertoggle_combo[]      = {RSE_SPC, MTCMD_ENT, COMBO_END};
const uint16_t PROGMEM sleep_combo[]            = {KC_F7, KC_F8, KC_F9, KC_F10, COMBO_END};
const uint16_t PROGMEM reset_combo[]            = {KC_BSPC, MTRCTLQUO, MTRSFTBSLS, COMBO_END};
const uint16_t PROGMEM numlock_combo[]          = {KC_L, KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM f12_combo[]              = {KC_F1, KC_F2, COMBO_END};
const uint16_t PROGMEM f11_combo[]              = {KC_F9, KC_F10, COMBO_END};
const uint16_t PROGMEM capsword_combo[]         = {KC_LSFT, MTRSFTBSLS, COMBO_END};
const uint16_t PROGMEM openterminal_combo[]     = {KC_8, KC_9, COMBO_END};
const uint16_t PROGMEM nextterminal_combo[]     = {KC_7, KC_8, COMBO_END};
const uint16_t PROGMEM prevterminal_combo[]     = {KC_9, KC_0, COMBO_END};
const uint16_t PROGMEM newterminal_combo[]      = {KC_7, KC_8, KC_9, COMBO_END};

// const uint8_t combo_mods = get_mods();
// const uint8_t combo_oneshot_mods = get_oneshot_mods();

combo_t key_combos[] = {[ARROW_FUNCTION]    = COMBO_ACTION(arrow_function_combo),
                        [CONSOLE_LOG]       = COMBO_ACTION(console_log_combo),
                        [HTML_P]            = COMBO_ACTION(html_p_combo),
                        [HTML_TITLE]        = COMBO_ACTION(html_title_combo),
                        [HTML_DIV]          = COMBO_ACTION(html_div_combo),
                        [HTML_HTML]         = COMBO_ACTION(html_html_combo),
                        [HTML_HEAD]         = COMBO_ACTION(html_head_combo),
                        [HTML_BODY]         = COMBO_ACTION(html_body_combo),
                        [HTML_FOOTER]       = COMBO_ACTION(html_footer_combo),
                        [HTML_A_HREF]       = COMBO_ACTION(html_a_href_combo),
                        [HTML_IMG]          = COMBO_ACTION(html_img_combo),
                        [CSS_STYLE]         = COMBO_ACTION(css_style_combo),
                        [HTML_GENERIC_TAG]  = COMBO_ACTION(html_generic_tag_combo),
                        [UNDO]              = COMBO_ACTION(undo_combo),
                        [REDO]              = COMBO_ACTION(redo_combo),
                        [CUT]               = COMBO_ACTION(cut_combo),
                        [COPY]              = COMBO_ACTION(copy_combo),
                        [PASTE]             = COMBO_ACTION(paste_combo),
                        [PASTECLIPBOARD]    = COMBO_ACTION(pasteclip_combo),
                        [PASTETEXT]         = COMBO_ACTION(pastetxt_combo),
                        [SELECTALL]         = COMBO_ACTION(selectall_combo),
                        [QUESTIONMARK]      = COMBO_ACTION(questionmark_combo),
                        [EXCLAMATIONMARK]   = COMBO_ACTION(exclamationmark_combo),
                        [DQUOTE]            = COMBO_ACTION(dquote_combo),
                        [UNDERSCORE]        = COMBO_ACTION(underscore_combo),
                        [DASH]              = COMBO_ACTION(dash_combo),
                        [EQUALSIGN]         = COMBO_ACTION(equalsign_combo),
                        [ATSYMB]            = COMBO_ACTION(atsymb_combo),
                        [QUOTES]            = COMBO_ACTION(QUOTES_combo),
                        [LOWERTOGGLE]       = COMBO_ACTION(lowertoggle_combo),
                        [SLEEP]             = COMBO_ACTION(sleep_combo),
                        [RESETKEY]          = COMBO_ACTION(reset_combo),
                        [NUMLOCKC]          = COMBO_ACTION(numlock_combo),
                        [F12COMBO]          = COMBO_ACTION(f12_combo),
                        [F11COMBO]          = COMBO_ACTION(f11_combo),
                        [OPENTERMINAL]      = COMBO_ACTION(openterminal_combo),
                        [NEXTTERMINAL]      = COMBO_ACTION(nextterminal_combo),
                        [PREVTERMINAL]      = COMBO_ACTION(prevterminal_combo),
                        [NEWTERMINAL]       = COMBO_ACTION(newterminal_combo),
                        [CAPSWORD]          = COMBO_ACTION(capsword_combo)};
/* COMBO_ACTION(x) is same as COMBO(x, XXXXX) */

// Combo reference layers - all layers reference layer 0 (Colemak-DH) for combo positions
uint8_t combo_ref_from_layer(uint8_t layer) {
    switch (get_highest_layer(layer_state)) {
        case _QWERTY: return _COLEMAKDH;  // QWERTY uses Colemak-DH combo positions
        case _CANARY: return _COLEMAKDH;  // Canary uses Colemak-DH combo positions
        default: return _COLEMAKDH;       // Default to Colemak-DH positions
    }
}

// Tap Dance definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_LSFT_LPRN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lsft_lprn_finished, lsft_lprn_reset),
    [TD_LGUI_RPRN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lgui_rprn_finished, lgui_rprn_reset),
};

// Tap Dance function implementations
void lsft_lprn_finished(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            // Hold: Left Shift
            register_code(KC_LSFT);
        } else {
            // Tap: (
            register_code(KC_LSFT);
            register_code(KC_9);
            unregister_code(KC_9);
            unregister_code(KC_LSFT);
        }
    }
}

void lsft_lprn_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            // Release Left Shift
            unregister_code(KC_LSFT);
        }
    }
}

// Tap dance skips mod_config. Remember the swapped code so release matches.
static uint8_t rprn_hold_mod = KC_NO;

void lgui_rprn_finished(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            // Hold: Cmd on Mac, Ctrl on Windows (same swap as a GUI mod-tap).
            rprn_hold_mod = (uint8_t)keycode_config(KC_LGUI);
            register_code(rprn_hold_mod);
        } else {
            // Tap: )
            register_code(KC_LSFT);
            register_code(KC_0);
            unregister_code(KC_0);
            unregister_code(KC_LSFT);
        }
    }
}

void lgui_rprn_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            unregister_code(rprn_hold_mod);
        }
    }
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    const uint8_t mods         = get_mods();
    const uint8_t oneshot_mods = get_oneshot_mods();

    switch (combo_index) {
        case ARROW_FUNCTION:
            if (pressed) {
                SEND_STRING("() => {}");
                for (int i = 0; i < 8; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case CONSOLE_LOG:
            if (pressed) {
                SEND_STRING("console.log('')");
                for (int i = 0; i < 8; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_DIV:
            if (pressed) {
                SEND_STRING("<div></div>");
                for (int i = 0; i < 6; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_P:
            if (pressed) {
                SEND_STRING("<p></p>");
                for (int i = 0; i < 4; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_TITLE:
            if (pressed) {
                SEND_STRING("<title></title>");
                for (int i = 0; i < 8; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case CSS_STYLE:
            if (pressed) {
                SEND_STRING("<style></style>");
                for (int i = 0; i < 8; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_HTML:
            if (pressed) {
                SEND_STRING("<html lang=\"en\"></html>");
                for (int i = 0; i < 7; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_HEAD:
            if (pressed) {
                SEND_STRING("<head></head>");
                for (int i = 0; i < 7; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_BODY:
            if (pressed) {
                SEND_STRING("<body></body>");
                for (int i = 0; i < 7; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_FOOTER:
            if (pressed) {
                SEND_STRING("<footer></footer>");
                for (int i = 0; i < 9; i++) {
                    tap_code16(KC_LEFT);
                }
            }
            break;
        case HTML_A_HREF:
            if (pressed) {
                SEND_STRING("<a href=\"link_goes_here\">name_of_link_goes_here</a>");
                tap_code16(KC_HOME);
                for (int i = 0; i < 10; i++) {
                    tap_code16(KC_RGHT);
                }
            }
            break;
        case HTML_IMG:
            if (pressed) {
                SEND_STRING("<img src=\"image_source_or_link_goes_here\" alt=\"name_if_cant_load\" width=\"num_pixels\" height=\"num_pixels\">");
                tap_code16(KC_HOME);
                for (int i = 0; i < 11; i++) {
                    tap_code16(KC_RGHT);
                }
            }
            break;
        case HTML_GENERIC_TAG:
            if (pressed) {
                SEND_STRING("<TAG></TAG>");
                tap_code16(KC_ESC);
                for (int i = 0; i < 9; i++) {
                    tap_code16(KC_LEFT);
                }

                tap_code16(is_mac() ? LGUI(KC_D) : LCTL(KC_D));
                tap_code16(is_mac() ? LGUI(KC_D) : LCTL(KC_D));
                tap_code16(KC_BSPC);
            }
            break;
        case UNDO:
            if (pressed) {
                tap_primary(KC_Z);
            }
            break;
        case REDO:
            if (pressed) {
                if (is_mac()) {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_Z);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_Z);
                } else {
                    tap_primary(KC_Y);
                }
            }
            break;
        case CUT:
            if (pressed) {
                tap_primary(KC_X);
            }
            break;
        case COPY:
            if (pressed) {
                tap_primary(KC_C);
            }
            break;
        case PASTE:
            if (pressed) {
                tap_primary(KC_V);
            }
            break;
        case PASTECLIPBOARD:
            if (pressed) {
                if (is_mac()) {
                    register_code(KC_LALT);
                    register_code(KC_LSFT);
                    register_code(KC_LGUI);
                    register_code(KC_SPC);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_SPC);
                } else {
                    tap_mod_key(KC_LGUI, KC_V); // Win+V clipboard history
                }
            }
            break;
        case PASTETEXT:
            if (pressed) {
                if (is_mac()) {
                    register_code(KC_LSFT);
                    register_code(KC_LGUI);
                    register_code(KC_V);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_V);
                } else {
                    register_code(KC_LSFT);
                    register_code(KC_LCTL);
                    register_code(KC_V);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                    unregister_code(KC_V);
                }
                tap_code16(C(S(KC_V)));
            }
            break;
        case SELECTALL:
            if (pressed) {
                tap_primary(KC_A);
            }
            break;
        case QUESTIONMARK:
            if (pressed) {
                tap_code16(KC_QUES);
            }
            break;
        case EXCLAMATIONMARK:
            if (pressed) {
                tap_code16(KC_EXLM);
            }
            break;
        case DQUOTE:
            if (pressed) {
                tap_code16(KC_DQUO);
            }
            break;
        case UNDERSCORE:
            if (pressed) {
                tap_code16(KC_UNDS);
            }
            break;
        case DASH:
            if (pressed) {
                tap_code16(KC_PMNS);
            }
            break;
        case EQUALSIGN:
            if (pressed) {
                tap_code16(KC_EQL);
            }
            break;
        case ATSYMB:
            if (pressed) {
                tap_code16(KC_AT);
            }
            break;
        case QUOTES:
            if (pressed) {
                clear_mods(); // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                    SEND_STRING("'");
                } else {
                    SEND_STRING("\"");
                }
                set_mods(mods); // Restore mods.
            }
            break;
        case LOWERTOGGLE:
            if (pressed) {
                layer_invert(_LOWER);
                if (IS_LAYER_ON(_LOWER)) {
#ifdef AUDIO_ENABLE
                    PLAY_SONG(tone_startup);
#endif
                }
                if (IS_LAYER_OFF(_LOWER)) {
#ifdef AUDIO_ENABLE
                    PLAY_SONG(tone_goodbye);
#endif
                }
            }
            break;
        case SLEEP:
            if (pressed) {
                tap_code16(KC_SLEP);
            }
            break;
        case RESETKEY:
            if (pressed) {
                reset_keyboard();
            }
            break;
        case NUMLOCKC:
            if (pressed) {
                tap_code16(KC_NUM);
            }
            break;
        case F12COMBO:
            if (pressed) {
                tap_code16(KC_F12);
            }
            break;
        case F11COMBO:
            if (pressed) {
                tap_code16(KC_F11);
            }
            break;
        case OPENTERMINAL:
            if (pressed) {
                // Mac VS Code / the old Mac define: Ctrl+`. Windows define: Win+`.
                tap_code16(is_mac() ? LCTL(KC_GRV) : LGUI(KC_GRV));
            }
            break;
        case NEXTTERMINAL:
            if (pressed) {
                tap_code16(is_mac() ? LSFT(LGUI(KC_LBRC)) : LSFT(LCTL(KC_LBRC)));
            }
            break;
        case PREVTERMINAL:
            if (pressed) {
                tap_code16(is_mac() ? LSFT(LGUI(KC_RBRC)) : LSFT(LCTL(KC_RBRC)));
            }
            break;
        case NEWTERMINAL:
            if (pressed) {
                tap_code16(is_mac() ? LCTL(LSFT(KC_GRV)) : LGUI(LSFT(KC_GRV)));
            }
            break;
        case CAPSWORD:
            if (pressed) {
                caps_word_on();
            }
            break;
    }
}

