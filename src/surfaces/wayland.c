#include "surfaces/wayland.h"
#include "window.h"

#define SSP_WAYLAND_MAP_KEYCODE(sym, ssp_keycode)   \
    case sym: keycode = keycode; break;

#define SSP_WAYLAND_MAP_LETTER(x, upper_x) \
    case XKB_KEY_##x: \
    case XKB_KEY_##upper_x : keycode = SSP_KEYCODE_##upper_x; break;

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

static void ssp_wayland_keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size)
{
    struct SSPWindow *window = data;
    struct SSPWaylandSurfaceContext *context = &window->surface_context;
    char *map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(context->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    
    munmap(map_shm, size);
    close(fd);

    struct xkb_state *xkb_state = xkb_state_new(xkb_keymap);
    xkb_keymap_unref(context->xkb_keymap);
    xkb_state_unref(context->xkb_state);
    context->xkb_keymap = xkb_keymap;
    context->xkb_state = xkb_state;
}

static void ssp_wayland_keyboard_convert_input(struct SSPInputManager *input_manager, xkb_keysym_t sym, enum SSP_KEYBOARD_ACTION action)
{
    enum SSP_KEYCODE keycode = SSP_KEYCODE_UNDEFINED;

    switch (sym) {
        SSP_WAYLAND_MAP_LETTER(a, A)
        SSP_WAYLAND_MAP_LETTER(b, B)
        SSP_WAYLAND_MAP_LETTER(c, C)
        SSP_WAYLAND_MAP_LETTER(d, D)
        SSP_WAYLAND_MAP_LETTER(e, E)
        SSP_WAYLAND_MAP_LETTER(f, F)
        SSP_WAYLAND_MAP_LETTER(g, G)
        SSP_WAYLAND_MAP_LETTER(h, H)
        SSP_WAYLAND_MAP_LETTER(i, I)
        SSP_WAYLAND_MAP_LETTER(j, J)
        SSP_WAYLAND_MAP_LETTER(k, K)
        SSP_WAYLAND_MAP_LETTER(l, L)
        SSP_WAYLAND_MAP_LETTER(m, M)
        SSP_WAYLAND_MAP_LETTER(n, N)
        SSP_WAYLAND_MAP_LETTER(o, O)
        SSP_WAYLAND_MAP_LETTER(p, P)
        SSP_WAYLAND_MAP_LETTER(q, Q)
        SSP_WAYLAND_MAP_LETTER(r, R)
        SSP_WAYLAND_MAP_LETTER(s, S)
        SSP_WAYLAND_MAP_LETTER(t, T)
        SSP_WAYLAND_MAP_LETTER(u, U)
        SSP_WAYLAND_MAP_LETTER(v, V)
        SSP_WAYLAND_MAP_LETTER(w, W)
        SSP_WAYLAND_MAP_LETTER(x, X)
        SSP_WAYLAND_MAP_LETTER(y, Y)
        SSP_WAYLAND_MAP_LETTER(z, Z)

        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Shift_L, SSP_KEYCODE_LEFT_SHIFT)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Shift_R, SSP_KEYCODE_RIGHT_SHIFT)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Return, SSP_KEYCODE_ENTER)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Control_L, SSP_KEYCODE_LEFT_CTRL)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Control_R, SSP_KEYCODE_RIGHT_CTRL)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Shift_Lock, SSP_KEYCODE_CAPS_LOCK)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Tab, SSP_KEYCODE_TAB)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_BackSpace, SSP_KEYCODE_BACKSPACE)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_space, SSP_KEYCODE_SPACE)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Alt_L, SSP_KEYCODE_ALT)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_Alt_R, SSP_KEYCODE_ALT_GR)

        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_leftarrow, SSP_KEYCODE_LEFT_ARROW)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_rightarrow, SSP_KEYCODE_RIGHT_ARROW)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_uparrow, SSP_KEYCODE_UP_ARROW)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_downarrow, SSP_KEYCODE_DOWN_ARROW)

        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_0, SSP_KEYCODE_0)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_1, SSP_KEYCODE_1)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_2, SSP_KEYCODE_2)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_3, SSP_KEYCODE_3)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_4, SSP_KEYCODE_4)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_5, SSP_KEYCODE_5)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_6, SSP_KEYCODE_6)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_7, SSP_KEYCODE_7)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_8, SSP_KEYCODE_8)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_9, SSP_KEYCODE_9)

        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_0, SSP_KEYCODE_PAD_0)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_1, SSP_KEYCODE_PAD_1)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_2, SSP_KEYCODE_PAD_2)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_3, SSP_KEYCODE_PAD_3)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_4, SSP_KEYCODE_PAD_4)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_5, SSP_KEYCODE_PAD_5)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_6, SSP_KEYCODE_PAD_6)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_7, SSP_KEYCODE_PAD_7)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_8, SSP_KEYCODE_PAD_8)
        SSP_WAYLAND_MAP_KEYCODE(XKB_KEY_KP_9, SSP_KEYCODE_PAD_9)
    }

    ssp_input_manager_change_key_state(input_manager, keycode, action);
}

