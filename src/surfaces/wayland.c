#include "surfaces/wayland.h"
#include "window.h"

static void ssp_wayland_xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener ssp_wayland_xdg_surface_listener = {
    .configure = ssp_wayland_xdg_surface_configure
};

static void ssp_wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener ssp_wayland_xdg_wm_base_listener = {
    .ping = ssp_wayland_xdg_wm_base_ping
};

static void ssp_wayland_registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
	struct SSPWaylandSurfaceContext *context = (struct SSPWaylandSurfaceContext *) data;

    if (!strcmp(interface, wl_compositor_interface.name))
        context->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    else if (!strcmp(interface, xdg_wm_base_interface.name)) {
        context->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(context->xdg_wm_base, &ssp_wayland_xdg_wm_base_listener, context);
    }
}

static void ssp_wayland_registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}

static const struct wl_registry_listener ssp_wayland_registry_listener = {
	.global = ssp_wayland_registry_handle_global,
	.global_remove = ssp_wayland_registry_handle_global_remove,
};

static void ssp_wayland_xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
    struct SSPWindow *window = data;

    window->should_close = true;
};

static void ssp_wayland_xdg_toplevel_handle_configure(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height, struct wl_array *states)
{
    struct SSPWindow *window = data;

    if (width > 0 && height > 0) {
        if (window->width != width || window->height != height) {
            window->width  = width;
            window->height = height;
            window->resize_needed = true;
        }
    }
}

static const struct xdg_toplevel_listener ssp_wayland_xdg_toplevel_listener = {
    .close = ssp_wayland_xdg_toplevel_handle_close,
    .configure = ssp_wayland_xdg_toplevel_handle_configure,
    .configure_bounds = NULL,
    .wm_capabilities = NULL
};

void ssp_wayland_surface_end(struct SSPWaylandSurfaceContext *wayland_context)
{
    wl_display_disconnect(wayland_context->display);
}

enum SSP_ERROR_CODE ssp_wayland_surface_init(struct SSPWindow *window, struct SSPWaylandSurfaceContext *wayland_context, struct SSPConfig *config)
{
    const char *xdg_title = (config && config->window_title) ? config->window_title : SSP_WAYLAND_WINDOW_TITLE_DEFAULT;

    wayland_context->display = wl_display_connect(SSP_WAYLAND_DISPLAY_NAME);

    if (!wayland_context->display)
        return SSP_ERROR_CODE_SURFACE_INIT;
    
    wayland_context->registry = wl_display_get_registry(wayland_context->display);
    if (!wayland_context->registry)
        return SSP_ERROR_CODE_SURFACE_INIT;

    wl_registry_add_listener(wayland_context->registry, &ssp_wayland_registry_listener, wayland_context);
    wl_display_roundtrip(wayland_context->display);

    wayland_context->surface = wl_compositor_create_surface(wayland_context->compositor);
    if (!wayland_context->surface)
        return SSP_ERROR_CODE_SURFACE_INIT;
    
    wayland_context->xdg_surface = xdg_wm_base_get_xdg_surface(wayland_context->xdg_wm_base, wayland_context->surface);
    xdg_surface_add_listener(wayland_context->xdg_surface, &ssp_wayland_xdg_surface_listener, window);
    wayland_context->xdg_toplevel = xdg_surface_get_toplevel(wayland_context->xdg_surface);
    xdg_toplevel_set_title(wayland_context->xdg_toplevel, xdg_title);
    xdg_toplevel_add_listener(wayland_context->xdg_toplevel, &ssp_wayland_xdg_toplevel_listener, window);

    wl_surface_commit(wayland_context->surface);
    if (wl_display_roundtrip(wayland_context->display) == -1)
        return SSP_ERROR_CODE_SURFACE_INIT;

    return SSP_ERROR_CODE_SUCCESS;
}

bool ssp_wayland_surface_run(struct SSPWaylandSurfaceContext *wayland_context)
{
    wl_display_dispatch_pending(wayland_context->display);
    wl_display_flush(wayland_context->display);
    return true;
}