#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "vulkan_context/vulkan_context.h"
#include "engine.h"

struct SSPTexture *spp_texture_create(struct SSPEngine *engine, const char *path)
{
    struct SSPTexture *texture = calloc(1, sizeof(struct SSPTexture));

    stbi_uc * pixels = stbi_load(path, &texture->width, &texture->height, &texture->channels, STBI_rgb_alpha);
    if (!pixels) {
        free(texture);
        return NULL;
    }

    texture->image_size = texture->width * texture->height * 4;

    struct SSPVulkanContext *context = &engine->renderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;

    VkDeviceMemory staging_buffer_memory;
    VkBuffer staging_buffer;

    ssp_vulkan_stage_buffer(context, texture->image_size, pixels, &staging_buffer, &staging_buffer_memory);
    stbi_image_free(pixels);

    ssp_vulkan_create_image(
        ext_func,
        &context->device,
        texture->width, texture->height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &texture->image,
        &texture->image_memory);

    ssp_vulkan_transition_image_layout(context, texture->image,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0, VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        context->transfer_command_buffers[0],
        VK_QUEUE_FAMILY_IGNORED, context->queue_family_indices.transfer);

    return texture;
}
