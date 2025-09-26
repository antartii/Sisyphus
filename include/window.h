#ifndef _SISYPHUS_WINDOW_H
#define _SISYPHUS_WINDOW_H

#include <stdlib.h>
#include "errors.h"
#include "surfaces/surfaces.h"
#include <stdio.h>
#include <stdbool.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_WINDOW_TITLE_DEFAULT "Sisyphus window"
#define SSP_WINDOW_DEFAULT_WIDTH 1080
#define SSP_WINDOW_DEFAULT_HEIGHT 1920

struct SSPWindow {
    int width;
    int height;
    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
        struct SSPWaylandSurfaceContext surface_context;
    #endif

    bool should_close;
    bool resize_needed;

    char *title;
};

enum SSP_ERROR_CODE ssp_window_create(struct SSPWindow *pWindow, struct SSPConfig *config);
void ssp_window_destroy(struct SSPWindow *pWindow);
bool ssp_window_run(struct SSPWindow *pWindow);

#ifdef __cplusplus
    }
#endif

#endif
