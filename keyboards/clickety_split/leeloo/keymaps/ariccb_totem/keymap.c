/* Totem layout on the Leeloo, for practice before the Totem arrives.
 *
 * The 38 Totem keys sit on the Lily58 matrix: 3x5 alphas on the inner
 * columns, outer pinkies on the bottom row's outer column, and the three
 * Totem thumbs on the three innermost thumb keys. Every other key is KC_NO.
 *
 * Combos are positional on Colemak (COMBO_ONLY_FROM_LAYER 0) and are
 * suppressed while a momentary layer is up, except the terminal combos,
 * which fire only on Lower (Num on the Pro Micro). On the KB2040 image,
 * L+U+Y latches Nav on every layer.
 */

#include QMK_KEYBOARD_H
#include "send_string.h"

#include "features/adaptive_keys.h"
#include "features/hold_tap.h"
#include "features/os_mode.h"

enum combo_events {
    CMB_UNDO,
    CMB_REDO,
    CMB_CUT,
    CMB_COPY,
    CMB_PASTE,
    CMB_CLIP,
    CMB_PASTETXT,
    CMB_SELALL,
    CMB_QUES,
    CMB_UNDS,
    CMB_DASH,
    CMB_EQL,
    CMB_AT,
    CMB_QUOTES,
    CMB_CAPS,
    CMB_NUM,
    CMB_ARROW,
    CMB_CONSOLE,
    CMB_HTML_P,
    CMB_HTML_TITLE,
    CMB_HTML_DIV,
    CMB_HTML_HTML,
    CMB_HTML_HEAD,
    CMB_HTML_BODY,
    CMB_HTML_FOOTER,
    CMB_HTML_HREF,
    CMB_HTML_IMG,
    CMB_HTML_STYLE,
    CMB_HTML_TAG,
    CMB_TERM_OPEN,
    CMB_TERM_NEXT,
    CMB_TERM_PREV,
    CMB_TERM_NEW,
    COMBO_LENGTH
};

uint16_t COMBO_LEN = COMBO_LENGTH;

