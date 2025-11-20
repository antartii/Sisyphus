#include "window.h"

enum SSP_ERROR_CODE ssp_window_create(struct SSPWindow *pWindow, struct SSPConfig *config)
{
    enum SSP_ERROR_CODE result = SSP_ERROR_CODE_SURFACE_INIT;

    ssp_input_manager_init(&pWindow->input_manager);

    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    result = ssp_wayland_surface_init(pWindow, &pWindow->surface_context, config);
    #endif

    return result;
}

bool ssp_window_run(struct SSPWindow *pWindow)
{
    bool result = false;
    
    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    result = ssp_wayland_surface_run(&pWindow->surface_context);
    #endif
    return result;
}

void ssp_window_destroy(struct SSPWindow *pWindow)
{
    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    ssp_wayland_surface_end(&pWindow->surface_context);
    #endif

    free(pWindow->title);
}
