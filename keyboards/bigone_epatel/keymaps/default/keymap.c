#include <stdio.h>

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE
};

enum custom_keycodes {
    THEONE = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        THEONE
    )
};

void keyboard_pre_init_user(void) {
    setPinInputHigh(F6); // 1
    setPinInputHigh(F7); // 2
    setPinInputHigh(B1); // 3
    setPinInputHigh(B3); // 4
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static const char * const output[] = {
        "\n", // 0
        " ", // 1
        "Hello, world!\n", // 2
        "edward.patel@memention.com", // 3 - edward.patel@memention.com
        "4", // 4
        "5", // 5
        "6", // 6
        "7", // 7
        "8", // 8
        "9", // 9
        "10", // 10
        "11", // 11
        "", // 12 - yes/no
        "", // 13 - timer between
        "", // 14 - timer pressed
        "", // 15 - counter
    };

    static uint32_t timer = 0;
    static int counter = 1;

    int c = (readPin(B3) ? 0x01 : 0) |
            (readPin(B1) ? 0x02 : 0) |
            (readPin(F7) ? 0x04 : 0) |
            (readPin(F6) ? 0x08 : 0);

    char tmp[20];

    switch (keycode) {
        case THEONE:
            if (record->event.pressed) {
                // when key is pressed
                if (c == 15) {
                    timer = timer_read32();
                } else if (c == 14) {
                    timer = timer_read32();
                } else if (c == 13) {
                    if (timer == 0) {
                        timer = timer_read32();
                    } else {
                        uint32_t e = timer_elapsed32(timer);
                        int s = e/1000;
                        int ms = (e%1000)/10;
                        sprintf(tmp, "%d.%02d\n", s, ms);
                        send_string(tmp);
                        timer = 0;
                    }
                } else if (c == 12) {
                    uint16_t t = timer_read();
                    t = t * 757 + 263;
                    if (t & 0x04) {
                        send_string("yes\n");
                    } else {
                        send_string("no\n");
                    }
                } else {
                    send_string(output[c]);
                }
            } else {
                // when key is released
                if (c == 15) {
                    if (timer_elapsed32(timer) > 2000) {
                        counter = 0;
                    }
                    sprintf(tmp, "%d\n", counter++);
                    send_string(tmp);
                } else if (c == 14) {
                    uint32_t e = timer_elapsed32(timer);
                    int s = e/1000;
                    int ms = (e%1000)/10;
                    sprintf(tmp, "%d.%02d\n", s, ms);
                    send_string(tmp);
                }
            }
            break;
    }
    return true;
}
