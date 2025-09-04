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
#include "guess_os.h"

// #include "features/caps_word.h"
// #include "features/autocorrection.h"

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
enum layer_names { _COLEMAKDH, _QWERTY, _CANARY, _LOWER, _RAISE, _ADJUST, _NAV };

#if (host_os == OS_MACOS || host_os == OS_IOS) // if the os is iOS or MacOS
#    define MAC_HOTKEYS
#endif

#define XXXXX KC_NO
#define _____ KC_TRNS
#define HYPERESC MT(MOD_LCTL | MOD_LGUI | MOD_LALT | MOD_LSFT, KC_ESC) // Hyper on hold, Esc on tap
#define RSE_SPC LT(_RAISE, KC_SPC)                                     // Raise on hold, One Shot Shift on tap
#define LOW_SPC LT(_LOWER, KC_SPC)                                     // lower on hold, One Shot Shift on tap
#define RSE_SPC LT(_RAISE, KC_SPC)                                     // lower on hold, One Shot Shift on tap
#define LOW_DEL LT(_LOWER, KC_DEL)                                     // raise on hold, Delete on tap
#define L_NAV_0 LT(_NAV, KC_0)                                  // Adjust Layer on hold, Dot on tap
#define L_NAV_SPC LT(_NAV, KC_SPC)                                  // Adjust Layer on hold, Space on tap
#define ALT_GRV MT(MOD_LALT, KC_GRV)                                   // alt on hold, grave ` on tap
#define MTLGUI_STAB MT(MOD_LGUI, LSFT(KC_TAB))                         // GUI on hold, Shift Tab on tap
#define MTRSFTBSLS MT(MOD_RSFT, KC_BSLS)                               // Shift on hold, \ on tap
#define MTRCTLQUO MT(MOD_RCTL, KC_QUOT)                                // Ctrl on hold, " on tap
#define CO_TAB MT(MOD_LCTL | MOD_LALT, KC_TAB)                        // Ctrl + Opt on hold, Tab on tap
#define FN_A LT(_ADJUST, KC_A)
#define FN_C LT(_ADJUST, KC_C)
#define FN_O LT(_ADJUST, KC_O)
#define FN_SCLN LT(_ADJUST, KC_SCLN)

#ifdef MAC_HOTKEYS
// Mac Specific Hotkeys
#    define TERMINAL LCTL(KC_GRV)
#    define DESKTR LCTL(KC_RGHT) // move one virtual desktop to the right
#    define DESKTL LCTL(KC_LEFT) // move one virtual desktop to the left
#    define MTCMD_ENT MT(MOD_LGUI, KC_ENT)
#    define MTCTL_OSS MT(MOD_LCTL, KC_F24)
#    define MTLALT_Z MT(MOD_LALT, KC_Z)
#    define MTLALT_W MT(MOD_LALT, KC_W)
#    define ALT_SLSH MT(MOD_LALT, KC_SLSH)
// modifier tap keys
#    define MTALT_TS MT(MOD_LALT, CTRL_TAB)   // Alt on hold, Ctrl + Tab on tap (TS == tab switcher)
#    define MTLALT_LRBC MT(MOD_LALT, KC_LBRC) // Alt on hold, [ on tap
#    define MTLSFT_LPRN TD(TD_LSFT_LPRN)      // Shift on hold, ( on tap (using tap dance)
#    define MTLGUI_RPRN TD(TD_LGUI_RPRN)      // GUI on hold, ) on tap (using tap dance)
#    define MTLCTL_RBRC MT(MOD_LCTL, KC_RBRC) // Ctrl on hold, ] on tap
#    define MT_REP MT(MOD_LCTL, KC_0)        // Ctrl on hold, Repeat key on tap
// #    define MT_REP MT(MOD_LCTL, QK_REP) // Ctrl on hold, Repeat key on tap // this doesn't work, because QK_REP is not a basic keycode
#    define MTALT_F5 MT(MOD_LALT, KC_F5)      // alt on hold, F5 on tap
#    define MTLSFT_F6 MT(MOD_LSFT, KC_F6)     // shift on hold, F6 on tap
#    define MTGUI_F7 MT(MOD_LGUI, KC_F7)      // gui on hold, F7 on tap
#    define MTCTL_F8 MT(MOD_LCTL, KC_F8)      // ctrl on hold, F8 on tap
#    define MTALT_RHGT MT(MOD_LALT, KC_RGHT)  // alt on hold, mouse right on tap
#    define MTLSFT_DN MT(MOD_LSFT, KC_DOWN)   // shift on hold, down arrow on tap
#    define MTGUI_LEFT MT(MOD_LGUI, KC_LEFT)  // gui on hold, left arrow on tap
#    define MTCTL_RCLCK MT(MOD_LCTL, KC_M)    // ctrl on hold, right arrow on tap
#    define FIND_F3 KC_F3                     // search on page
#    define BWSRLEFT LGUI(KC_LEFT)
#    define BWSRRHGT LGUI(KC_RGHT)
#    define CRSR_UP LGUI(LALT(KC_UP))   // add cursor above
#    define CRSR_DN LGUI(LALT(KC_DOWN)) // add cursor below
#    define SEARCH LGUI(KC_F)           // search on page
#    define ZOOM_RESET LGUI(KC_0)       // reset zoom

