#ifndef _SISYPHUS_WINDOW_H
#define _SISYPHUS_WINDOW_H

#include <stdlib.h>
#include "errors.h"

struct SSPWindow {

};

enum SSP_ERROR_CODE ssp_window_create(struct SSPWindow *pWindow);
void ssp_window_destroy(struct SSPWindow *pWindow);

#endif
