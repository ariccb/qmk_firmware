#include "chatter_guard.h"

#include "quantum.h"

// Contact bounce longer than DEBOUNCE (5 ms) was a second press. S and W
// showed up as wordss / wordww. A deliberate same-finger double is slower.
#define CHATTER_MS 40

static const uint16_t chatter_keys[] = {KC_S, KC_W};
#define CHATTER_COUNT (sizeof(chatter_keys) / sizeof(chatter_keys[0]))

static uint16_t chatter_last[CHATTER_COUNT];
static bool     chatter_seen[CHATTER_COUNT];

bool chatter_should_drop(uint16_t keycode, uint16_t now) {
    for (uint8_t i = 0; i < CHATTER_COUNT; i++) {
        if (keycode != chatter_keys[i]) {
            continue;
        }
        if (chatter_seen[i] && TIMER_DIFF_16(now, chatter_last[i]) < CHATTER_MS) {
            chatter_last[i] = now;
            return true;
        }
        chatter_seen[i] = true;
        chatter_last[i] = now;
        return false;
    }
    return false;
}
