#include "input_manager.h"

const char *SSP_KEYCODE_STR[] = {
    #define X(keycode, str) str,
    SSP_KEYCODES(X)
    #undef X
};

void ssp_input_manager_init(struct SSPInputManager *input_manager)
{
    input_manager->keys = malloc(sizeof(bool) * SSP_KEYCODE_COUNT);
}

void ssp_input_manager_change_key_state(struct SSPInputManager *input_manager, enum SSP_KEYCODE keycode, enum SSP_KEYBOARD_ACTION action)
{
    if (keycode != SSP_KEYCODE_UNDEFINED)
        input_manager->keys[keycode] = (bool) action;

    #ifdef DEBUG
    const char *action_str = (action == SSP_KEYBOARD_ACTION_PRESS ? "press" : "release");
    printf("Key %s : %s\n", action_str, SSP_KEYCODE_STR[keycode]);
    #endif
}
