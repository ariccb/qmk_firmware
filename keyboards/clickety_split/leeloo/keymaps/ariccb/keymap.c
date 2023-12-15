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

//#include "features/caps_word.h"
//#include "features/autocorrection.h"

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skiep them
// entirely and just use numbers.
enum layer_names {
    _COLEMAKDH,
    _QWERTY,
    _LOWER,
    _RAISE,
    _ADJUST
};

#if (host_os == OS_MACOS || host_os == OS_IOS)// if the os is iOS or MacOS
#define MAC_HOTKEYS
#endif

#define XXXXX KC_NO
#define _____ KC_TRNS
#define HYPERESC MT(MOD_LCTL | MOD_LGUI | MOD_LALT | MOD_LSFT, KC_ESC) // Hyper on hold, Esc on tap
#define LOW_SPC LT(_LOWER, KC_SPC)   // lower on hold, One Shot Shift on tap
#define RSE_DEL LT(_RAISE, KC_DEL)  // raise on hold, Delete on tap
#define ALT_GRV MT(MOD_LALT, KC_GRV) // alt on hold, grave ` on tap
#define MTLGUI_STAB MT(MOD_LGUI, LSFT(KC_TAB))  // GUI on hold, Shift Tab on tap
#define MTRSFTBSLS MT(MOD_RSFT, KC_BSLS) // Shift on hold, \ on tap
#define MTRCTLQUO MT(MOD_RCTL, KC_QUOT) // Ctrl on hold, " on tap
#define MEH_TAB  MT(MOD_LCTL | MOD_LGUI | MOD_LALT, KC_TAB) // MEH on hold, Tab on tap
#define FN_A LT(_ADJUST, KC_A)
#define FN_O LT(_ADJUST, KC_O)
#define FN_SCLN LT(_ADJUST, KC_SCLN)


#ifdef MAC_HOTKEYS
// Mac Specific Hotkeys
#define TERMINAL LCTL(KC_GRV)
#define DESKTR LCTL(KC_RGHT)  // move one virtual desktop to the right
#define DESKTL LCTL(KC_LEFT)  // move one virtual desktop to the left
#define MTCMD_ENT MT(MOD_LGUI, KC_ENT)
#define MTCTL_OSS MT(MOD_LCTL, KC_F24)
#define MTLALT_Z MT(MOD_LALT, KC_Z)
// modifier tap keys
#define MTALT_APP MT(MOD_LALT, KC_PMNS) // Alt on hold, Menu on tap
#define MTLALT_LRBC MT(MOD_LALT, KC_LBRC) // Alt on hold, [ on tap
#define MTLSFT_LPRN MT(MOD_LSFT, KC_LPRN) // Shift on hold, ( on tap
#define MTLGUI_RPRN MT(MOD_LGUI, KC_RPRN) // GUI on hold, ) on tap
#define MTLCTL_RBRC MT(MOD_LCTL, KC_RBRC) // Ctrl on hold, ] on tap
#define MTALT_F5 MT(MOD_LALT, KC_F5) // alt on hold, F5 on tap
#define MTLSFT_F6 MT(MOD_LSFT, KC_F6) // shift on hold, F6 on tap
#define MTGUI_F7 MT(MOD_LGUI, KC_F7) // gui on hold, F7 on tap
#define MTCTL_F8 MT(MOD_LCTL, KC_F8) // ctrl on hold, F8 on tap
#define MTALT_RHGT MT(MOD_LALT, KC_RGHT) // alt on hold, mouse right on tap
#define MTLSFT_DN MT(MOD_LSFT, KC_DOWN) // shift on hold, down arrow on tap
#define MTGUI_LEFT MT(MOD_LGUI, KC_LEFT) // gui on hold, left arrow on tap
#define MTCTL_RCLCK MT(MOD_LCTL, KC_M) // ctrl on hold, right arrow on tap
#define FIND_F3 KC_F3 // search on page
#define BWSRLEFT LGUI(KC_LEFT)
#define BWSRRHGT LGUI(KC_RGHT)
#define CRSR_UP LGUI(LALT(KC_UP)) // add cursor above
#define CRSR_DN LGUI(LALT(KC_DOWN)) // add cursor below
#define SEARCH LGUI(KC_F) // search on page
#define ZOOM_RESET LGUI(KC_0) // reset zoom