static void ssp_wayland_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
{
    struct SSPWindow *window = data;
    struct SSPWaylandSurfaceContext *context = &window->surface_context;
    struct SSPInputManager *input_manager = &window->input_manager;

    uint32_t *key;
    wl_array_for_each(key, keys) {
        char buf[128];
        xkb_keysym_t sym = xkb_state_key_get_one_sym(context->xkb_state, *key + 8);

        ssp_wayland_keyboard_convert_input(input_manager, sym, SSP_KEYBOARD_ACTION_PRESS);

        //xkb_keysym_get_name(sym, buf, sizeof(buf));
        //fprintf(stderr, "sym: %-12s (%d), ", buf, sym);
        //xkb_state_key_get_utf8(context->xkb_state, *key + 8, buf, sizeof(buf));
        //fprintf(stderr, "utf8: '%s'\n", buf);
    }
}

static void ssp_wayland_keyboard_key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    struct SSPWindow *window = data;
    struct SSPWaylandSurfaceContext *context = &window->surface_context;
    struct SSPInputManager *input_manager = &window->input_manager;

    char buf[128];
    uint32_t keycode = key + 8;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(context->xkb_state, keycode);
    enum SSP_KEYBOARD_ACTION action = (state == WL_KEYBOARD_KEY_STATE_PRESSED);

    ssp_wayland_keyboard_convert_input(input_manager, sym, action);
}

static void ssp_wayland_keyboard_leave(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface)
{
}

static void ssp_wayland_keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    struct SSPWindow *window = data;
    struct SSPWaylandSurfaceContext *context = &window->surface_context;
    
    xkb_state_update_mask(context->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void ssp_wayland_keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard, int32_t rate, int32_t delay)
{
}


static const struct wl_keyboard_listener ssp_wayland_keyboard_listener = {
    .keymap = ssp_wayland_keyboard_keymap,
    .enter = ssp_wayland_keyboard_enter,
    .leave = ssp_wayland_keyboard_leave,
    .key = ssp_wayland_keyboard_key,
    .modifiers = ssp_wayland_keyboard_modifiers,
    .repeat_info = ssp_wayland_keyboard_repeat_info,
};

static void ssp_wayland_seat_capabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities)
{
    struct SSPWindow *window = data;
    struct SSPInputManager *input_manager = &window->input_manager;
    struct SSPWaylandSurfaceContext *context = &window->surface_context;

    input_manager->have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    if (input_manager->have_keyboard && context->wl_keyboard == NULL) {
        context->wl_keyboard = wl_seat_get_keyboard(context->wl_seat);
        wl_keyboard_add_listener(context->wl_keyboard, &ssp_wayland_keyboard_listener, window);
    } else if (!input_manager->have_keyboard && context->wl_keyboard != NULL) {
        wl_keyboard_release(context->wl_keyboard);
        context->wl_keyboard = NULL;
    }
}

static void ssp_wayland_seat_name(void *data, struct wl_seat *wl_seat, const char *name)
{
}

static const struct wl_seat_listener ssp_wayland_seat_listener = {
    .capabilities = ssp_wayland_seat_capabilities,
    .name = ssp_wayland_seat_name,
};

static void ssp_wayland_registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    struct SSPWindow *window = data;
	struct SSPWaylandSurfaceContext *context = &window->surface_context;

    if (!strcmp(interface, wl_compositor_interface.name))
        context->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    else if (!strcmp(interface, xdg_wm_base_interface.name)) {
        context->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(context->xdg_wm_base, &ssp_wayland_xdg_wm_base_listener, context);
    } else if (!strcmp(interface, wl_seat_interface.name)) {
        context->wl_seat = wl_registry_bind(registry, name, &wl_seat_interface, 7);
        wl_seat_add_listener(context->wl_seat, &ssp_wayland_seat_listener, window);
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
    const char *xdg_app_title = (config && config->app_name) ? config->app_name : SSP_WAYLAND_WINDOW_TITLE_DEFAULT;

    wayland_context->display = wl_display_connect(SSP_WAYLAND_DISPLAY_NAME);

    if (!wayland_context->display)
        return SSP_ERROR_CODE_SURFACE_INIT;
    
    wayland_context->registry = wl_display_get_registry(wayland_context->display);
    if (!wayland_context->registry)
        return SSP_ERROR_CODE_SURFACE_INIT;

    wl_registry_add_listener(wayland_context->registry, &ssp_wayland_registry_listener, window);
    wl_display_roundtrip(wayland_context->display);

    wayland_context->surface = wl_compositor_create_surface(wayland_context->compositor);
    if (!wayland_context->surface)
        return SSP_ERROR_CODE_SURFACE_INIT;
    
    wayland_context->xdg_surface = xdg_wm_base_get_xdg_surface(wayland_context->xdg_wm_base, wayland_context->surface);
    xdg_surface_add_listener(wayland_context->xdg_surface, &ssp_wayland_xdg_surface_listener, window);
    wayland_context->xdg_toplevel = xdg_surface_get_toplevel(wayland_context->xdg_surface);
    xdg_toplevel_set_title(wayland_context->xdg_toplevel, xdg_title);
    xdg_toplevel_set_app_id(wayland_context->xdg_toplevel, xdg_app_title);
    xdg_toplevel_add_listener(wayland_context->xdg_toplevel, &ssp_wayland_xdg_toplevel_listener, window);

    wayland_context->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

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