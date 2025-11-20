#pragma once

#include <stdbool.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define SSP_KEYCODES(X) \
    X(SSP_KEYCODE_UNDEFINED, "Undefined") \
    \
    X(SSP_KEYCODE_A, "A") \
    X(SSP_KEYCODE_B, "B") \
    X(SSP_KEYCODE_C, "C") \
    X(SSP_KEYCODE_D, "D") \
    X(SSP_KEYCODE_E, "E") \
    X(SSP_KEYCODE_F, "F") \
    X(SSP_KEYCODE_G, "G") \
    X(SSP_KEYCODE_H, "H") \
    X(SSP_KEYCODE_I, "I") \
    X(SSP_KEYCODE_J, "J") \
    X(SSP_KEYCODE_K, "K") \
    X(SSP_KEYCODE_L, "L") \
    X(SSP_KEYCODE_M, "M") \
    X(SSP_KEYCODE_N, "N") \
    X(SSP_KEYCODE_O, "O") \
    X(SSP_KEYCODE_P, "P") \
    X(SSP_KEYCODE_Q, "Q") \
    X(SSP_KEYCODE_R, "R") \
    X(SSP_KEYCODE_S, "S") \
    X(SSP_KEYCODE_T, "T") \
    X(SSP_KEYCODE_U, "U") \
    X(SSP_KEYCODE_V, "V") \
    X(SSP_KEYCODE_W, "W") \
    X(SSP_KEYCODE_X, "X") \
    X(SSP_KEYCODE_Y, "Y") \
    X(SSP_KEYCODE_Z, "Z") \
    \
    X(SSP_KEYCODE_LEFT_SHIFT, "left shift") \
    X(SSP_KEYCODE_RIGHT_SHIFT, "right shift") \
    X(SSP_KEYCODE_ENTER, "enter") \
    X(SSP_KEYCODE_LEFT_CTRL, "left ctrl") \
    X(SSP_KEYCODE_RIGHT_CTRL, "right ctrl") \
    X(SSP_KEYCODE_CAPS_LOCK, "caps lock") \
    X(SSP_KEYCODE_TAB, "tab") \
    X(SSP_KEYCODE_BACKSPACE, "backspace") \
    X(SSP_KEYCODE_SPACE, "space") \
    X(SSP_KEYCODE_ALT, "alt") \
    X(SSP_KEYCODE_ALT_GR, "alt gr") \
    \
    X(SSP_KEYCODE_LEFT_ARROW, "left arrow") \
    X(SSP_KEYCODE_RIGHT_ARROW, "right arrow") \
    X(SSP_KEYCODE_UP_ARROW, "up arrow") \
    X(SSP_KEYCODE_DOWN_ARROW, "down arrow") \
    \
    X(SSP_KEYCODE_1, "1") \
    X(SSP_KEYCODE_2, "2") \
    X(SSP_KEYCODE_3, "3") \
    X(SSP_KEYCODE_4, "4") \
    X(SSP_KEYCODE_5, "5") \
    X(SSP_KEYCODE_6, "6") \
    X(SSP_KEYCODE_7, "7") \
    X(SSP_KEYCODE_8, "8") \
    X(SSP_KEYCODE_9, "9") \
    X(SSP_KEYCODE_0, "0") \
    \
    X(SSP_KEYCODE_PAD_0, "keypad 0") \
    X(SSP_KEYCODE_PAD_1, "keypad 1") \
    X(SSP_KEYCODE_PAD_2, "keypad 2") \
    X(SSP_KEYCODE_PAD_3, "keypad 3") \
    X(SSP_KEYCODE_PAD_4, "keypad 4") \
    X(SSP_KEYCODE_PAD_5, "keypad 5") \
    X(SSP_KEYCODE_PAD_6, "keypad 6") \
    X(SSP_KEYCODE_PAD_7, "keypad 7") \
    X(SSP_KEYCODE_PAD_8, "keypad 8") \
    X(SSP_KEYCODE_PAD_9, "keypad 9")


enum SSP_KEYCODE {
    #define X(keycode, str) keycode,
    SSP_KEYCODES(X)
    #undef X

    SSP_KEYCODE_COUNT
};

extern const char *SSP_KEYCODE_STR[];

enum SSP_KEYBOARD_ACTION {
    SSP_KEYBOARD_ACTION_RELEASE = 0,
    SSP_KEYBOARD_ACTION_PRESS
};

struct SSPInputManager {
    bool have_keyboard;
    bool *keys;
};

void ssp_input_manager_init(struct SSPInputManager *input_manager);
void ssp_input_manager_change_key_state(struct SSPInputManager *input_manager, enum SSP_KEYCODE keycode, enum SSP_KEYBOARD_ACTION action);