#else
// Windows Specific Hotkeys
#define TERMINAL LGUI(KC_GRV)
#define DESKTR LGUI(LCTL(KC_RGHT))  // move one virtual desktop to the right
#define DESKTL LGUI(LCTL(KC_LEFT))  // move one virtual desktop to the left
#define MTLALT_LRBC MT(MOD_LCTL, KC_LBRC) // Ctrl (for windows) on hold, [ on tap
#define MTLSFT_LPRN MT(MOD_LSFT, KC_LPRN) // Shift on hold, ( on tap
#define MTLGUI_RPRN MT(MOD_LALT, KC_RPRN) // Alt (for windows) on hold, ) on tap
#define MTLCTL_RBRC MT(MOD_LGUI, KC_RBRC) // Win/GUI (for windows) on hold, ] on tap
#define MTLALT_Z MT(MOD_LGUI, KC_Z)
#define MTALT_APP MT(MOD_LALT, KC_APP) // Alt on hold, Menu on tap
#define MTCMD_ENT MT(MOD_LCTL, KC_ENT)
#define MTCTL_OSS MT(MOD_LGUI, KC_F24)
#define FIND_F3 KC_F3 // search on page
#define BWSRLEFT A(KC_LEFT)
#define BWSRRHGT A(KC_RGHT)
#define CRSR_UP LCTL(LALT(KC_UP)) // add cursor above
#define CRSR_DN LCTL(LALT(KC_DOWN)) // add cursor below
#define SEARCH LCTL(KC_F) // search on page
#define ZOOM_RESET LCTL(KC_0) // reset zoom



#endif  // MAC_HOTKEYS




enum planck_keycodes {
    COLEMAKDH = SAFE_RANGE,
    QWERTY,
    LOWER,
    RAISE,
    ADJUST,
    NUMPAD,
    GAMING,
    EXT_NUM,
    SELWORD,
    BRACES,
    BRACES2,
    ARROW,
    CTRL_TAB,
    CMD_TAB,
    LLOCK,
    SELWLEFT,
    SELWRIGHT
};
// this is the start of the combo keys
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* MIT Layout (COLEMAKH-DH)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |TRMNAL| F1 -F12- F2 |  F3  |  F4  |  F5  |                                      |  F6  |  F7  |  F8  |  F9  |  F10 +  F11 |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC| Q   |  W   |  F   |  P   |  B   |                                      |  J   |  L   |  U   |  Y   |  ;   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |MEH_TAB| FN_A |  R   |  S   |  T   |  G   |-------.                      .-------|  M   |  N   |  E   |  I   | FN_O |CTRL,'|
 * |------+------+------+------+------+------|Alt-Tab|                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| GUI_Z|  X   |  C   |  D   |  V   |       |-->Play/Pause         | GRV`  |  K   |  H   |  ,   |  .   |  /   |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CMD  |  LOW  / CTRL  /                       \GLOBE \  LOW  |RAISE/ |Cursor Up/Down|
 *                      | APP | ENTER| SPACE/OSShft/                          \Mac FN\ SPACE| DEL   |DIAL2 | -> TOGGLE QWERTY On Press
 *                      `-------------------------'                            '--------------------------'
 */
  [_COLEMAKDH] = LAYOUT(
  TERMINAL, KC_F1,   KC_F2,   KC_F3,     KC_F4,     KC_F5,                               KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,
  HYPERESC, KC_Q,     KC_W,   KC_F,      KC_P,      KC_B,                                KC_J,    KC_L,    KC_U,    KC_Y,   KC_SCLN, KC_BSPC,
  MEH_TAB,  FN_A,     KC_R,   KC_S,      KC_T,      KC_G,                                KC_M,    KC_N,    KC_E,    KC_I,   FN_O,    MTRCTLQUO,
  KC_LSFT,  MTLALT_Z, KC_X,   KC_C,      KC_D,      KC_V,    KC_MPLY,          ALT_GRV,  KC_K,    KC_H,    KC_COMM, KC_DOT, KC_SLSH, MTRSFTBSLS,
                              MTALT_APP, MTCMD_ENT, LOW_SPC, MTCTL_OSS,        KC_GLOBE, LOW_SPC, RSE_DEL, TG(_QWERTY) //Mute Mic
),

