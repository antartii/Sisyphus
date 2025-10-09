#include "engine.hpp"
#include "engine.h"

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
        ssp_dynamic_array_push(_renderer.data()->objects_to_draw, (void *) object.data());
    }
    
    void Engine::updateCamera(Camera &camera)
    {
        SSPCamera *data = camera.data();

        _renderer.dataVulkanContext()->cameraData = data;

        ssp_vulkan_update_proj(camera.data(), _renderer.dataVulkanContext()->uniform_buffers_mapped, _renderer.dataVulkanContext()->swapchain_extent);
        ssp_vulkan_update_view(camera.data(), _renderer.dataVulkanContext()->uniform_buffers_mapped);
    }
}
