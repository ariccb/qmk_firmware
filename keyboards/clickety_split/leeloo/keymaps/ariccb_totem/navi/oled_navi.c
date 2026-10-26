// HELL0 NAVI OLED for the Leeloo ariccb_totem KB2040 build.
//
// Animation by Nicolas Druoton (druotoni):
// https://github.com/druotoni/qmk_firmware/tree/master/keyboards/lily58/keymaps/druotoni
//
// Copied from the ariccb keymap so that build's layer names stay CDH/LOW/RSE/ADJ.
// One UF2: is_keyboard_left() picks the scope or the ring. The tri-layer
// update stays in keymap.c, which calls update_layer_frame.

#include QMK_KEYBOARD_H
#include "transactions.h"

#include "oled_navi.h"
#include "gui_state.h"
#include "boot.h"
#include "navi_logo.h"
#include "layer_frame.h"
#include "burst.h"
#include "ring.h"

typedef struct _sync_keycode_t {
    uint16_t keycode;
} sync_keycode_t;

static bool           navi_sync_need_send = false;
static sync_keycode_t navi_sync_keycode;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    (void)rotation;
    // 128x32 panel, drawn as 32 wide by 128 tall. Both halves use this
    // rotation; the right-hand art is mirrored in the renderer.
    return OLED_ROTATION_270;
}

static void render(gui_state_t t) {
    render_logo(t);

    if (is_keyboard_left()) {
        render_layer_frame(t);
        render_gears();
        decay_scope();
        render_scope(t);
    } else {
        render_circle(t);
    }
}

static void update(uint16_t keycode) {
    if (is_keyboard_left()) {
        update_scope();
    } else {
        update_circle(keycode);
    }
}

static void reset_side(void) {
    if (is_keyboard_left()) {
        reset_scope();
    } else {
        reset_ring();
    }
}

static void set_wackingup_mode_clean(void) {
    oled_clear();
    reset_side();
}

bool oled_task_user(void) {
    gui_state_t t = get_gui_state();

    if (t == _SLEEP) {
        oled_off();
        return false;
    }

    oled_on();

#ifdef WITH_BOOT
    if (t == _BOOTING) {
        bool boot_finished = render_boot();
        if (boot_finished) {
            set_wackingup_mode_clean();
            update_gui_state();
        }
        return false;
    }
#endif

    if (t == _HALTING) {
        render_halt();
        return false;
    }

    render(t);
    return false;
}

static void process_key(uint16_t keycode) {
    update(keycode);

    gui_state_t t = get_gui_state();

    if (t == _IDLE) {
        reset_side();
    }

    if (t == _BOOTING || t == _HALTING) {
        set_wackingup_mode_clean();
    }

    if (t == _SLEEP) {
        set_wackingup_mode_clean();
        reset_boot();
    }

    update_gui_state();
}

static void user_sync_a_slave_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    (void)in_buflen;
    (void)out_buflen;
    (void)out_data;
    const sync_keycode_t *m2s = (const sync_keycode_t *)in_data;
    process_key(m2s->keycode);
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_SYNC_A, user_sync_a_slave_handler);
    update_layer_frame(layer_state, default_layer_state);
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) {
        return;
    }
    if (!navi_sync_need_send) {
        return;
    }
    if (transaction_rpc_send(USER_SYNC_A, sizeof(navi_sync_keycode), &navi_sync_keycode)) {
        navi_sync_need_send = false;
    }
}

void navi_process_record(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return;
    }
    navi_sync_keycode.keycode = keycode;
    navi_sync_need_send       = true;
    process_key(keycode);
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    update_layer_frame(layer_state, state);
    return state;
}