#else
// Windows Specific Hotkeys
#    define TERMINAL LGUI(KC_GRV)
#    define DESKTR LGUI(LCTL(KC_RGHT))        // move one virtual desktop to the right
#    define DESKTL LGUI(LCTL(KC_LEFT))        // move one virtual desktop to the left
#    define MTLALT_LRBC MT(MOD_LCTL, KC_LBRC) // Ctrl (for windows) on hold, [ on tap
#    define MTLSFT_LPRN MT(MOD_LSFT, KC_LPRN) // Shift on hold, ( on tap
#    define MTLGUI_RPRN MT(MOD_LALT, KC_RPRN) // Alt (for windows) on hold, ) on tap
#    define MTLCTL_RBRC MT(MOD_LGUI, KC_RBRC) // Win/GUI (for windows) on hold, ] on tap
#    define MT_REP MT(MOD_LGUI, QK_REP)  // Win/GUI (for windows) on hold, Repeat key on tap
#    define MTLALT_Z MT(MOD_LGUI, KC_Z)
#    define MTALT_TS MT(MOD_LALT, CTRL_TAB) // Alt on hold,  Ctrl + Tab on tap (TS == tab switcher)
#    define MTCMD_ENT MT(MOD_LCTL, KC_ENT)
#    define MTCTL_OSS MT(MOD_LGUI, KC_F24)
#    define FIND_F3 KC_F3 // search on page
#    define BWSRLEFT A(KC_LEFT)
#    define BWSRRHGT A(KC_RGHT)
#    define CRSR_UP LCTL(LALT(KC_UP))   // add cursor above
#    define CRSR_DN LCTL(LALT(KC_DOWN)) // add cursor below
#    define SEARCH LCTL(KC_F)           // search on page
#    define ZOOM_RESET LCTL(KC_0)       // reset zoom

#endif // MAC_HOTKEYS

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
};
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* MIT Layout (COLEMAKH-DH)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |NON-US#| F1 -F12- F2 |  F3  |  F4  |  F5  |                                      |  F6  |  F7  |  F8  |  F9  |  F10  |QWERTY|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC| Q   |  W   |  F   |  P   |  B   |                                      |  J   |  L   |  U   |  Y   |  ;   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CO_TAB| FN_A|  R   |  S   |  T   |  G   |-------.                      .-------|  M   |  N   |  E   |  I   |  O   |CTRL,'|
 * |------+------+------+------+------+------|Select |                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| ALT_Z|  X   |  C   |  D   |  V   |Words l/r|-->Toggle Microphone| GRV`  |  K   |  H   |  ,   |  .   |ALT,/ |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                       \Repeat   \ RAISE | LOW   |Volume Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                          \Key      \ SPACE| DEL   |DIAL2 | -> TOGGLE play/pause On Press
 *                      `-------------------------'                            '--------------------------'
 */
  [_COLEMAKDH] = LAYOUT(
  KC_NUHS,  KC_F1,    KC_F2,  KC_F3,     KC_F4,     KC_F5,                               KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,   QWERTY,
  HYPERESC, KC_Q,     KC_W,   KC_F,      KC_P,      KC_B,                                KC_J,    KC_L,    KC_U,    KC_Y,   KC_SCLN,  KC_BSPC,
  CO_TAB,   FN_A,     KC_R,   KC_S,      KC_T,      KC_G,                                KC_M,    KC_N,    KC_E,    KC_I,   KC_O,     MTRCTLQUO,
  KC_LSFT,  MTLALT_Z, KC_X,   KC_C,      KC_D,      KC_V,   C(A(G(KC_M))),     ALT_GRV, KC_K,    KC_H,    KC_COMM, KC_DOT, ALT_SLSH, MTRSFTBSLS,
                              MTALT_TS, MTCMD_ENT, LOW_SPC, MTCTL_OSS,         QK_REP,  RSE_SPC, LOW_DEL, KC_MPLY
),

