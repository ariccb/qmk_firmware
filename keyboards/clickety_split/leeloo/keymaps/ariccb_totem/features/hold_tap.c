#include "hold_tap.h"
#include "adaptive_keys.h"

#include <string.h>

// QMK 0.19 has no chordal hold or flow tap. This is the Totem timeless
// hold-tap: require-prior-idle, opposite-hand triggers, balanced decision
// on the other key's release, and hold-trigger-on-release so two mods on
// the same hand can both be held. a_nav (left pinky) decides on the other
// hand's press so a Nav arrow can repeat. Outer pinkies are tap-preferred.

enum {
    PH_IDLE = 0,
    PH_PENDING,
    PH_HOLDING,
    PH_TAPPING,
    PH_GHOST
};

enum {
    F_POS = 1 << 0,
    F_REL = 1 << 1,
    F_PRESS = 1 << 2,
    F_HYPER = 1 << 3,
    F_LEFT = 1 << 4,
    F_TAPPREF = 1 << 5
};

#define NO_LAYER 0xFF
#define QMAX 16

typedef struct {
    uint16_t term;
    uint16_t prior;
    uint16_t quick;
    uint16_t tap;
    uint8_t  flags;
    uint8_t  mod;
    uint8_t  layer;
} th_cfg_t;

typedef struct {
    uint8_t  phase;
    bool     down;
    bool     was_tap;
    uint16_t t0;
    uint16_t tap_kc;
    uint16_t hold_kc;
    keypos_t pos;
} th_state_t;

typedef struct {
    keypos_t pos;
    uint16_t keycode;
    uint16_t time;
    bool     pressed;
    bool     force_tap;
} qev_t;

typedef struct {
    uint8_t  row;
    uint8_t  col;
    uint16_t kc;
    bool     on;
} cache_t;

static const th_cfg_t cfg[TH_COUNT] = {
    /* pinky A/Nav: hold on the other hand's press */
    {280, 150, 175, KC_NO, F_POS | F_PRESS | F_LEFT, 0, L_NAV},
    /* left Alt, Shift, Ctrl */
    {280, 150, 175, KC_NO, F_POS | F_REL | F_LEFT, MOD_LALT, NO_LAYER},
    {280, 150, 175, KC_NO, F_POS | F_REL | F_LEFT, MOD_LSFT, NO_LAYER},
    {280, 150, 175, KC_NO, F_POS | F_REL | F_LEFT, MOD_LCTL, NO_LAYER},
    /* left inner Cmd, stricter */
    {320, 200, 200, KC_NO, F_POS | F_REL | F_LEFT, MOD_LGUI, NO_LAYER},
    /* right inner Cmd, then Ctrl, Shift, Alt */
    {320, 200, 200, KC_NO, F_POS | F_REL, MOD_RGUI, NO_LAYER},
    {280, 150, 175, KC_NO, F_POS | F_REL, MOD_RCTL, NO_LAYER},
    {280, 150, 175, KC_NO, F_POS | F_REL, MOD_RSFT, NO_LAYER},
    {280, 150, 175, KC_NO, F_POS | F_REL, MOD_RALT, NO_LAYER},
    /* outer pinkies: tap Esc / Backspace, hold Hyper */
    {200, 150, 0, KC_ESC, F_HYPER | F_TAPPREF | F_LEFT, 0, NO_LAYER},
    {200, 150, 0, KC_BSPC, F_HYPER | F_TAPPREF, 0, NO_LAYER},
    /* thumbs: balanced, no prior-idle, no hand restriction */
    {200, 0, 120, KC_TAB, 0, 0, L_NAV},
    {200, 0, 120, KC_SPC, 0, 0, L_NUM},
    {200, 0, 120, KC_ENT, 0, 0, L_SYM},
    {200, 0, 120, KC_BSPC, 0, 0, L_ADJ},
};

/* Colemak, QWERTY, Canary tap letters for ids 0..8. Right pinky is plain. */
static const uint8_t letters[3][9] = {
    {KC_A, KC_R, KC_S, KC_T, KC_G, KC_M, KC_N, KC_E, KC_I},
    {KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L},
    {KC_C, KC_R, KC_S, KC_T, KC_G, KC_M, KC_N, KC_E, KC_I},
};

static th_state_t st[TH_COUNT];
static qev_t      queue[QMAX];
static uint8_t    qn;
static cache_t    sent[8];
static bool       pumping;
static bool       have_typing;
static uint16_t   last_typing;
static uint16_t   remembered;

