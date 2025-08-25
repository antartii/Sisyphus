#include "window.h"

enum SSP_ERROR_CODE ssp_window_create(struct SSPWindow *pWindow)
{
    return SSP_ERROR_CODE_SUCCESS;
}

void ssp_window_destroy(struct SSPWindow *pWindow)
{
    free(pWindow);
}