/* MIT Layout (QWERTY)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |NON-US#| F1 -F12- F2 |  F3  |  F4  |  F5  |                                      |  F6  |  F7  |  F8  |  F9  |  F10  |CANARY|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC| Q   |  W   |  E   |  R   |  T   |                                      |  Y   |  U   |  I   |  O   |  P   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CO_TAB|FN_A |  S   |  D   |  F  -@-  G  |-------.                      .-------|  H   |  J   |  K   |  L   | FN_; |CTRL,'|
 * |------+------+------+------+------+------|Select |                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| ALT_Z|  X   |  C   |  V   |  B   |Words l/r|-->Toggle Microphone| GRV`  |  N   |  M   |  ,   |  .   |ALT,/ |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                      \Repeat \  RAISE |LOW   |Volume Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                         \Key    \ SPACE| DEL   |DIAL2 | -> TOGGLE play/pause On Press
 *                      `-------------------------'                           '-------------------------'
 */
  [_QWERTY] = LAYOUT(
  KC_NUHS,  KC_F1,    KC_F2,  KC_F3,     KC_F4,     KC_F5,                                KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  CANARY,
  HYPERESC, KC_Q,     KC_W,   KC_E,      KC_R,      KC_T,                                KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,     KC_BSPC,
  CO_TAB,   FN_A,     KC_S,   KC_D,      KC_F,      KC_G,                                KC_H,    KC_J,    KC_K,    KC_L,   KC_SCLN,  MTRCTLQUO,
  KC_LSFT,  MTLALT_Z, KC_X,   KC_C,      KC_V,      KC_B,   C(A(G(KC_M))),     ALT_GRV,  KC_N,    KC_M,    KC_COMM, KC_DOT, ALT_SLSH, MTRSFTBSLS,
                              MTALT_TS, MTCMD_ENT, LOW_SPC, MTCTL_OSS,         QK_REP,   RSE_SPC, LOW_DEL, KC_MPLY // Toggle microphone
),
/* MIT Layout (CANARY)
 * .-----------------------------------------.                                      .-----------------------------------------.          Slightly modified: Q and W swap
 * |NON-US#| F1 -F12- F2 |  F3  |  F4  |  F5  |                                     |  F6  |  F7  |  F8  |  F9  |  F10  |COLEMAKDH|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC|  Q   |  L   |  Y   |  P   |  B   |                                      |  Z   |  F   |  O   |  U   |  ;   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CO_TAB| FN_C |  R   |  S   |  T   |  G   |-------.                      .-------|  M   |  N   |  E   |  I   |  A   |CTRL,'|
 * |------+------+------+------+------+------|Select |                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| ALT_W|  J   |  V   |  D   |  K   |Words l/r|-->Toggle Microphone| GRV`  |  X   |  H   |  ,   |  .   |ALT,/ |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                      \Repeat \  RAISE | LOW  |Volume Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                         \Key    \ SPACE|  DEL  |DIAL2 | -> TOGGLE play/pause On Press
 *                      `-------------------------'                           '-------------------------'
 */
  [_CANARY] = LAYOUT(
  KC_NUHS,  KC_F1,    KC_F2,  KC_F3,     KC_F4,     KC_F5,                               KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  COLEMAKDH,
  HYPERESC, KC_Q,     KC_L,   KC_Y,      KC_P,      KC_B,                                KC_Z,    KC_F,    KC_O,    KC_U,   KC_SCLN,  KC_BSPC,
  CO_TAB,   FN_C,     KC_R,   KC_S,      KC_T,      KC_G,                                KC_M,    KC_N,    KC_E,    KC_I,   KC_A,     MTRCTLQUO,
  KC_LSFT,  MTLALT_W, KC_J,   KC_V,      KC_D,      KC_K,   C(A(G(KC_M))),     ALT_GRV,  KC_X,    KC_H,    KC_COMM, KC_DOT, ALT_SLSH, MTRSFTBSLS,
                              MTALT_TS, MTCMD_ENT, LOW_SPC, MTCTL_OSS,         QK_REP,   RSE_SPC, LOW_DEL, KC_MPLY // Toggle Microphone
),


