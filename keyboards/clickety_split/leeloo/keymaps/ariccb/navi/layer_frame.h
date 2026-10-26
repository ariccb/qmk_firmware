// Copyright 2021 Nicolas Druoton (druotoni)
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Layer indexes follow enum layer_names in ../keymap.c:
// Colemak-DH, QWERTY, Canary, Lower, Raise, Adjust.

#pragma once

#include "gui_state.h"

void render_gears(void);
void render_layer_frame(gui_state_t t);
void update_layer_frame(layer_state_t momentary, layer_state_t base_default);
