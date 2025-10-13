#include "renderer.hpp"
#include "renderer.h"

namespace Sisyphus {
    Renderer::Renderer(Window &window, Config &config):
        _renderer(std::make_unique<struct SSPRenderer>())
    {
        ssp_renderer_create(_renderer.get(), window.data(), config.data());
    }

    Renderer::~Renderer()
    {
        ssp_renderer_destroy(_renderer.get());
    }

    void Renderer::stop()
    {
        vkDeviceWaitIdle(_renderer->vulkan_context.device.logical_device);
    }

    struct SSPRenderer *Renderer::data()
    {
        return _renderer.get();
    }

    struct SSPVulkanContext *Renderer::dataVulkanContext()
    {
        return &_renderer->vulkan_context;
    }

    bool Renderer::draw_frame(Window &window)
    {
        return (ssp_vulkan_draw_frame(&_renderer->vulkan_context, window.data(), _renderer->objects_to_draw) == SSP_ERROR_CODE_SUCCESS);
    }
}
