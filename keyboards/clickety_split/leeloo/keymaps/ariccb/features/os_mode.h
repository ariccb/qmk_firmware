#pragma once

#include "quantum.h"

/*
 * Mac when Ctrl/GUI are not swapped (Raise CG_NORM). Windows when they are
 * swapped (Raise CG_SWAP). Base-layer mods are written as on a Mac, and QMK's
 * CG swap retargets those Cmd and Ctrl holds. Fresh EEPROM sets the swap so
 * the board boots in Windows mode. See the note at the top of keymap.c.
 */
static inline bool is_mac(void) {
    return !keymap_config.swap_lctl_lgui;
}
