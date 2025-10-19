#include "vulkan_context/texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "vulkan_context/vulkan_context.h"
#include "engine.h"

struct SSPTexture *ssp_texture_create_from_context(struct SSPVulkanContext *pContext, const char *path)
{
    struct SSPTexture *texture = calloc(1, sizeof(struct SSPTexture));

    stbi_uc * pixels = stbi_load(path, &texture->width, &texture->height, &texture->channels, STBI_rgb_alpha);
    if (!pixels) {
        free(texture);
        #ifdef DEBUG
        printf("Couldn't load the texture %s\n", path);
        #endif
        return NULL;
    }

    texture->image_size = texture->width * texture->height * 4;

    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkDeviceMemory staging_buffer_memory;
    VkBuffer staging_buffer;

    ssp_vulkan_stage_buffer(ext_func, &pContext->device, texture->image_size, pixels, &staging_buffer, &staging_buffer_memory);
    stbi_image_free(pixels);

    ssp_vulkan_create_image(
        ext_func,
        &pContext->device,
        texture->width, texture->height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &texture->image,
        &texture->image_memory);

    struct SSPVulkanCommandContext *command_context = &pContext->command_context;
    ssp_vulkan_copy_image_buffer_queue_push(command_context->transfer_copy_buffer_to_image_queue,
        command_context, texture->image, &texture->image_view,
        texture->height, texture->width,
        0, 0,
        staging_buffer,
        staging_buffer_memory,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER | SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY);

    return texture;
}

struct SSPTexture *spp_texture_create(struct SSPEngine *engine, const char *path)
{
    return ssp_texture_create_from_context(&engine->renderer->vulkan_context, path);
}

void ssp_texture_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPTexture *texture, struct SSPVulkanDevice *device)
{
    ext_func->vkDestroyImage(device->logical_device, texture->image, NULL);
    ext_func->vkFreeMemory(device->logical_device, texture->image_memory, NULL);
    ext_func->vkDestroyImageView(device->logical_device, texture->image_view, NULL);

    free(texture);
}