/* MIT Layout (LOWER) // couldn't get mod-tap to work with LPRN and RPRN
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |NON-US#|  1  |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0   |LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
*  |  `   |  !   |  <   |  >   |  =   |  ^   |                                      |   *  |   7  |   8  |   9  |   :  | Bsp  |
 * |------+------+------+------+-------------|                                      |------+------+------+------+------+------|
 * |S(TAB)|  #   |  [Alt| ( Sft| ) Cmd| ] Ctl|-------.                      .-------|   -  |   4  |   5  |   6  |   +  |  &   |
 * |------+------+------+------+------+------|Brightness|Haze(shift)        |       |------+------+------+------+------+------|
 * |SHIFT |  @   |  $   |  {   |  }   |  %   | DIAL1 |--> Press for         |   ~   |   .  |   1  |   2  |   3  |   /   |  |  |
 * .-----------------------------------------|-------|   haze On/Off        |-------|-----------------------------------------'
 *                      | ALT | CMD  |++NAV++/ LCTRL/                        \  Alt \++NAV++| **** |ZOOM IN/OUT|
 *                      | APP | ENTER| ***** / OSSft/                         \Repeat\   0  |  .  | DIAL2|--> Next Song on Press
 *                      `-------------------------'                            '-------------------------'
 */
  [_LOWER] = LAYOUT(
  KC_NUHS,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                KC_6,     KC_7,   KC_8,  KC_9,  KC_0,    LLOCK,
  KC_TILD,   KC_EXLM, KC_LABK, KC_RABK, KC_EQL,  KC_CIRC,                             KC_ASTR,  KC_7,   KC_8,  KC_9,  KC_COLN, KC_BSPC,
  S(KC_TAB), KC_HASH, MTLALT_LRBC, MTLSFT_LPRN, MTLGUI_RPRN, MTLCTL_RBRC,             KC_PMNS,  KC_4,   KC_5,  KC_6,  KC_PPLS, KC_AMPR,
  KC_LSFT,   KC_AT,   KC_DLR,  KC_LCBR, KC_RCBR, KC_PERC, C(A(KC_Q)),        KC_TILD, KC_DOT,   KC_1,   KC_2,  KC_3,  KC_PSLS, KC_PIPE,
                               _____,   _____,   L_NAV_SPC,   _____,         QK_AREP, L_NAV_0,  KC_DOT, KC_MNXT // Next Song on press
),
/* MIT Layout (RAISE)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |CGNORM|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0   |LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CGSWAP|  Mb3 |  Mb2 | MsUp |  Mb1 |      |                                      | Mute |  Mb1 | Mb3 | Mb2  |  Mb3 | Bksp |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |      |      | MsLft|BRACES|BRACES2|      |-------.                      .-------| Vol+ |SELPART|SELWORD|CMD'['|CMD']'|****|
 * |------+------+------+------+------+------|Undo/Redo|                    |       |------+------+------+------+------+------|
 * |      |ScLeft| ScUp |ScDown|ScRight|     | DIAL1 |--> Press for         |       | Vol- |SELWORD|SELPART|ARROW|      |      |
 * .-----------------------------------------|-------|  Next Track          |-------|-----------------------------------------'
 *                      | ALT | CMD  |  ADJUST  / LCTRL/                     \Repeat\ ******|      |Zoom in/out|
 *                      | APP | ENTER| ***** / OSSft/                         \ key  \******|      | DIAL2|--> //RESET ZOOM on Press
 *                      `-------------------------'                            '--------------------------'
 */
  [_RAISE] = LAYOUT(
  CG_NORM, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                   KC_6,    KC_7,    KC_8,    KC_9,       KC_0,       LLOCK,
  CG_SWAP, KC_BTN3, KC_BTN2, KC_MS_U, KC_BTN1, _____,                                  KC_MUTE, KC_BTN1, KC_BTN3, KC_BTN2,    KC_BTN3,    KC_BSPC,
  _____,   _____,   KC_MS_L, BRACES, BRACES2, ARROW,                                   KC_VOLU, SELPART, SELWORD, G(KC_LBRC), G(KC_RBRC), _____,
  _____,   KC_MS_L, KC_MS_U, KC_MS_D, KC_MS_R, _____,   ZOOM_RESET,           _____,   KC_VOLD, SELWORD, SELPART,  ARROW,      _____,      _____,
                            _____, LGUI(KC_ENT), L_NAV_SPC, _____,            _____, _____, _____,   ZOOM_RESET
),