/* MIT Layout (QWERTY)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |TRMNAL| F1 -F12- F2 |  F3  |  F4  |  F5  |                                      |  F6  |  F7  |  F8  |  F9  |  F10 +  F11 |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |HYP,ESC| Q   |  W   |  E   |  R   |  T   |                                      |  Y   |  U   |  I   |  O   |  P   | BSPC |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |MEH_TAB| FN_A |  S   |  D   |  F  -@- G   |-------.                      .-------|  H   |  J   |  K   |  L   | FN_; |CTRL,'|
 * |------+------+------+------+------+------|Alt-Tab|                      | LALT  |------+------+------+------+------+------|
 * | SHIFT| GUI_Z|  X   |  C   |  V   |  B   |       |-->Play/Pause         | GRV`  |  N   |  M   |  ,   |  .   |  /   |SFT,\ |
 * .-----------------------------------------|-------|   on Button Press    |-------|-----------------------------------------'
 *                      | ALT | CTRL |  LOW  /      /                        \GLOBE \  LOW  |RAISE| Cursor R/L|
 *                      | APP | ENTER| SPACE/ OSSft/                          \Mac FN\ SPACE| DEL | DIAL2 | -> TOGGLE QWERTY On Press
 *                      `-------------------------'                            '-------------------------'
 */
  [_QWERTY] = LAYOUT(
  TERMINAL, KC_F1,   KC_F2,     KC_F3,     KC_F4,     KC_F5,                               KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,
  HYPERESC, KC_Q,     KC_W,     KC_E,      KC_R,      KC_T,                                KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,    KC_BSPC,
  MEH_TAB,   FN_A,     KC_S,     KC_D,      KC_F,      KC_G,                                KC_H,    KC_J,    KC_K,    KC_L,   FN_SCLN, MTRCTLQUO,
  KC_LSFT,  MTLALT_Z, KC_X,     KC_C,      KC_V,      KC_B,    KC_MPLY,          ALT_GRV,  KC_N,    KC_M,    KC_COMM, KC_DOT, KC_SLSH, MTRSFTBSLS,
                                MTALT_APP, MTCMD_ENT, LOW_SPC, MTCTL_OSS,        KC_GLOBE, LOW_SPC, RSE_DEL, TG(_QWERTY)
),

/* MIT Layout (LOWER) // couldn't get mod-tap to work with LPRN and RPRN
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |CGSWAP|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0   | F12  |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
*  |  `   |  !   |  <   |  >   |  =   |  ^   |                                      |   *  |   7  |   8  |   9  |   :  | Bsp  |
 * |------+------+------+------+-------------|                                      |------+------+------+------+------+------|
 * |S(TAB)|  #   | [,alt|(,sft |),gui | ],ctl|-------.                      .-------|   &  |   4  |   5  |   6  |   -  |  +   |
 * |------+------+------+------+------+------|Volume |                      |       |------+------+------+------+------+------|
 * |SHIFT |  ~   |  $   |  {   |  }   |  %   | DIAL1 |--> Press for         |       |   |  |   1  |   2  |   3  |   /  |  |   |
 * .-----------------------------------------|-------|   ZOOM_RESET         |-------|-----------------------------------------'
 *                      | ALT | CTRL |  LOW  /      /                        \       \      |     |ZOOM IN/OUT|
 *                      | APP | ENTER| SPACE/ OSSft/                          \ SPACE \  0  |  .  | DIAL2|--> LAYER LOCK on Press
 *                      `-------------------------'                            '-------------------------'
 */
  [_LOWER] = LAYOUT(
  CG_SWAP,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                KC_6,    KC_7,   KC_8,  KC_9,  KC_0,    KC_F12,
  KC_GRV,    KC_EXLM, KC_LABK, KC_RABK, KC_EQL,  KC_CIRC,                             KC_ASTR, KC_7,   KC_8,  KC_9,  KC_COLN, KC_BSPC,
  S(KC_TAB), KC_HASH, KC_LBRC, KC_LPRN, KC_RPRN, KC_RBRC,                             KC_AMPR, KC_4,   KC_5,  KC_6,  KC_PMNS, KC_PPLS,
  KC_LSFT,   KC_TILD, KC_DLR,  KC_LCBR, KC_RCBR, KC_PERC, ZOOM_RESET,        KC_TILD, KC_PIPE, KC_1,   KC_2,  KC_3,  KC_PSLS, KC_PIPE,
                               _____,   _____,   _____,   _____,             KC_SPC,  KC_0,    KC_DOT, LLOCK //LAYER LOCK on Press
),

