#pragma once

#include "quantum.h"

// Layer numbers shared with keymap.c. SYS sits above NUM and SYM so the
// tri-layer covers both while the inner thumbs are held.
enum totem_layers {
    L_BASE = 0,
    L_QWERTY,
    L_CANARY,
    L_NUM,
    L_NAV,
    L_SYM,
    L_ADJ,
    L_SYS
};

// Contiguous from TH_KC_PINKY. hold_tap.c maps keycode - TH_KC_PINKY to the id.
enum totem_keycodes {
    TH_KC_PINKY = SAFE_RANGE,
    TH_KC_LRING,
    TH_KC_LMID,
    TH_KC_LIDX,
    TH_KC_LIN,
    TH_KC_RIN,
    TH_KC_RIDX,
    TH_KC_RMID,
    TH_KC_RRING,
    TH_KC_HESC,
    TH_KC_HBSP,
    TH_KC_TNAV,
    TH_KC_TNUM,
    TH_KC_TSYM,
    TH_KC_TADJ,
    OS_REDO,
    OS_CLIP,
    OS_FWD,
    OS_BACK,
    OS_FWDBR,
    OS_SAVE,
    OS_FIND,
    OS_ZOUT,
    OS_ZIN,
    OS_ZRST,
    OS_ADDCUR,
    OS_PGUP,
    OS_SELWORD,
    OS_SELLINE,
    OS_SELPART,
    CMD_TAB_K,
    CMD_STAB_K,
    CTRL_TAB_K,
    BRACE_K,
    BRACE2_K,
    ARROW_K,
    TO_BASE,
    TO_QWERTY,
    TO_CANARY,
    REPEAT_K
};

#define TH_COUNT (TH_KC_TADJ - TH_KC_PINKY + 1)

// Returns true when the event was consumed.
bool hold_tap_process(uint16_t keycode, keyrecord_t *record);
void hold_tap_task(void);
void hold_tap_note(uint16_t keycode);
void hold_tap_remember(uint16_t keycode);
void hold_tap_repeat(void);
