#include "window.hpp"
#include "window.h"

namespace Sisyphus {
    Window::Window(Config &config):
        _window((struct SSPWindow *) calloc(1, sizeof(struct SSPWindow)))
    {
        _window->height = SSP_WINDOW_DEFAULT_HEIGHT;
        _window->width = SSP_WINDOW_DEFAULT_WIDTH;

        ssp_window_create(_window.get(), config.data());
    }

    Window::~Window()
    {
        ssp_window_destroy(_window.get());
    }

    bool Window::should_close() const
    {
        return _window->should_close;
    }

    bool Window::run()
    {
        bool result = false;

        #ifdef VK_USE_PLATFORM_WAYLAND_KHR
        result = ssp_wayland_surface_run(&_window->surface_context);
        #endif
        return result;
    }
}