/* MIT Layout (RAISE)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |CGSWAP|  1   |  2   |  3   |  4   |  5   |                                      |  6   |  7   |  8   |  9   |  0   |QWERTY|
 * |------+------+------+------+------+------|                               point  |------+------+------+------+------+------|
 * |CGNORM| Mb3  | Mb2  | MsUp | Mb1  | Mute |                               mouse  |      |      | MbMid|      |   :  | Bksp |
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |      |      | MsLft| MDown|Mright| Vol+ |-------.                      .-------|ARROW | MbLft|SELWORD|MbRgt|      |   "  |
 * |------+------+------+------+------+------|Undo/Redo|                    |       |------+------+------+------+------+------|
 * |      | MWLft| MWUp | NWDn |NWRght| Vol- | DIAL1 |--> Press for         | LLOCK |      |BRACES2|SELWORD|BRACES| !  |   |  |
 * .-----------------------------------------|-------|                      |-------|-----------------------------------------'
 *                      | ALT | CTRL |  LOW  /      /                        \ Mouse\ Mouse |******|Search|
 *                      | APP | ENTER| OSSft/ SPACE/                          \ Btn 1\ Btn 3|******| DIAL2|--> //LAYER LOCK on Press
 *                      `-------------------------'                            '--------------------------'
 */
  [_RAISE] = LAYOUT(
  CG_SWAP, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                   KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    TG(_QWERTY),
  CG_NORM, KC_BTN3, KC_BTN2, KC_MS_U, KC_BTN1, KC_MUTE,                                _____,   _____,   KC_BTN3, KC_NUM,  KC_COLN, KC_BSPC,
  _____,   _____,   KC_MS_L, KC_MS_D, KC_MS_R, KC_VOLU,                                ARROW,   KC_BTN1, SELWORD, KC_BTN2, _____,   KC_DQUO,
  _____,   KC_WH_L, KC_WH_U, KC_WH_D, KC_WH_R, KC_VOLD, LLOCK,                _____,   _____,   BRACES2, SELWORD, BRACES,  KC_EXLM, KC_PIPE,
                                  _____, LGUI(KC_ENT), _____, _____,          KC_BTN1, KC_BTN3, _____,   LLOCK //LAYER LOCK on Press
),

/* MIT Layout (ADJUST)
 * .-----------------------------------------.                                      .-----------------------------------------.
 * |CGSWAP|  F1  |  F2  |  F3  |  F4  |  F5  |                                      |TabUp | Back |C+A_Up|Frward|SclLok|QWERTY|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |CGNORM|      |  F9  |  F10 | F11  |  F12 |                                      |TabDn | Home |  Up  |  End |NumLck|Delete|
 * |------+------+------+------+------+------|                                      |------+------+------+------+------+------|
 * |******|*STAB*|Alt,F5|Sft,F6|Cmd,F7|Ctl,F8|--------.                     .-------|      | Left | Down | Right|      |CAPSLK|
 * |------+------+------+------+------+------| Program|                     | APP   |------+------+------+------+------+------|
 * |      |      |  F1  |  F2  |  F3  |  F4  |Switcher|-->  Press For       |SWITCHER|     |PageUp|C+A_Dn|PageDn|Forwrd|INSERT|
 * .-----------------------------------------|--------|     LLOCK           |-------|-----------------------------------------'
 *                      | ALT | CTRL |  Alt  / Ctrl  /                       \ TAB   \       |      | Cursor Left/Right |
 *                      | APP | ENTER| Tab  /  Tab  /                         \SWITCHER\     |      | DIAL2 |---> //LAYER LOCK on Press
 *                      `--------------------------'                           '----------------------------'
 */
  [_ADJUST] = LAYOUT(
  CG_SWAP, KC_F1, KC_F2,    KC_F3,     KC_F4,    KC_F5,                                C(KC_PGUP), BWSRLEFT, CRSR_UP, BWSRRHGT, KC_SCRL,  TG(_QWERTY),
  CG_NORM, _____, KC_F9,    KC_F10,    KC_F11,   KC_F12,                               C(KC_PGDN), KC_HOME,  KC_UP,   KC_END,   KC_NUM,   KC_DEL,
  _____,   _____, MTALT_F5, MTLSFT_F6, MTGUI_F7, MTCTL_F8,                             _____,      KC_LEFT,  KC_DOWN, KC_RGHT,  _____,    KC_CAPS,
  _____,   _____, KC_F1,    KC_F2,     KC_F3,    KC_F4,    LLOCK,            CMD_TAB,  _____,      KC_PGUP,  CRSR_DN, KC_PGDN,  BWSRRHGT, KC_INS,
                              _____,   _____,    _____,    _____,            CTRL_TAB, _____,      _____,    LLOCK //LAYER LOCK on Press
)
};

