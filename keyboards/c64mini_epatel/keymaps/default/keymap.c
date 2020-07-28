/* Copyright 2020 Edward Patel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
    _SHIFT
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    RSHIFT = SAFE_RANGE,
    LSHIFT,
    CAPSLOCK
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
    //  0          1          2         3         4     5     6     7         8
/* 0 */ KC_BSPC,   KC_BSLS,   KC_PLUS,  KC_9,     KC_7, KC_5, KC_3, KC_1,
/* 1 */ KC_F1,     KC_HOME,   KC_MINUS, KC_0,     KC_8, KC_6, KC_4, KC_2,     KC_GRV,
/* 2 */ KC_TAB,    KC_ASTR,   KC_P,     KC_I,     KC_Y, KC_R, KC_W, KC_LCTL,
/* 3 */ KC_F3,     KC_CIRC,   KC_AT,    KC_O,     KC_U, KC_T, KC_E, KC_Q,
/* 4 */ KC_ENTER,  KC_SCLN,   KC_L,     KC_J,     KC_G, KC_D, KC_A, KC_ESC,
/* 5 */ KC_F5,     KC_EQL,    KC_COLN,  KC_K,     KC_H, KC_F, KC_S, CAPSLOCK,
/* 6 */ KC_RIGHT,  RSHIFT,    KC_DOT,   KC_M,     KC_B, KC_C, KC_Z, KC_LGUI,
/* 7 */ KC_F7,     KC_DOWN,   KC_SLASH, KC_COMMA, KC_N, KC_V, KC_X, LSHIFT,   KC_SPACE
    ),
    [_SHIFT] = LAYOUT(
    //  0          1           2         3         4        5        6        7          8
/* 0 */ KC_INS,    KC_BSLS,    KC_PLUS,  KC_RPRN,  KC_QUOT, KC_PERC, KC_HASH, KC_EXLM,
/* 1 */ KC_F2,     S(KC_HOME), KC_MINUS, KC_0,     KC_LPRN, KC_AMPR, KC_DLR,  KC_DQT,    KC_GRV,
/* 2 */ KC_TAB,    KC_ASTR,    S(KC_P),  S(KC_I),  S(KC_Y), S(KC_R), S(KC_W), KC_LCTL,
/* 3 */ KC_F4,     KC_CIRC,    KC_AT,    S(KC_O),  S(KC_U), S(KC_T), S(KC_E), S(KC_Q),
/* 4 */ KC_ENTER,  KC_RBRC,    S(KC_L),  S(KC_J),  S(KC_G), S(KC_D), S(KC_A), S(KC_ESC),
/* 5 */ KC_F6,     KC_EQL,     KC_LBRC,  S(KC_K),  S(KC_H), S(KC_F), S(KC_S), CAPSLOCK,
/* 6 */ KC_LEFT,   RSHIFT,     KC_GT,    S(KC_M),  S(KC_B), S(KC_C), S(KC_Z), KC_LGUI,
/* 7 */ KC_F8,     KC_UP,      KC_QUES,  KC_LT,    S(KC_N), S(KC_V), S(KC_X), LSHIFT,    KC_SPACE
    )
};

static uint8_t shiftKeys = 0;
static uint8_t previousShiftKeys = 0;
static int8_t capslockState = 0;
static int8_t c64KeyDown = 0;
static int8_t shiftRegistered = 0;

void checkCaseChange(void) {
    if (shiftKeys && c64KeyDown) {
        if (!shiftRegistered) {
            register_code(KC_LSHIFT);
            shiftRegistered = true;
        }
    }
}

void checkShiftKeys(void) {
    if (shiftKeys && !previousShiftKeys) {
        set_single_persistent_default_layer(_SHIFT);
        checkCaseChange();
    }
    if (!shiftKeys && previousShiftKeys) {
        set_single_persistent_default_layer(_BASE);
    }
    previousShiftKeys = shiftKeys;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (shiftRegistered) {
        unregister_code(KC_LSHIFT);
        shiftRegistered = false;
    }
    switch (keycode) {
        case KC_LGUI:
            c64KeyDown = record->event.pressed;
            checkCaseChange();
            break;
        case LSHIFT:
            if (record->event.pressed) {
                shiftKeys = shiftKeys | 0x01;
            } else {
                shiftKeys = shiftKeys & ~0x01;
            }
            checkShiftKeys();
            return false;
            break;
        case RSHIFT:
            if (record->event.pressed) {
                shiftKeys = shiftKeys | 0x02;
            } else {
                shiftKeys = shiftKeys & ~0x02;
            }
            checkShiftKeys();
            return false;
            break;
        case CAPSLOCK:
            switch (capslockState) {
                case 0:
                    if (record->event.pressed) {
                        capslockState = 1;
                        shiftKeys = shiftKeys | 0x04;
                        checkShiftKeys();
                    }
                    break;
                case 1:
                    if (!record->event.pressed) {
                        capslockState = 2;
                    }
                    break;
                case 2:
                    if (record->event.pressed) {
                        capslockState = 3;
                    }
                    break;
                case 3:
                    if (!record->event.pressed) {
                        capslockState = 0;
                        shiftKeys = shiftKeys & ~0x04;
                        checkShiftKeys();
                    }
                    break;
            }
            return false;
            break;
    }
    return true;
}