/* MIT Layout (ADJUST)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |      |  F1  |  F2  |  F3  |  F4  |  F5  |                                      |TabUp | Back |C+A_Up|Frward|SclLok|LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |      |      |  F9  |  F10 | F11  |  F12 |                                      |TabDn | Home |  ↑  |  End |NumLck| Bksp |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CAPSLK|******|Alt,F5|Sft,F6|Cmd,F7|Ctl,F8|--------.                     .-------|BackApp|  ←  |  ↓  |  →   |APPBack|APP SWITCH|
 * |------+------+------+------+------+------| Program|                     |Back Tab|-----+------+------+------+------+------|
 * |      |      |  F1  |  F2  |  F3  |  F4  |Switcher|-->  Press For       |SWITCH |TAB SW|PageUp|C+A_Dn|PageDn|Forwrd|INSERT|
 * .-----------------------------------------|--------|                     |-------|-----------------------------------------'
 *                      | ALT | CMD  | ***** / Tab    /                     \  Repeat\  *****|***** | Cursor Left/Right |
 *                      | APP | ENTER| ***** / Switcher /                     \  key  \*****|***** | DIAL2 |
 *                      `--------------------------'                           '----------------------------'
 */
  [_ADJUST] = LAYOUT(
  _____,   KC_F1, KC_F2,    KC_F3,     KC_F4,    KC_F5,                                C(KC_PGUP), BWSRLEFT, CRSR_UP, BWSRRHGT, KC_SCRL,  LLOCK,
  _____,   _____, KC_F9,    KC_F10,    KC_F11,   KC_F12,                               C(KC_PGDN), KC_HOME,  KC_UP,   KC_END,   KC_NUM,   KC_BSPC,
  KC_CAPS, _____, MTALT_F5, MTLSFT_F6, MTGUI_F7, MTCTL_F8,                             CMD_S_TAB,  KC_LEFT,  KC_DOWN, KC_RGHT,  CMD_S_TAB,  CMD_TAB,
  _____,   _____, KC_F1,    KC_F2,     KC_F3,    KC_F4,    _____,          CTRL_S_TAB, CTRL_TAB,   KC_PGUP,  CRSR_DN, KC_PGDN,  BWSRRHGT, KC_INS,
                            _____,     LGUI(KC_ENT), _____, CTRL_TAB,         _____,   _____,      _____,    _____
)

/* MIT Layout (NAV) new layer, currently copied from raise
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |CGNORM|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0   |LLOCK |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CGSWAP|  Mb3 |  Mb2 | MsUp |  Mb1 |      |                                      | Mute |  Mb1 | M Up | Mb2  |  Mb3 | Bksp |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |      |      | MsLft| MDown|Mright|      |-------.                      .-------| Vol+ |M Left| MDown|Mright |CMD']'|****|
 * |------+------+------+------+------+------|Undo/Redo|                    |       |------+------+------+------+------+------|
 * |      |ScLeft| ScUp |ScDown|ScRight|     | DIAL1 |--> Press for         |       | Vol- |BRACES|BRACES2|ARROW|      |      |
 * .-----------------------------------------|-------|  Next Track          |-------|-----------------------------------------'
 *                      | ALT | CMD  |  ADJUST  / LCTRL/                     \Repeat\       |******|Zoom in/out|
 *                      | APP | ENTER| ***** / OSSft/                         \ key  \      |******| DIAL2|--> //RESET ZOOM on Press
 *                      `-------------------------'                            '--------------------------'
 */
  [_NAV] = LAYOUT(
  CG_NORM, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                   KC_6,    KC_7,    KC_8,    KC_9,       KC_0,       LLOCK,
  CG_SWAP, KC_BTN3, KC_BTN2, KC_MS_U, KC_BTN1, _____,                                  KC_MUTE, KC_BTN1, KC_MS_U, KC_BTN2,    KC_BTN3,    KC_BSPC,
  _____,   _____,   KC_MS_L, KC_MS_D, KC_MS_R, _____,                                  KC_VOLU, KC_MS_L, KC_MS_D, KC_MS_R, G(KC_RBRC), _____,
  _____,   KC_MS_L, KC_MS_U, KC_MS_D, KC_MS_R, _____,   ZOOM_RESET,           _____,   KC_VOLD, BRACES2, BRACES,  ARROW,      _____,      _____,
                            _____, LGUI(KC_ENT), L_NAV_SPC, _____,            _____, _____, _____, ZOOM_RESET
),


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
#    ifdef MAC_HOTKEYS
                tap_code16(clockwise ? LGUI(LSFT(KC_Z)) : LGUI(KC_Z)); // undo and redo