static fast_timer_t last_encoding_time = 0;
static const fast_timer_t ENCODER_DEBOUNCE = 50;

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (TIMER_DIFF_FAST(timer_read_fast(), last_encoding_time) >= ENCODER_DEBOUNCE) {
        last_encoding_time = timer_read_fast();
    }
    else {
        return false;
    }
    // then goes the rest of your rotary encoder handling code.
    if (index == 0) {
        uint8_t mod_state = get_mods();
        switch (biton32(layer_state)) {
            case _LOWER:
            // Volume Up/Down or Brightness up/ down with shift
                if (mod_state & MOD_MASK_SHIFT) {
                    unregister_mods(mod_state);
                    wait_ms(100);
                    if (clockwise) {
                        tap_code(KC_BRIGHTNESS_DOWN);
                    } else {
                        tap_code(KC_BRIGHTNESS_UP);
                    }
                    register_mods(mod_state);
                } else {
                    if (clockwise) {
                        tap_code(KC_AUDIO_VOL_DOWN);
                    } else {
                        tap_code(KC_AUDIO_VOL_UP);
                    }
                }
                // Next song on button press
            case _ADJUST:
                // CMD/ALT Tab through open windows
                if (!clockwise) {
                    tabtimer = timer_read();
                    if(!tabbing) {
#ifdef MAC_HOTKEYS
                        register_code(KC_LGUI);
#else
                        register_code(KC_LALT);
#endif  // MAC_HOTKEYS
                        register_code(KC_TAB);
                        unregister_code(KC_TAB);
                        tabbing = true;
                    }
                    tap_code(KC_RGHT);
                } else {
                    tabtimer = timer_read();
                    if(!tabbing) {
#ifdef MAC_HOTKEYS
                        register_code(KC_LGUI);
#else
                        register_code(KC_LALT);
#endif  // MAC_HOTKEYS
                        register_code(KC_TAB);
                        unregister_code(KC_TAB);
                        tabbing = true;
                    }
                    tap_code(KC_LEFT);
                }
                break;
                //  on press, LAYER LOCK
            default:
              // CTRL_TAB through open Tabs
                if (!clockwise) {
                    ctrl_tabtimer = timer_read();
                    if(!ctrl_tabbing) {
                        register_code(KC_LCTL);
                        ctrl_tabbing = true;
                    }
                    tap_code(KC_TAB);
                } else {
                    ctrl_tabtimer = timer_read();
                    if(!ctrl_tabbing) {
                        register_code(KC_LCTL);
                    ctrl_tabbing = true;
                    }
                    register_code(KC_LSFT);
                    tap_code(KC_TAB);
                    unregister_code(KC_LSFT);
                    }
                    break;
        }
    } else if (index == 1) {
        switch (biton32(layer_state)) {
            case _LOWER:
            // Zoom in / Zoom Out
#ifdef MAC_HOTKEYS
                if (!clockwise) {
                    tap_code16(LGUI(KC_PMNS));
                } else {
                    tap_code16(LGUI(KC_PPLS));
                }
#else
                if (!clockwise) {
                    tap_code16(LCTL(KC_PMNS));
                } else {
                    tap_code16(LCTL(KC_PPLS));
                }
#endif
                break;
            case _RAISE:
            // Undo / Redo - Revision History
#ifdef MAC_HOTKEYS
                if (!clockwise) {
                    tap_code16(LGUI(KC_Z));
                } else {
                    tap_code16(LGUI(LSFT(KC_Z)));
                }
#else
                if (!clockwise) {
                    tap_code16(LCTL(KC_Z));
                } else {
                    tap_code16(LCTL(KC_Y));
                }
#endif
              //  on press, LAYER LOCK
                break;
            case _ADJUST:
               // Move cursor up and down
                if (!clockwise) {
                    tap_code16(KC_LEFT);
                } else {
                    tap_code16(KC_RGHT);
                }
                break;
                // on press, LAYER LOCK
            default:
               // Move cursor left and right
                if (!clockwise) {
                    tap_code16(KC_UP);
                } else {
                    tap_code16(KC_DOWN);
                }
                break;
                // on press, LAYER LOCK
        }
    }
    return false;
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_layer_lock(keycode, record, LLOCK)) { return false; }
    if (!process_select_word(keycode, record, SELWORD)) { return false; }
    if (!process_adaptive_key(keycode, record)) { return false; }
 // if (!process_caps_word(keycode, record)) { return false; }
  //if (!process_autocorrection(keycode, record)) { return false; }

    const uint8_t mods = get_mods();
    const uint8_t oneshot_mods = get_oneshot_mods();

    switch (keycode) {
        case BRACES:  // Types (), or {}, and puts cursor between braces.
            if (record->event.pressed) {
                clear_mods();  // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                SEND_STRING("<>");
                } else {
                SEND_STRING("()");
                }
                tap_code(KC_LEFT);  // Move cursor between braces.
                set_mods(mods);  // Restore mods.
            }
            return false;
            break;
        case BRACES2:  // Types [], or <>, and puts cursor between braces.
            if (record->event.pressed) {
                clear_mods();  // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                SEND_STRING("{}");
                } else {
                SEND_STRING("[]");
                }
                tap_code(KC_LEFT);  // Move cursor between braces.
                set_mods(mods);  // Restore mods.
            }
            return false;
            break;
        case ARROW:  // Arrow macro, types -> or =>.
            if (record->event.pressed) {
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {  // Is shift held?
                    del_mods(MOD_MASK_SHIFT);  // Temporarily delete shift.
                    del_oneshot_mods(MOD_MASK_SHIFT);
                    SEND_STRING("const yourFuncVariable = (x,y) => x + y.method;");
                for (int counter = 0; counter <= 24; counter++){
                    tap_code(KC_LEFT);
                }
#ifdef MAC_HOTKEYS
                tap_code16(LALT(LSFT(KC_LEFT)));
#else
                tap_code16(LCTL(LSFT(KC_LEFT)));
#endif  // MAC_HOTKEYS
                set_mods(mods);  // Restore mods.
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
                if(!ctrl_tabbing) {
                    register_code(KC_LCTL);
                    ctrl_tabbing = true;
                }
            tap_code(KC_TAB);
            }
            return false;
            break;
        case CMD_TAB:
            if (record->event.pressed) {
                tabtimer = timer_read_fast();
                if(!tabbing) {
#ifdef MAC_HOTKEYS
                    register_code(KC_LGUI);
#else
                    register_code(KC_LALT);
#endif  // MAC_HOTKEYS
                    tabbing = true;
                }
            tap_code(KC_TAB);
            }
            return false;
            break;
#ifdef MAC_HOTKEYS
        case MT(MOD_LCTL, KC_F24):
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

enum combo_events {
    EM_EMAIL,
    EM_WORK_EMAIL,
    HOME_ADDRESS,
    HTML_P,
    HTML_TITLE,
    HTML_DIV,
    HTML_HTML,
    HTML_HEAD,
    HTML_BODY,
    HTML_FOOTER,
    HTML_A_HREF,
    HTML_IMG,
    CSS_STYLE,
    HTML_GENERIC_TAG,
    UNDO,
    REDO,
    CUT,
    COPY,
    PASTE,
    PASTECLIPBOARD,
    PASTETEXT,
    SELECTALL,
    QUESTIONMARK,
    EXCLAMATIONMARK,
    DQUOTE,
    UNDERSCORE,
    DASH,
    EQUALSIGN,
    ATSYMB,
    QUOTES,
    LOWERTOGGLE,
    SLEEP,
    RESETKEY,
    NUMLOCKC,
    F12COMBO,
    CAPSWORD,
    COMBO_LENGTH
};


uint16_t COMBO_LEN = COMBO_LENGTH; // remove the COMBO_COUNT define and use this instead

const uint16_t PROGMEM email_combo[]                = {KC_J, KC_L, COMBO_END};
const uint16_t PROGMEM email_work_combo[]           = {KC_M, KC_N, COMBO_END};
const uint16_t PROGMEM home_address_combo[]         = {KC_E, KC_J, COMBO_END};
const uint16_t PROGMEM html_p_combo[]               = {KC_P, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_title_combo[]           = {KC_T, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_div_combo[]             = {KC_D, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_html_combo[]            = {KC_Q, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_head_combo[]            = {KC_W, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_body_combo[]            = {KC_R, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_footer_combo[]          = {KC_X, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_a_href_combo[]          = {KC_A, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_img_combo[]             = {KC_F, KC_DOT, COMBO_END};
const uint16_t PROGMEM css_style_combo[]            = {KC_S, KC_DOT, COMBO_END};
const uint16_t PROGMEM html_generic_tag_combo[]     = {KC_G, KC_DOT, COMBO_END};
const uint16_t PROGMEM undo_combo[]                 = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM redo_combo[]                 = {KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM cut_combo[]                  = {MTLALT_Z, KC_X, COMBO_END};
const uint16_t PROGMEM copy_combo[]                 = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM paste_combo[]                = {KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM pasteclip_combo[]            = {KC_X, KC_D, COMBO_END};
const uint16_t PROGMEM pastetxt_combo[]             = {KC_X, KC_V, COMBO_END};
const uint16_t PROGMEM selectall_combo[]            = {MTLALT_Z, KC_D, COMBO_END};
const uint16_t PROGMEM questionmark_combo[]         = {KC_DOT, KC_SLSH, COMBO_END};
const uint16_t PROGMEM exclamationmark_combo[]      = {KC_SLSH, MTRSFTBSLS, COMBO_END};
const uint16_t PROGMEM dquote_combo[]               = {KC_O, MTRCTLQUO, COMBO_END};
const uint16_t PROGMEM underscore_combo[]           = {KC_COMMA, KC_DOT, COMBO_END};
const uint16_t PROGMEM dash_combo[]                 = {KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM equalsign_combo[]            = {KC_E, KC_I, COMBO_END};
const uint16_t PROGMEM atsymb_combo[]               = {KC_T, KC_G, COMBO_END};
const uint16_t PROGMEM QUOTES_combo[]               = {KC_H, KC_COMMA, COMBO_END};
const uint16_t PROGMEM lowertoggle_combo[]          = {LOW_SPC, MTCMD_ENT, COMBO_END};
const uint16_t PROGMEM sleep_combo[]                = {KC_F7, KC_F8, KC_F9, KC_F10, COMBO_END};
const uint16_t PROGMEM reset_combo  []              = {KC_BSPC, MTRCTLQUO, MTRSFTBSLS, COMBO_END};
const uint16_t PROGMEM numlock_combo[]              = {KC_L, KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM f12_combo[]                  = {KC_F1, KC_F2, COMBO_END};
const uint16_t PROGMEM capsword_combo[]             = {KC_LSFT, MTRSFTBSLS, COMBO_END};


// const uint8_t combo_mods = get_mods();r
// const uint8_t combo_oneshot_mods = get_oneshot_mods();

combo_t key_combos[] = {
    [EM_EMAIL] = COMBO_ACTION(email_combo),
    [EM_WORK_EMAIL] = COMBO_ACTION(email_work_combo),
    [HOME_ADDRESS] = COMBO_ACTION(home_address_combo),
    [HTML_P] = COMBO_ACTION(html_p_combo),
    [HTML_TITLE] = COMBO_ACTION(html_title_combo),
    [HTML_DIV] = COMBO_ACTION(html_div_combo),
    [HTML_HTML] = COMBO_ACTION(html_html_combo),
    [HTML_HEAD] = COMBO_ACTION(html_head_combo),
    [HTML_BODY] = COMBO_ACTION(html_body_combo),
    [HTML_FOOTER] = COMBO_ACTION(html_footer_combo),
    [HTML_A_HREF] = COMBO_ACTION(html_a_href_combo),
    [HTML_IMG] = COMBO_ACTION(html_img_combo),
    [CSS_STYLE] = COMBO_ACTION(css_style_combo),
    [HTML_GENERIC_TAG] = COMBO_ACTION(html_generic_tag_combo),
    [UNDO] = COMBO_ACTION(undo_combo),
    [REDO] = COMBO_ACTION(redo_combo),
    [CUT] = COMBO_ACTION(cut_combo),
    [COPY] = COMBO_ACTION(copy_combo),
    [PASTE] = COMBO_ACTION(paste_combo),
    [PASTECLIPBOARD] = COMBO_ACTION(pasteclip_combo),
    [PASTETEXT] = COMBO_ACTION(pastetxt_combo),
    [SELECTALL] = COMBO_ACTION(selectall_combo),
    [QUESTIONMARK] = COMBO_ACTION(questionmark_combo),
    [EXCLAMATIONMARK] = COMBO_ACTION(exclamationmark_combo),
    [DQUOTE] = COMBO_ACTION(dquote_combo),
    [UNDERSCORE] = COMBO_ACTION(underscore_combo),
    [DASH] = COMBO_ACTION(dash_combo),
    [EQUALSIGN] = COMBO_ACTION(equalsign_combo),
    [ATSYMB] = COMBO_ACTION(atsymb_combo),
    [QUOTES] = COMBO_ACTION(QUOTES_combo),
    [LOWERTOGGLE] = COMBO_ACTION(lowertoggle_combo),
    [SLEEP] = COMBO_ACTION(sleep_combo),
    [RESETKEY] = COMBO_ACTION(reset_combo),
    [NUMLOCKC] = COMBO_ACTION(numlock_combo),
    [F12COMBO] = COMBO_ACTION(f12_combo),
    [CAPSWORD] = COMBO_ACTION(capsword_combo)
};
/* COMBO_ACTION(x) is same as COMBO(x, XXXXX) */

void process_combo_event(uint16_t combo_index, bool pressed) {
    const uint8_t mods = get_mods();
    const uint8_t oneshot_mods = get_oneshot_mods();

    switch(combo_index) {
        case EM_EMAIL:
            if (pressed) {
                SEND_STRING("contact@ariccb.dev");
            }
            break;
        case EM_WORK_EMAIL:
            if (pressed) {
                SEND_STRING("aric@collegiumbuilt.com");
            }
            break;
        case HOME_ADDRESS:
            if (pressed) {
                SEND_STRING("111 Highview Gate SE");
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
#endif  // MAC_HOTKEYS
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
                register_code(KC_LGUI);
                register_code(KC_X);
                unregister_code(KC_LGUI);
                unregister_code(KC_X);
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
                register_code(KC_LGUI);
                register_code(KC_C);
                unregister_code(KC_LGUI);
                unregister_code(KC_C);
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
                register_code(KC_LGUI);
                register_code(KC_V);
                unregister_code(KC_LGUI);
                unregister_code(KC_V);
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
                unregister_code(KC_LGUI);
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
                clear_mods();  // Temporarily disable mods.
                clear_oneshot_mods();
                if ((mods | oneshot_mods) & MOD_MASK_SHIFT) {
                SEND_STRING("'");
                } else {
                SEND_STRING("\"");
                }
                set_mods(mods);  // Restore mods.
                }
                break;
        case LOWERTOGGLE:
            if (pressed) {
                layer_invert(_LOWER);
                if(IS_LAYER_ON(_LOWER)){
                #ifdef AUDIO_ENABLE
                    PLAY_SONG(tone_startup);
                #endif
                }
                if(IS_LAYER_OFF(_LOWER)){
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
        case CAPSWORD:
            caps_word_on();
        break;
    }
};
