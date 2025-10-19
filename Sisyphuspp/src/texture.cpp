#include "texture.hpp"
#include "engine.hpp"
#include "vulkan_context/texture.h"
#include "stb_image/stb_image.h"
#include "vulkan_context/vulkan_context.h"

namespace Sisyphus {
    Texture::Texture(Engine &engine, const std::string &path):
        _data(std::unique_ptr<SSPTexture>(ssp_texture_create_from_context(engine.getRenderer().dataVulkanContext(), path.c_str())))
    {
    }

    Texture::~Texture()
    {
    }

    void Texture::FreeBuffers(Engine &engine)
    {
        struct SSPVulkanContext *context = engine.getRenderer().dataVulkanContext();
        struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;
        VkDevice logical_device = context->device.logical_device;

        ext_func->vkDestroyImage(logical_device, _data->image, NULL);
        ext_func->vkFreeMemory(logical_device, _data->image_memory, NULL);
        ext_func->vkDestroyImageView(logical_device, _data->image_view, NULL);
    }
}