#    else
                tap_code16(clockwise ? LCTL(KC_Y) : LCTL(KC_Z)); // windows undo and redo
#    endif
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
#    ifdef MAC_HOTKEYS
tap_code16(clockwise ? LGUI(LCTL(LSFT(KC_LEFT))) : LGUI(LCTL(LSFT(KC_RGHT)))); // expand selection in vsCode
#    else
tap_code16(clockwise ? LALT(LGUI(LSFT(KC_LEFT))) : LALT(LGUI(LSFT(KC_RGHT))));
#    endif

            }
            break;

        case _RAISE:
            if (index == 0) { // Left encoder at RAISE layer
#    ifdef MAC_HOTKEYS
                // can't think of anything cool to put here
#    else
                // can't think of anything cool to put here on windows
#    endif
            } else if (index == 1) { // Right encoder at RAISE layer
#    ifdef MAC_HOTKEYS
                tap_code16(clockwise ? LGUI(KC_PPLS) : LGUI(KC_PMNS)); // zoom in and out
#    else
                tap_code16(clockwise ? LCTL(KC_PPLS) : LCTL(KC_PMNS));
#    endif
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

// Super app tab functions
void super_app_tab_start(void) {
    if (!super_app_tabbing) {
#ifdef MAC_HOTKEYS
        register_code(KC_LGUI);  // Hold Cmd on Mac
#else
        register_code(KC_LALT);  // Hold Alt on Windows/Linux
#endif
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
#ifdef MAC_HOTKEYS
        unregister_code(KC_LGUI);  // Release Cmd
#else
        unregister_code(KC_LALT);  // Release Alt
#endif
        super_app_tabbing = false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
#ifdef MAC_HOTKEYS
                    tap_code16(LALT(LSFT(KC_LEFT)));
#else
                    tap_code16(LCTL(LSFT(KC_LEFT)));
#endif                              // MAC_HOTKEYS
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
#ifdef MAC_HOTKEYS
                    register_code(KC_LGUI);
#else
                    register_code(KC_LALT);
#endif // MAC_HOTKEYS
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
#ifdef MAC_HOTKEYS
                    register_code(KC_LGUI);
#else
                    register_code(KC_LALT);
#endif // MAC_HOTKEYS
                    tabbing = true;
                }
                tap_code16(LSFT(KC_TAB));
            }
            return false;
            break;
#ifdef MAC_HOTKEYS
        case MT(MOD_LCTL, KC_F24):
        if (record->tap.count > 0) {
            if (record->event.pressed) {
                set_oneshot_mods(MOD_LSFT);
            }
            return false;
        }
        break;
#else
        case MT(MOD_LGUI, KC_F24):
#endif // MAC_HOTKEYS
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    set_oneshot_mods(MOD_LSFT);
                }
                return false;
            }
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
                        [PASTE]             = COMBO_ACTION(paste_combo),//
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

