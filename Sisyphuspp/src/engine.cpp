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
        if (!_window.should_close()
            && _window.run()
            && _renderer.draw_frame(_window))
                return true;
        
        _renderer.stop();
        return false;
    }

    void Engine::draw(Object &object)
    {
        ssp_dynamic_array_push(_renderer.data()->objects_to_draw, object.data());
    }
    
    void Engine::updateCamera(Camera &camera)
    {
        SSPCamera *data = camera.data();

        _renderer.dataVulkanContext()->cameraData = data;

        ssp_vulkan_update_proj(_renderer.dataVulkanContext());
        ssp_vulkan_update_view(_renderer.dataVulkanContext());
    }
}