const uint16_t PROGMEM c_undo[]     = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM c_redo[]     = {KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM c_cut[]      = {KC_Z, KC_X, COMBO_END};
const uint16_t PROGMEM c_copy[]     = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM c_paste[]    = {KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM c_clip[]     = {KC_X, KC_D, COMBO_END};
const uint16_t PROGMEM c_ptxt[]     = {KC_X, KC_V, COMBO_END};
const uint16_t PROGMEM c_selall[]   = {KC_Z, KC_D, COMBO_END};
const uint16_t PROGMEM c_ques[]     = {KC_DOT, KC_SLSH, COMBO_END};
const uint16_t PROGMEM c_unds[]     = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_dash[]     = {KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM c_eql[]      = {TH_KC_RMID, TH_KC_RRING, COMBO_END};
const uint16_t PROGMEM c_at[]       = {TH_KC_LIDX, TH_KC_LIN, COMBO_END};
const uint16_t PROGMEM c_quotes[]   = {KC_H, KC_COMM, COMBO_END};
const uint16_t PROGMEM c_caps[]     = {TH_KC_LIDX, TH_KC_RIDX, COMBO_END};
const uint16_t PROGMEM c_num[]      = {KC_L, KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM c_arrow[]    = {TH_KC_PINKY, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_console[]  = {KC_H, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hp[]       = {KC_P, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_htitle[]   = {TH_KC_LIDX, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hdiv[]     = {KC_D, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hhtml[]    = {KC_Q, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hhead[]    = {KC_W, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hbody[]    = {TH_KC_LRING, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hfoot[]    = {KC_X, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hhref[]    = {KC_F, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_himg[]     = {TH_KC_RRING, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_hstyle[]   = {TH_KC_LMID, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_htag[]     = {TH_KC_LIN, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_term[]     = {KC_L, KC_U, COMBO_END};
const uint16_t PROGMEM c_termn[]    = {KC_J, KC_L, COMBO_END};
const uint16_t PROGMEM c_termp[]    = {KC_U, KC_SLSH, COMBO_END};
const uint16_t PROGMEM c_termnew[]  = {KC_J, KC_L, KC_U, COMBO_END};

combo_t key_combos[] = {
    [CMB_UNDO]        = COMBO_ACTION(c_undo),
    [CMB_REDO]        = COMBO_ACTION(c_redo),
    [CMB_CUT]         = COMBO_ACTION(c_cut),
    [CMB_COPY]        = COMBO_ACTION(c_copy),
    [CMB_PASTE]       = COMBO_ACTION(c_paste),
    [CMB_CLIP]        = COMBO_ACTION(c_clip),
    [CMB_PASTETXT]    = COMBO_ACTION(c_ptxt),
    [CMB_SELALL]      = COMBO_ACTION(c_selall),
    [CMB_QUES]        = COMBO(c_ques, KC_QUES),
    [CMB_UNDS]        = COMBO(c_unds, KC_UNDS),
    [CMB_DASH]        = COMBO(c_dash, KC_MINS),
    [CMB_EQL]         = COMBO(c_eql, KC_EQL),
    [CMB_AT]          = COMBO(c_at, KC_AT),
    [CMB_QUOTES]      = COMBO_ACTION(c_quotes),
    [CMB_CAPS]        = COMBO(c_caps, CW_TOGG),
    [CMB_NUM]         = COMBO_ACTION(c_num),
    [CMB_ARROW]       = COMBO_ACTION(c_arrow),
    [CMB_CONSOLE]     = COMBO_ACTION(c_console),
    [CMB_HTML_P]      = COMBO_ACTION(c_hp),
    [CMB_HTML_TITLE]  = COMBO_ACTION(c_htitle),
    [CMB_HTML_DIV]    = COMBO_ACTION(c_hdiv),
    [CMB_HTML_HTML]   = COMBO_ACTION(c_hhtml),
    [CMB_HTML_HEAD]   = COMBO_ACTION(c_hhead),
    [CMB_HTML_BODY]   = COMBO_ACTION(c_hbody),
    [CMB_HTML_FOOTER] = COMBO_ACTION(c_hfoot),
    [CMB_HTML_HREF]   = COMBO_ACTION(c_hhref),
    [CMB_HTML_IMG]    = COMBO_ACTION(c_himg),
    [CMB_HTML_STYLE]  = COMBO_ACTION(c_hstyle),
    [CMB_HTML_TAG]    = COMBO_ACTION(c_htag),
    [CMB_TERM_OPEN]   = COMBO_ACTION(c_term),
    [CMB_TERM_NEXT]   = COMBO_ACTION(c_termn),
    [CMB_TERM_PREV]   = COMBO_ACTION(c_termp),
    [CMB_TERM_NEW]    = COMBO_ACTION(c_termnew),
};

#define XXX KC_NO
#define ___ KC_TRNS

#ifdef CONVERT_TO_KB2040
// Momentary thumbs. Release returns to the base layer. Tapping term is 200 ms.
// Right side matches Totem: plain grave, Backspace/Lower, Delete/Raise.
#    define TH_ENT MT(MOD_LGUI, KC_ENT)              // Enter, hold Cmd
#    define TH_SPC LT(L_LOWER, KC_SPC)               // Space, hold Lower
#    define TH_TAB MT(MOD_LGUI | MOD_LALT, KC_TAB)   // Tab, hold Cmd+Alt
#    define TH_BSPC LT(L_LOWER, KC_BSPC)             // Backspace, hold Lower
#    define TH_DEL LT(L_RAISE, KC_DEL)               // Delete, hold Raise
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#ifdef CONVERT_TO_KB2040
    /* Totem master (Colemak port + Raise mouse/media). Keys the Totem
     * does not have stay KC_NO: the encoder beside V, the outer right
     * thumb, and the outer columns. The key beside K stays Enter.
     * Thumbs, left to right: Enter/Cmd, Space/Lower, Tab/Cmd+Alt,
     * grave, Backspace/Lower, Delete/Raise. */
    [L_BASE] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_Q, KC_W, KC_F, KC_P, KC_B, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, XXX,
        XXX, TH_KC_PINKY, TH_KC_LRING, TH_KC_LMID, TH_KC_LIDX, TH_KC_LIN, TH_KC_RIN, TH_KC_RIDX, TH_KC_RMID, TH_KC_RRING, KC_O, XXX,
        TH_KC_HESC, KC_Z, KC_X, KC_C, KC_D, KC_V, XXX, KC_ENT, KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, TH_KC_HBSP,
        XXX, TH_ENT, TH_SPC, TH_TAB, KC_GRV, TH_BSPC, TH_DEL, XXX),

    [L_QWERTY] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, XXX,
        XXX, TH_KC_PINKY, TH_KC_LRING, TH_KC_LMID, TH_KC_LIDX, TH_KC_LIN, TH_KC_RIN, TH_KC_RIDX, TH_KC_RMID, TH_KC_RRING, KC_SCLN, XXX,
        TH_KC_HESC, KC_Z, KC_X, KC_C, KC_V, KC_B, XXX, KC_ENT, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, TH_KC_HBSP,
        XXX, TH_ENT, TH_SPC, TH_TAB, KC_GRV, TH_BSPC, TH_DEL, XXX),

    [L_CANARY] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_Q, KC_L, KC_Y, KC_P, KC_B, KC_Z, KC_F, KC_O, KC_U, KC_SCLN, XXX,
        XXX, TH_KC_PINKY, TH_KC_LRING, TH_KC_LMID, TH_KC_LIDX, TH_KC_LIN, TH_KC_RIN, TH_KC_RIDX, TH_KC_RMID, TH_KC_RRING, KC_A, XXX,
        TH_KC_HESC, KC_W, KC_J, KC_V, KC_D, KC_K, XXX, KC_ENT, KC_X, KC_H, KC_COMM, KC_DOT, KC_SLSH, TH_KC_HBSP,
        XXX, TH_ENT, TH_SPC, TH_TAB, KC_GRV, TH_BSPC, TH_DEL, XXX),

    /* Left symbols keep Totem's inner-column order. Right hand is
     * *789+ / &456- / ~123/|. Thumbs: grave, Shift-Tab, transparent,
     * next sentence, period, 0. */
    [L_LOWER] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_EXLM, KC_LABK, KC_RABK, KC_EQL, KC_CIRC, KC_ASTR, KC_7, KC_8, KC_9, KC_PLUS, XXX,
        XXX, KC_HASH, KC_LBRC, KC_LPRN, KC_RPRN, KC_RBRC, KC_AMPR, KC_4, KC_5, KC_6, KC_MINS, XXX,
        KC_LSFT, KC_AT, KC_DLR, KC_LCBR, KC_RCBR, KC_PERC, XXX, KC_ENT, KC_TILD, KC_1, KC_2, KC_3, KC_SLSH, KC_PIPE,
        XXX, KC_GRV, S(KC_TAB), ___, NEXT_SENTENCE, KC_DOT, KC_0, XXX),

    /* F9-F12 on the inner top columns. F1-F4 sit one key to the right
     * of the outer pinky, so Z is blank. Prev/next tab are raw Ctrl+Tab
     * on either OS. L+U+Y latches this layer. */
    [L_NAV] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, XXX, KC_F9, KC_F10, KC_F11, KC_F12, C(KC_PGUP), KC_HOME, KC_UP, KC_END, XXX, XXX,
        XXX, ___, KC_F5, KC_F6, KC_F7, KC_F8, OS_TAB_PREV, KC_LEFT, KC_DOWN, KC_RGHT, OS_TAB_NEXT, XXX,
        ___, XXX, KC_F1, KC_F2, KC_F3, KC_F4, XXX, KC_ENT, OS_BACK, KC_PGUP, G(A(KC_DOWN)), KC_PGDN, OS_FWD, XXX,
        XXX, OS_APPS, KC_SPC, OS_SWITCH, KC_ENT, KC_BSPC, KC_DEL, XXX),

    /* Left hand is the mouse block. Y is unused. J/L/U are volume,
     * semicolon is next track. Home row is mic mute, previous, play,
     * next. Far-left thumb is Cmd+Enter / Ctrl+Enter. */
    [L_RAISE] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_WH_U, KC_BTN2, KC_MS_U, KC_BTN1, KC_BTN3, KC_VOLD, KC_VOLU, KC_MUTE, ___, KC_MNXT, XXX,
        XXX, ___, ___, ___, ___, ___, C(A(G(KC_M))), KC_MPRV, KC_MPLY, KC_MNXT, ___, XXX,
        KC_WH_D, KC_MS_L, KC_MS_D, KC_MS_R, ___, ___, XXX, KC_ENT, XXX, OS_SELWORD, A(C(S(KC_RGHT))), OS_SELLINE, OS_BACK, OS_FWDBR,
        XXX, OS_ENT, ___, ___, ___, KC_BSPC, KC_DEL, XXX),

    /* Mac on Q, Windows on A, bootloader on the outer left pinky.
     * Bluetooth, output toggle, and the autocorrect key are absent. */
    [L_SYS] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, CG_NORM, XXX, XXX, XXX, XXX, TO_BASE, TO_QWERTY, TO_CANARY, XXX, XXX, XXX,
        XXX, CG_SWAP, QK_RBT, XXX, XXX, XXX, OS_SAVE, OS_FIND, XXX, XXX, KC_SLEP, XXX,
        QK_BOOT, XXX, XXX, XXX, XXX, XXX, XXX, KC_ENT, XXX, XXX, XXX, XXX, XXX, ___,
        XXX, XXX, XXX, ___, ___, XXX, XXX, XXX),
#else
    [L_BASE] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_Q, KC_W, KC_F, KC_P, KC_B, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, XXX,
        XXX, TH_KC_PINKY, TH_KC_LRING, TH_KC_LMID, TH_KC_LIDX, TH_KC_LIN, TH_KC_RIN, TH_KC_RIDX, TH_KC_RMID, TH_KC_RRING, KC_O, XXX,
        TH_KC_HESC, KC_Z, KC_X, KC_C, KC_D, KC_V, TH_KC_TNUM, TH_KC_TSYM, KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, TH_KC_HBSP,
        XXX, XXX, OSM(MOD_LSFT), TH_KC_TNAV, TH_KC_TADJ, KC_DEL, XXX, XXX),

    [L_QWERTY] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, XXX,
        XXX, TH_KC_PINKY, TH_KC_LRING, TH_KC_LMID, TH_KC_LIDX, TH_KC_LIN, TH_KC_RIN, TH_KC_RIDX, TH_KC_RMID, TH_KC_RRING, KC_SCLN, XXX,
        TH_KC_HESC, KC_Z, KC_X, KC_C, KC_V, KC_B, TH_KC_TNUM, TH_KC_TSYM, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, TH_KC_HBSP,
        XXX, XXX, OSM(MOD_LSFT), TH_KC_TNAV, TH_KC_TADJ, KC_DEL, XXX, XXX),

    [L_CANARY] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_Q, KC_L, KC_Y, KC_P, KC_B, KC_Z, KC_F, KC_O, KC_U, KC_SCLN, XXX,
        XXX, TH_KC_PINKY, TH_KC_LRING, TH_KC_LMID, TH_KC_LIDX, TH_KC_LIN, TH_KC_RIN, TH_KC_RIDX, TH_KC_RMID, TH_KC_RRING, KC_A, XXX,
        TH_KC_HESC, KC_W, KC_J, KC_V, KC_D, KC_K, TH_KC_TNUM, TH_KC_TSYM, KC_X, KC_H, KC_COMM, KC_DOT, KC_SLSH, TH_KC_HBSP,
        XXX, XXX, OSM(MOD_LSFT), TH_KC_TNAV, TH_KC_TADJ, KC_DEL, XXX, XXX),

    [L_NUM] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_WH_U, KC_BTN2, KC_MS_U, KC_BTN1, KC_BTN3, KC_7, KC_8, KC_9, KC_PMNS, KC_PSLS, XXX,
        XXX, ___, ___, ___, ___, ___, KC_4, KC_5, KC_6, KC_ASTR, KC_EQL, XXX,
        KC_WH_D, KC_MS_L, KC_MS_D, KC_MS_R, ___, ___, ___, KC_ENT, KC_1, KC_2, KC_3, KC_DOT, KC_0, KC_PPLS,
        XXX, XXX, ___, ___, KC_BSPC, KC_DEL, XXX, XXX),

    [L_NAV] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, OS_PGUP, KC_HOME, KC_UP, KC_END, TG(L_NAV), XXX,
        XXX, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, CMD_STAB_K, KC_LEFT, KC_DOWN, KC_RGHT, CMD_TAB_K, XXX,
        ___, KC_F11, KC_F12, OS_ZOUT, OS_ZIN, OS_ZRST, KC_SPC, KC_ENT, CTRL_TAB_K, KC_PGUP, OS_ADDCUR, KC_PGDN, OS_FWD, OSM(MOD_LGUI),
        XXX, XXX, ___, ___, KC_BSPC, KC_DEL, XXX, XXX),

    [L_SYM] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, KC_LCBR, KC_RCBR, KC_LABK, KC_RABK, KC_PIPE, KC_VOLU, KC_MUTE, C(A(G(KC_M))), KC_MPLY, KC_MNXT, XXX,
        XXX, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC, KC_BSLS, ___, ___, ___, ___, ___, XXX,
        KC_EQL, KC_PLUS, KC_MINS, KC_UNDS, KC_ASTR, KC_SLSH, ___, ___, KC_VOLD, LALT(LSFT(KC_RGHT)), OS_SELPART, OS_SELLINE, OS_BACK, OS_FWDBR,
        XXX, XXX, ___, ___, KC_BSPC, KC_DEL, XXX, XXX),

    [L_ADJ] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, QK_RBT, XXX, XXX, XXX, XXX, TO_BASE, TO_QWERTY, TO_CANARY, BRACE_K, TG(L_ADJ), XXX,
        XXX, QK_BOOT, XXX, XXX, XXX, XXX, TG(L_NUM), TG(L_NAV), TG(L_SYM), KC_GRV, KC_SLEP, XXX,
        ___, XXX, KC_BRIU, KC_BRID, REPEAT_K, KC_CAPS, KC_SPC, KC_ENT, KC_INS, KC_PSCR, ARROW_K, BRACE2_K, CW_TOGG, ___,
        XXX, XXX, ___, KC_TAB, ___, KC_DEL, XXX, XXX),

    [L_SYS] = LAYOUT(
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, CG_NORM, CG_SWAP, XXX, XXX, XXX, XXX, OS_SAVE, OS_FIND, XXX, XXX, XXX,
        XXX, QK_BOOT, QK_RBT, XXX, XXX, XXX, XXX, XXX, XXX, XXX, KC_SLEP, XXX,
        XXX, XXX, XXX, XXX, XXX, XXX, ___, ___, XXX, XXX, XXX, XXX, XXX, XXX,
        XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX),
#endif
};

static uint8_t tab_mod;
static uint8_t switch_mod;

static void tap_mod(uint16_t mod, uint16_t key) {
    uint16_t swapped = keycode_config(mod);
    register_code(swapped);
    tap_code(key);
    unregister_code(swapped);
}

static void tap_mod_shift(uint16_t mod, uint16_t key) {
    uint16_t swapped = keycode_config(mod);
    register_code(swapped);
    register_code(KC_LSFT);
    tap_code(key);
    unregister_code(KC_LSFT);
    unregister_code(swapped);
}

static void tab_hold(uint16_t mod, bool shift) {
    tab_mod = keycode_config(mod);
    register_code(tab_mod);
    if (shift) {
        tap_code16(S(KC_TAB));
    } else {
        tap_code(KC_TAB);
    }
}

static void tab_drop(void) {
    if (tab_mod) {
        unregister_code(tab_mod);
        tab_mod = 0;
    }
}

/* Drop shift while a brace or snippet is typed, then put held Shift back.
 * A one-shot shift is consumed. */
static void type_seq(const char *text, uint8_t lefts, uint8_t rights, bool home, bool select_left) {
    uint8_t mods = get_mods();
    uint8_t weak = get_weak_mods();
    uint8_t osm  = get_oneshot_mods();
    if (mods & MOD_MASK_SHIFT) {
        unregister_mods(MOD_MASK_SHIFT);
    }
    del_weak_mods(MOD_MASK_SHIFT);
    del_oneshot_mods(MOD_MASK_SHIFT);
    send_keyboard_report();
    send_string_P(text);
    if (home) {
        tap_code(KC_HOME);
    }
    for (uint8_t i = 0; i < rights; i++) {
        tap_code(KC_RGHT);
    }
    for (uint8_t i = 0; i < lefts; i++) {
        tap_code(KC_LEFT);
    }
    if (select_left) {
        tap_code16(A(S(KC_LEFT)));
    }
    set_mods(mods);
    set_weak_mods(weak);
    if (osm & ~MOD_MASK_SHIFT) {
        set_oneshot_mods(osm & ~MOD_MASK_SHIFT);
    }
    send_keyboard_report();
}

static void html_tag(void) {
    uint8_t mods = get_mods();
    uint8_t weak = get_weak_mods();
    uint8_t osm  = get_oneshot_mods();
    if (mods & MOD_MASK_SHIFT) {
        unregister_mods(MOD_MASK_SHIFT);
    }
    del_weak_mods(MOD_MASK_SHIFT);
    del_oneshot_mods(MOD_MASK_SHIFT);
    send_keyboard_report();
    send_string_P(PSTR("<TAG></TAG>"));
    tap_code(KC_ESC);
    for (uint8_t i = 0; i < 9; i++) {
        tap_code(KC_LEFT);
    }
    tap_mod(KC_LGUI, KC_D);
    tap_mod(KC_LGUI, KC_D);
    tap_code(KC_BSPC);
    set_mods(mods);
    set_weak_mods(weak);
    if (osm & ~MOD_MASK_SHIFT) {
        set_oneshot_mods(osm & ~MOD_MASK_SHIFT);
    }
    send_keyboard_report();
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (!pressed) {
        return;
    }
    switch (combo_index) {
        case CMB_UNDO:
            tap_mod(KC_LGUI, KC_Z);
            break;
        case CMB_REDO:
            if (is_mac()) {
                tap_code16(S(G(KC_Z)));
            } else {
                tap_code16(C(KC_Y));
            }
            break;
        case CMB_CUT:
            tap_mod(KC_LGUI, KC_X);
            break;
        case CMB_COPY:
            tap_mod(KC_LGUI, KC_C);
            break;
        case CMB_PASTE:
            tap_mod(KC_LGUI, KC_V);
            break;
        case CMB_CLIP:
            if (is_mac()) {
                tap_code16(A(S(G(KC_SPC))));
            } else {
                tap_code16(G(KC_V));
            }
            break;
        case CMB_PASTETXT:
            tap_mod_shift(KC_LGUI, KC_V);
            break;
        case CMB_SELALL:
            tap_mod(KC_LGUI, KC_A);
            break;
        case CMB_QUOTES:
            if ((get_mods() | get_oneshot_mods() | get_weak_mods()) & MOD_MASK_SHIFT) {
                type_seq(PSTR("'"), 0, 0, false, false);
            } else {
                type_seq(PSTR("\""), 0, 0, false, false);
            }
            break;
        case CMB_ARROW:
            type_seq(PSTR("() => {}"), 8, 0, false, false);
            break;
        case CMB_CONSOLE:
            type_seq(PSTR("console.log('')"), 8, 0, false, false);
            break;
        case CMB_HTML_P:
            type_seq(PSTR("<p></p>"), 4, 0, false, false);
            break;
        case CMB_HTML_TITLE:
            type_seq(PSTR("<title></title>"), 8, 0, false, false);
            break;
        case CMB_HTML_DIV:
            type_seq(PSTR("<div></div>"), 6, 0, false, false);
            break;
        case CMB_HTML_HTML:
            type_seq(PSTR("<html lang=\"en\"></html>"), 7, 0, false, false);
            break;
        case CMB_HTML_HEAD:
            type_seq(PSTR("<head></head>"), 7, 0, false, false);
            break;
        case CMB_HTML_BODY:
            type_seq(PSTR("<body></body>"), 7, 0, false, false);
            break;
        case CMB_HTML_FOOTER:
            type_seq(PSTR("<footer></footer>"), 9, 0, false, false);
            break;
        case CMB_HTML_HREF:
            type_seq(PSTR("<a href=\"link_goes_here\">name_of_link_goes_here</a>"), 0, 10, true, false);
            break;
        case CMB_HTML_IMG:
            type_seq(PSTR("<img src=\"image_source_or_link_goes_here\" alt=\"name_if_cant_load\" width=\"num_pixels\" height=\"num_pixels\">"), 0, 11, true, false);
            break;
        case CMB_HTML_STYLE:
            type_seq(PSTR("<style></style>"), 8, 0, false, false);
            break;
        case CMB_HTML_TAG:
            html_tag();
            break;
        case CMB_TERM_OPEN:
            tap_mod(KC_LCTL, KC_GRV);
            break;
        case CMB_TERM_NEXT:
            tap_mod_shift(KC_LGUI, KC_LBRC);
            break;
        case CMB_TERM_PREV:
            tap_mod_shift(KC_LGUI, KC_RBRC);
            break;
        case CMB_TERM_NEW:
            tap_mod_shift(KC_LCTL, KC_GRV);
            break;
        case CMB_NUM:
#ifdef CONVERT_TO_KB2040
            // L+U+Y latches Nav. A layer-tap release of Nav can clear it,
            // because QMK layers are not refcounted.
            layer_invert(L_NAV);
#else
            tap_code(KC_NUM);
#endif
            break;
    }
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    (void)combo;
    (void)keycode;
    (void)record;
    if (combo_index >= CMB_TERM_OPEN) {
#ifdef CONVERT_TO_KB2040
        return get_highest_layer(layer_state) == L_LOWER;
#else
        return get_highest_layer(layer_state) == L_NUM;
#endif
    }
#ifdef CONVERT_TO_KB2040
    if (combo_index == CMB_NUM) {
        return true;
    }
#endif
    return get_highest_layer(layer_state) <= L_CANARY;
}

uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    (void)combo;
    if (index == CMB_NUM || index == CMB_TERM_NEW) {
        return 70;
    }
    return 50;
}

void eeconfig_init_user(void) {
#if (EECONFIG_USER_DATA_SIZE) == 0
    eeconfig_update_user(0);
#endif
    keymap_config.raw            = eeconfig_read_keymap();
    keymap_config.swap_lctl_lgui = true;
    keymap_config.swap_rctl_rgui = true;
    eeconfig_update_keymap(keymap_config.raw);
}

layer_state_t layer_state_set_user(layer_state_t state) {
#ifdef CONVERT_TO_KB2040
    return update_tri_layer_state(state, L_LOWER, L_RAISE, L_SYS);
#else
    return update_tri_layer_state(state, L_NUM, L_SYM, L_SYS);
#endif
}

#ifdef CONVERT_TO_KB2040
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    switch (keycode) {
        case TH_ENT:
        case TH_SPC:
        case TH_TAB:
        case TH_BSPC:
        case TH_DEL:
            return 200;
        default:
            return TAPPING_TERM;
    }
}
#endif

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case KC_A ... KC_Z:
        case KC_MINS:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        case KC_UNDS:
        case KC_BSPC:
        case KC_DEL:
            return true;
        default:
            return false;
    }
}

