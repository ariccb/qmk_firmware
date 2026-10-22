#pragma once

#include "quantum.h"

/*
 * Mac when Ctrl/GUI are not swapped (System CG_NORM). Windows when they are
 * (System CG_SWAP). Home-row mods are written as on a Mac, and QMK's CG swap
 * retargets Cmd and Ctrl. Fresh EEPROM sets the swap so the board boots in
 * Windows mode.
 */
static inline bool is_mac(void) {
    return !keymap_config.swap_lctl_lgui;
}
