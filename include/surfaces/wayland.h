#ifndef _SISYPHUS_WAYLAND_H
#define _SISYPHUS_WAYLAND_H

#include <xdg/xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#include <xdg/xdg-decoration-unstable-v1-client-protocol.h>
#include <wayland-client.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "errors.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_WAYLAND_DISPLAY_NAME NULL
#define SSP_WAYLAND_WINDOW_TITLE_DEFAULT "Sisyphus window"

struct SSPWindow;

struct SSPWaylandSurfaceContext {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_surface *surface;
    struct xdg_wm_base *xdg_wm_base;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct wl_event_queue *queue;
};

enum SSP_ERROR_CODE ssp_wayland_surface_init(struct SSPWindow *window, struct SSPWaylandSurfaceContext *wayland_context, struct SSPConfig *config);
void ssp_wayland_surface_end(struct SSPWaylandSurfaceContext *wayland_context);
bool ssp_wayland_surface_run(struct SSPWaylandSurfaceContext *wayland_context);

#ifdef __cplusplus
    }
#endif

#endif