void lgui_rprn_finished(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            // Hold: Left GUI
            register_code(KC_LGUI);
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
            // Release Left GUI
            unregister_code(KC_LGUI);
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

#ifdef MAC_HOTKEYS
                tap_code16(LGUI(KC_D));
                tap_code16(LGUI(KC_D));
#else
                tap_code16(LCTL(KC_D));
                tap_code16(LCTL(KC_D));
#endif // MAC_HOTKEYS
                tap_code16(KC_BSPC);
            }
            break;
        case UNDO:
            if (pressed) {
#ifdef MAC_HOTKEYS
                register_code(KC_LGUI);
                register_code(KC_Z);
                unregister_code(KC_LGUI);
                unregister_code(KC_Z);
#else
                register_code(KC_LCTL);
                register_code(KC_Z);
                unregister_code(KC_LCTL);
                unregister_code(KC_Z);
#endif
            }
            break;
        case REDO:
            if (pressed) {
#ifdef MAC_HOTKEYS
                register_code(KC_LGUI);
                register_code(KC_LSFT);
                register_code(KC_Z);
                unregister_code(KC_LGUI);
                unregister_code(KC_LSFT);
                unregister_code(KC_Z);
#else
                register_code(KC_LCTL);
                register_code(KC_Y);
                unregister_code(KC_LCTL);
                unregister_code(KC_Y);
#endif
            }
            break;
        case CUT:
            if (pressed) {
#ifdef MAC_HOTKEYS
                // register_code(KC_LGUI);
                // register_code(KC_X);
                // unregister_code(KC_LGUI);
                // unregister_code(KC_X);

                tap_code16(KC_CUT);
#else
                register_code(KC_LCTL);
                register_code(KC_X);
                unregister_code(KC_LCTL);
                unregister_code(KC_X);
#endif
            }
            break;
        case COPY:
            if (pressed) {
#ifdef MAC_HOTKEYS
                // register_code(KC_LGUI);
                // register_code(KC_C);
                // unregister_code(KC_LGUI);
                // unregister_code(KC_C);
                tap_code16(KC_COPY);
#else
                register_code(KC_LCTL);
                register_code(KC_C);
                unregister_code(KC_LCTL);
                unregister_code(KC_C);
#endif
            }
            break;
        case PASTE:
            if (pressed) {
#ifdef MAC_HOTKEYS
                // register_code(KC_LGUI);
                // register_code(KC_V);
                // unregister_code(KC_LGUI);
                // unregister_code(KC_V);
                // going to try the kc_paste instead and see if that works universally between all keylayouts on mac
                tap_code16(KC_PASTE);
#else
                register_code(KC_LCTL);
                register_code(KC_V);
                unregister_code(KC_LCTL);
                unregister_code(KC_V);
#endif
            }
            break;
        case PASTECLIPBOARD:
            if (pressed) {
#ifdef MAC_HOTKEYS
                register_code(KC_LALT);
                register_code(KC_LSFT);
                register_code(KC_LGUI);
                register_code(KC_SPC);
                unregister_code(KC_LALT);
                unregister_code(KC_LSFT);
                unregister_code(KC_LGUI);
                unregister_code(KC_SPC);
#else
                register_code(KC_LGUI);
                register_code(KC_V);
                unregister_code(KC_LGUI);
                unregister_code(KC_V);
#endif
            }
            break;
        case PASTETEXT:
            if (pressed) {
#ifdef MAC_HOTKEYS
                register_code(KC_LSFT);
                register_code(KC_LGUI);
                register_code(KC_V);
                unregister_code(KC_LSFT);
                unregister_code(KC_LGUI);
                unregister_code(KC_V);
#else
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_V);
                unregister_code(KC_LSFT);
                unregister_code(KC_LCTL);
                unregister_code(KC_V);
#endif
                tap_code16(C(S(KC_V)));
            }
            break;
        case SELECTALL:
            if (pressed) {
#ifdef MAC_HOTKEYS
                register_code(KC_LGUI);
                register_code(KC_A);
                unregister_code(KC_LGUI);
                unregister_code(KC_A);
#else
                register_code(KC_LCTL);
                register_code(KC_A);
                unregister_code(KC_LCTL);
                unregister_code(KC_A);
#endif
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
                tap_code16(LCTL(KC_GRV));
            }
            break;
        case NEXTTERMINAL:
            if (pressed) {
                tap_code16(LSFT(LGUI(KC_LBRC)));
            }
            break;
        case PREVTERMINAL:
            if (pressed) {
                tap_code16(LSFT(LGUI(KC_RBRC)));
            }
            break;
        case NEWTERMINAL:
            if (pressed) {
                tap_code16(LCTL(LSFT(KC_GRV)));
            }
            break;
        case CAPSWORD:
            if (pressed) {
                caps_word_on();
            }
            break;
    }
}
