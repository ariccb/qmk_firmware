// Copyright 2021 Nicolas Druoton (druotoni)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include QMK_KEYBOARD_H

// Record a key for the left scope and the right ring. Safe to call on
// every process_record_user entry; releases are ignored.
void navi_process_record(uint16_t keycode, keyrecord_t *record);