static bool is_th(uint16_t keycode) {
    return keycode >= TH_KC_PINKY && keycode <= TH_KC_TADJ;
}

static uint8_t th_id(uint16_t keycode) {
    return keycode - TH_KC_PINKY;
}

static bool same_pos(keypos_t a, keypos_t b) {
    return a.row == b.row && a.col == b.col;
}

static bool is_typing_kc(uint16_t keycode) {
    return keycode >= KC_A && keycode <= KC_EXSEL;
}

static uint16_t mod_to_kc(uint8_t mod) {
    switch (mod) {
        case MOD_LCTL:
            return KC_LCTL;
        case MOD_LSFT:
            return KC_LSFT;
        case MOD_LALT:
            return KC_LALT;
        case MOD_LGUI:
            return KC_LGUI;
        case MOD_RCTL:
            return KC_RCTL;
        case MOD_RSFT:
            return KC_RSFT;
        case MOD_RALT:
            return KC_RALT;
        case MOD_RGUI:
            return KC_RGUI;
    }
    return KC_NO;
}

static uint16_t tap_keycode(uint8_t id) {
    if (cfg[id].tap) {
        return cfg[id].tap;
    }
    uint8_t layer = get_highest_layer(default_layer_state);
    if (layer > L_CANARY) {
        layer = L_BASE;
    }
    return letters[layer][id];
}

/* Opposite hand, including that hand's thumbs (rows 0-4 left, 5-9 right). */
static bool is_trigger(uint8_t id, keypos_t pos) {
    if (!(cfg[id].flags & F_POS)) {
        return true;
    }
    bool key_left  = cfg[id].flags & F_LEFT;
    bool other_left = pos.row < 5;
    return key_left != other_left;
}

