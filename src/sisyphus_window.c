#include "sisyphus_window.h"

struct SSPWindow *SSPWindow_create(void)
{
    struct SSPWindow *window = calloc(1, sizeof(struct SSPWindow));

    return window;
}

void SSPWindow_destroy(struct SSPWindow *window)
{
    free(window);
}
