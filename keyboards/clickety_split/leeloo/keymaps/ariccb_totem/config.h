/* Copyright 2022 Clickety Split Ltd.
 * Totem-practice keymap. See README.md.
 */

#pragma once

#define MASTER_LEFT

// Combos are positional on the Colemak layer, including while QWERTY or
// Canary is the default layer. Alpha combos are gated in combo_should_trigger.
#define COMBO_ONLY_FROM_LAYER 0
#define COMBO_TERM 50
#define COMBO_TERM_PER_COMBO
#define COMBO_SHOULD_TRIGGER

#define ADAPTIVE_TERM 350
#define ONESHOT_TIMEOUT 1000

// Thumb layer-taps use 200 ms and the home-row mods use 280/320 ms.
// features/hold_tap.c owns those timers; the keyboard TAPPING_TERM is unused.

#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

#ifndef NO_DEBUG
#    define NO_DEBUG
#endif
#if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
#    define NO_PRINT
#endif

#define CAPS_WORD_IDLE_TIMEOUT 5000

// Same kinetic mouse as the Leeloo ariccb keymap.
#define MK_KINETIC_SPEED
#define MOUSEKEY_INTERVAL 8
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_MOVE_DELTA 16
#define MOUSEKEY_INITIAL_SPEED 100
#define MOUSEKEY_BASE_SPEED 1200
#define MOUSEKEY_DECELERATED_SPEED 400
#define MOUSEKEY_ACCELERATED_SPEED 3000
#define MOUSEKEY_WHEEL_INITIAL_MOVEMENTS 8
#define MOUSEKEY_WHEEL_BASE_MOVEMENTS 25
#define MOUSEKEY_WHEEL_ACCELERATED_MOVEMENTS 48
#define MOUSEKEY_WHEEL_DECELERATED_MOVEMENTS 8

#define ENCODER_RESOLUTIONS {2}