static void cache_set(keypos_t pos, uint16_t keycode) {
    for (uint8_t i = 0; i < 8; i++) {
        if (sent[i].on && sent[i].row == pos.row && sent[i].col == pos.col) {
            sent[i].kc = keycode;
            return;
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (!sent[i].on) {
            sent[i].on  = true;
            sent[i].row = pos.row;
            sent[i].col = pos.col;
            sent[i].kc  = keycode;
            return;
        }
    }
    sent[0].on  = true;
    sent[0].row = pos.row;
    sent[0].col = pos.col;
    sent[0].kc  = keycode;
}

static uint16_t cache_take(keypos_t pos) {
    for (uint8_t i = 0; i < 8; i++) {
        if (sent[i].on && sent[i].row == pos.row && sent[i].col == pos.col) {
            sent[i].on = false;
            return sent[i].kc;
        }
    }
    return KC_NO;
}

static int find_release(uint8_t id) {
    for (uint8_t i = 0; i < qn; i++) {
        if (!queue[i].pressed && same_pos(queue[i].pos, st[id].pos)) {
            return i;
        }
    }
    return -1;
}

static void q_remove(uint8_t index) {
    if (index >= qn) {
        return;
    }
    memmove(&queue[index], &queue[index + 1], (qn - index - 1) * sizeof(qev_t));
    qn--;
}

static bool any_pending(void) {
    for (uint8_t id = 0; id < TH_COUNT; id++) {
        if (st[id].phase == PH_PENDING) {
            return true;
        }
    }
    return false;
}

void hold_tap_note(uint16_t keycode) {
    if (is_typing_kc(keycode)) {
        last_typing = timer_read();
        have_typing = true;
    }
}

void hold_tap_remember(uint16_t keycode) {
    if (is_typing_kc(keycode)) {
        remembered = keycode;
    }
}

void hold_tap_repeat(void) {
    if (remembered) {
        tap_code16(remembered);
    }
}

// S is a home-row hold-tap, so a bounce never arrives as KC_S in
// process_record_user. Drop a second tap inside 40 ms. Intentional ss
// (pass, miss) is a slower second stroke. Adaptive rules that replace
// some other letter with one S do not come through here as the S key.
static uint16_t last_s_tap;
static bool     have_s_tap;

static void emit_tap(uint8_t id) {
    uint16_t kc = tap_keycode(id);
    if (kc == KC_S) {
        uint16_t now = timer_read();
        if (have_s_tap && TIMER_DIFF_16(now, last_s_tap) < 40) {
            last_s_tap   = now;
            st[id].phase = st[id].down ? PH_GHOST : PH_IDLE;
            return;
        }
        have_s_tap = true;
        last_s_tap = now;
    }
    st[id].was_tap = true;
    st[id].tap_kc  = kc;

    keyrecord_t rec = {0};
    rec.event.pressed = true;
    rec.event.key     = st[id].pos;
    rec.event.time    = timer_read() | 1;
    rec.keycode       = kc;

    process_caps_word(kc, &rec);
    if (!process_adaptive_key(kc, &rec)) {
        clear_weak_mods();
        hold_tap_note(kc);
        st[id].phase = st[id].down ? PH_GHOST : PH_IDLE;
        return;
    }

    if (st[id].down) {
        register_code(kc);
        st[id].phase = PH_TAPPING;
    } else {
        tap_code(kc);
        clear_weak_mods();
        st[id].phase = PH_IDLE;
    }
    hold_tap_note(kc);
    hold_tap_remember(kc);
}

static void apply_hold(uint8_t id) {
    st[id].phase   = PH_HOLDING;
    st[id].was_tap = false;
    st[id].hold_kc = KC_NO;
    if (cfg[id].flags & F_HYPER) {
        register_mods(MOD_HYPR);
        send_keyboard_report();
    } else if (cfg[id].layer != NO_LAYER) {
        layer_on(cfg[id].layer);
    } else {
        uint16_t kc    = keycode_config(mod_to_kc(cfg[id].mod));
        st[id].hold_kc = kc;
        register_code(kc);
    }
}

static void release_th(uint8_t id) {
    st[id].down = false;
    switch (st[id].phase) {
        case PH_PENDING:
            emit_tap(id);
            break;
        case PH_TAPPING:
            unregister_code(st[id].tap_kc);
            clear_weak_mods();
            st[id].phase = PH_IDLE;
            break;
        case PH_GHOST:
            st[id].phase = PH_IDLE;
            break;
        case PH_HOLDING:
            if (cfg[id].flags & F_HYPER) {
                unregister_mods(MOD_HYPR);
                send_keyboard_report();
            } else if (cfg[id].layer != NO_LAYER) {
                layer_off(cfg[id].layer);
            } else if (st[id].hold_kc) {
                unregister_code(st[id].hold_kc);
            }
            st[id].phase = PH_IDLE;
            break;
        default:
            break;
    }
}

/* 1 hold, -1 tap, 0 still waiting. Own release wins, then the tapping term. */
static int decide(uint8_t id) {
    if (find_release(id) >= 0) {
        st[id].down = false;
        return -1;
    }
    if (st[id].down && timer_elapsed(st[id].t0) >= cfg[id].term) {
        return 1;
    }
    if (cfg[id].flags & F_TAPPREF) {
        return 0;
    }

    bool saw_trig = false;
    bool trig_open = false;
    bool non_press = false;
    bool non_released_before_trig = false;

    for (uint8_t i = 0; i < qn; i++) {
        if (!queue[i].pressed || same_pos(queue[i].pos, st[id].pos)) {
            continue;
        }
        bool released = false;
        for (uint8_t j = i + 1; j < qn; j++) {
            if (!queue[j].pressed && same_pos(queue[j].pos, queue[i].pos)) {
                released = true;
                break;
            }
        }
        if (is_trigger(id, queue[i].pos)) {
            saw_trig = true;
            if (!released) {
                trig_open = true;
            }
        } else {
            non_press = true;
            if (released && !saw_trig) {
                non_released_before_trig = true;
            }
        }
    }

    uint8_t flags = cfg[id].flags;
    if (flags & F_POS) {
        if (!(flags & F_REL)) {
            if ((flags & F_PRESS) && saw_trig) {
                return 1;
            }
            if (non_press) {
                return -1;
            }
            if (saw_trig && !trig_open) {
                return 1;
            }
            return 0;
        }
        if (non_released_before_trig && !saw_trig) {
            return -1;
        }
        if (saw_trig && !trig_open) {
            return 1;
        }
        return 0;
    }
    if (saw_trig && !trig_open) {
        return 1;
    }
    return 0;
}

/* Captured at the press, so a queued key keeps the prior-idle / quick-tap
 * decision even if it is dispatched after the window has elapsed. */
static bool should_force(uint8_t id) {
    bool quick = st[id].was_tap && cfg[id].quick && timer_elapsed(st[id].t0) < cfg[id].quick;
    bool idle  = cfg[id].prior && have_typing && timer_elapsed(last_typing) < cfg[id].prior;
    return quick || idle;
}

static void begin_th(uint8_t id, keypos_t pos, uint16_t time, bool force) {
    if (!force) {
        force = should_force(id);
    }
    st[id].t0   = time ? time : (timer_read() | 1);
    st[id].pos  = pos;
    st[id].down = true;
    if (force) {
        emit_tap(id);
        return;
    }
    st[id].phase = PH_PENDING;
}

static bool release_queued(keypos_t pos) {
    for (uint8_t i = 0; i < qn; i++) {
        if (!queue[i].pressed && same_pos(queue[i].pos, pos)) {
            return true;
        }
    }
    return false;
}

static void replay_record(keypos_t pos, uint16_t keycode, uint16_t time, bool pressed, bool oneshot_tap) {
    keyrecord_t rec = {0};
    rec.event.pressed = pressed;
    rec.event.key     = pos;
    rec.event.time    = time ? time : 1;
    rec.keycode       = keycode;
    /* A finished tap of OSM should stick. A key that is still held is a
     * real modifier until its later release. tap.count is 0 otherwise. */
    if (oneshot_tap) {
        rec.tap.count = 1;
    }
    process_record(&rec);
}

static void dispatch(qev_t ev) {
    /* Queued presses were not decided yet, so they see the layer a hold
     * just turned on. A hold-tap that is already pending keeps the id it
     * had when it was pressed. */
    if (ev.pressed) {
        uint16_t now = keymap_key_to_keycode(layer_switch_get_layer(ev.pos), ev.pos);
        if (is_th(now)) {
            bool force = is_th(ev.keycode) && now == ev.keycode && ev.force_tap;
            begin_th(th_id(now), ev.pos, ev.time, force);
            return;
        }
        cache_set(ev.pos, now);
        bool sticky = now >= QK_ONE_SHOT_MOD && now <= QK_ONE_SHOT_MOD_MAX && release_queued(ev.pos);
        replay_record(ev.pos, now, ev.time, true, sticky);
        return;
    }

    if (is_th(ev.keycode)) {
        uint8_t id = th_id(ev.keycode);
        if (st[id].phase != PH_IDLE && same_pos(st[id].pos, ev.pos)) {
            release_th(id);
            return;
        }
    }
    uint16_t keycode = cache_take(ev.pos);
    if (!keycode) {
        keycode = ev.keycode;
    }
    bool sticky = keycode >= QK_ONE_SHOT_MOD && keycode <= QK_ONE_SHOT_MOD_MAX;
    replay_record(ev.pos, keycode, ev.time, false, sticky);
}

static void enqueue(uint16_t keycode, keyrecord_t *record) {
    if (qn >= QMAX) {
        bool was = pumping;
        pumping  = true;
        for (uint8_t id = 0; id < TH_COUNT; id++) {
            if (st[id].phase == PH_PENDING) {
                apply_hold(id);
            }
        }
        while (qn) {
            qev_t ev = queue[0];
            q_remove(0);
            dispatch(ev);
        }
        pumping = was;
    }
    if (qn >= QMAX) {
        return;
    }
    queue[qn].pos       = record->event.key;
    queue[qn].keycode   = keycode;
    queue[qn].time      = record->event.time ? record->event.time : 1;
    queue[qn].pressed   = record->event.pressed;
    queue[qn].force_tap = record->event.pressed && is_th(keycode) && should_force(th_id(keycode));
    qn++;
}

static void pump(void) {
    if (pumping) {
        return;
    }
    pumping = true;
    while (qn) {
        for (uint8_t id = 0; id < TH_COUNT; id++) {
            if (st[id].phase != PH_PENDING) {
                continue;
            }
            int decision = decide(id);
            if (!decision) {
                continue;
            }
            int rel = find_release(id);
            if (rel >= 0) {
                st[id].down = false;
                q_remove(rel);
            }
            if (decision < 0) {
                emit_tap(id);
            } else {
                apply_hold(id);
            }
        }
        if (any_pending() || !qn) {
            break;
        }
        qev_t ev = queue[0];
        q_remove(0);
        dispatch(ev);
    }
    pumping = false;
}

void hold_tap_task(void) {
    if (any_pending() || qn) {
        pump();
    }
}

bool hold_tap_process(uint16_t keycode, keyrecord_t *record) {
    if (pumping) {
        return false;
    }

    if (!record->event.pressed && !is_th(keycode)) {
        uint16_t cached = cache_take(record->event.key);
        if (cached) {
            record->keycode = cached;
        }
    }

    if (any_pending() || qn) {
        enqueue(keycode, record);
        pump();
        return true;
    }

    if (is_th(keycode)) {
        uint8_t id = th_id(keycode);
        if (record->event.pressed) {
            begin_th(id, record->event.key, record->event.time, false);
        } else {
            release_th(id);
        }
        return true;
    }
    return false;
}
