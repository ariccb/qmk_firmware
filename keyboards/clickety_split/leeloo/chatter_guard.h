#pragma once

#include <stdbool.h>
#include <stdint.h>

// Drop a repeat of a watched key inside CHATTER_MS. Add a letter to
// chatter_keys[] in chatter_guard.c. Each key has its own timer, so
// S then W inside the window still types both.
bool chatter_should_drop(uint16_t keycode, uint16_t now);
