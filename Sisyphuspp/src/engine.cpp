#include "engine.hpp"

namespace Sisyphus {    
    Engine::Engine(Config &config):
        _window(config),
        _renderer(_window, config)
    {
    }

    Engine::~Engine()
    {
    }

    bool Engine::run()
    {
        return !_window.should_close()
            && _window.run()
            && _renderer.draw_frame(_window);
    }
    
    void Engine::updateCamera(Camera &camera)
    {
        ssp_vulkan_update_proj(&_renderer.data()->vulkan_context, camera.data());
        ssp_vulkan_update_view(&_renderer.data()->vulkan_context, camera.data());
    }
}