void matrix_scan_user(void) {
    hold_tap_task();
    adaptive_matrix_scan();
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        tap_code(clockwise ? KC_VOLD : KC_VOLU);
    } else {
        tap_code(clockwise ? KC_WH_D : KC_WH_U);
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (hold_tap_process(keycode, record)) {
        return false;
    }
    if (record->event.pressed) {
        hold_tap_note(keycode);
    }
    if (!process_adaptive_key(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case OS_REDO:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(S(G(KC_Z)));
                } else {
                    tap_code16(C(KC_Y));
                }
            }
            return false;
        case OS_CLIP:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(A(S(G(KC_SPC))));
                } else {
                    tap_code16(G(KC_V));
                }
            }
            return false;
        case OS_FWD:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(G(KC_RGHT));
                } else {
                    tap_code16(A(KC_RGHT));
                }
            }
            return false;
        case OS_BACK:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(G(KC_LBRC));
                } else {
                    tap_code16(A(KC_LEFT));
                }
            }
            return false;
        case OS_FWDBR:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(G(KC_RBRC));
                } else {
                    tap_code16(A(KC_RGHT));
                }
            }
            return false;
        case OS_SAVE:
            if (record->event.pressed) {
                tap_mod(KC_LGUI, KC_S);
            }
            return false;
        case OS_FIND:
            if (record->event.pressed) {
                tap_mod(KC_LGUI, KC_F);
            }
            return false;
        case OS_ZOUT:
            if (record->event.pressed) {
                tap_mod(KC_LGUI, KC_MINS);
            }
            return false;
        case OS_ZIN:
            if (record->event.pressed) {
                tap_mod(KC_LGUI, KC_EQL);
            }
            return false;
        case OS_ZRST:
            if (record->event.pressed) {
                tap_mod(KC_LGUI, KC_0);
            }
            return false;
        case OS_ADDCUR:
            if (record->event.pressed) {
                uint16_t mod = keycode_config(KC_LGUI);
                register_code(mod);
                register_code(KC_LALT);
                tap_code(KC_DOWN);
                unregister_code(KC_LALT);
                unregister_code(mod);
            }
            return false;
        case OS_PGUP:
            if (record->event.pressed) {
                tap_mod(KC_LCTL, KC_PGUP);
            }
            return false;
        case OS_SELWORD:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(A(S(KC_RGHT)));
                } else {
                    tap_code16(C(S(KC_RGHT)));
                }
            }
            return false;
        case OS_SELLINE:
            if (record->event.pressed) {
                uint16_t mod = keycode_config(KC_LGUI);
                register_code(mod);
                tap_code(KC_LEFT);
                register_code(KC_LSFT);
                tap_code(KC_RGHT);
                unregister_code(KC_LSFT);
                unregister_code(mod);
            }
            return false;
        case OS_SELPART:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(A(C(S(KC_RGHT))));
                } else {
                    tap_code16(A(G(S(KC_RGHT))));
                }
            }
            return false;
        case CMD_TAB_K:
            if (record->event.pressed) {
                tab_hold(KC_LGUI, false);
            } else {
                tab_drop();
            }
            return false;
        case CMD_STAB_K:
            if (record->event.pressed) {
                tab_hold(KC_LGUI, true);
            } else {
                tab_drop();
            }
            return false;
        case CTRL_TAB_K:
            if (record->event.pressed) {
                tab_hold(KC_LCTL, false);
            } else {
                tab_drop();
            }
            return false;
        case OS_TAB_NEXT:
            // Raw Ctrl. keycode_config would turn this into Win+Tab.
            if (record->event.pressed) {
                tap_code16(C(KC_TAB));
            }
            return false;
        case OS_TAB_PREV:
            if (record->event.pressed) {
                tap_code16(C(S(KC_TAB)));
            }
            return false;
        case OS_APPS:
            if (record->event.pressed) {
                if (is_mac()) {
                    tap_code16(C(KC_DOWN));
                } else {
                    tap_code16(A(KC_GRV));
                }
            }
            return false;
        case OS_SWITCH:
            if (record->event.pressed) {
                switch_mod = is_mac() ? KC_LGUI : KC_LALT;
                register_code(switch_mod);
                tap_code(KC_TAB);
            } else if (switch_mod) {
                unregister_code(switch_mod);
                switch_mod = 0;
            }
            return false;
        case OS_ENT:
            if (record->event.pressed) {
                tap_mod(KC_LGUI, KC_ENT);
            }
            return false;
        case NEXT_SENTENCE:
            if (record->event.pressed) {
                tap_code(KC_DOT);
                tap_code(KC_SPC);
                add_oneshot_mods(MOD_LSFT);
            }
            return false;
        case BRACE_K:
            if (record->event.pressed) {
                if ((get_mods() | get_oneshot_mods() | get_weak_mods()) & MOD_MASK_SHIFT) {
                    type_seq(PSTR("<>"), 1, 0, false, false);
                } else {
                    type_seq(PSTR("()"), 1, 0, false, false);
                }
            }
            return false;
        case BRACE2_K:
            if (record->event.pressed) {
                if ((get_mods() | get_oneshot_mods() | get_weak_mods()) & MOD_MASK_SHIFT) {
                    type_seq(PSTR("{}"), 1, 0, false, false);
                } else {
                    type_seq(PSTR("[]"), 1, 0, false, false);
                }
            }
            return false;
        case ARROW_K:
            if (record->event.pressed) {
                if ((get_mods() | get_oneshot_mods() | get_weak_mods()) & MOD_MASK_SHIFT) {
                    type_seq(PSTR("const yourFuncVariable = (x,y) => x + y.method;"), 25, 0, false, true);
                } else {
                    type_seq(PSTR("() => "), 5, 0, false, false);
                }
            }
            return false;
        case TO_BASE:
            if (record->event.pressed) {
                set_single_persistent_default_layer(L_BASE);
            }
            return false;
        case TO_QWERTY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(L_QWERTY);
            }
            return false;
        case TO_CANARY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(L_CANARY);
            }
            return false;
        case REPEAT_K:
            if (record->event.pressed) {
                hold_tap_repeat();
            }
            return false;
    }

    if (record->event.pressed) {
        hold_tap_remember(keycode);
    }
    return true;
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    oled_write_P(PSTR("KB2040 "), false);
    oled_write_P(is_keyboard_left() ? PSTR("Left ") : PSTR("Right"), false);
    oled_write_ln_P(is_keyboard_master() ? PSTR(" USB") : PSTR(" link"), false);

    switch (get_highest_layer(default_layer_state)) {
        case L_QWERTY:
            oled_write_ln_P(PSTR("QWERTY"), false);
            break;
        case L_CANARY:
            oled_write_ln_P(PSTR("Canary"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Colemak"), false);
            break;
    }

    switch (get_highest_layer(layer_state)) {
        case L_LOWER:
            oled_write_P(PSTR("Lower "), false);
            break;
        case L_NAV:
            oled_write_P(PSTR("Nav "), false);
            break;
        case L_RAISE:
            oled_write_P(PSTR("Raise "), false);
            break;
        case L_SYS:
            oled_write_P(PSTR("System "), false);
            break;
        default:
            oled_write_P(PSTR("Base "), false);
            break;
    }
    oled_write_ln_P(is_mac() ? PSTR("Mac") : PSTR("Win"), false);
    return false;
}
#endif
