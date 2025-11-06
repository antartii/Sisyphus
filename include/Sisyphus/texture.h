#pragma once

#include <vulkan/vulkan.h>
#include "vulkan_context/device.h"
#include "vulkan_context/vulkan_context.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_TEXTURE_STATUS {
    SSP_TEXTURE_NOT_INITIALISED = 0,
    SSP_TEXTURE_UPLOADING,
    SSP_TEXTURE_READY
};

struct SSPEngine;

struct SSPTexture {
    int width;
    int height;
    int channels;

    struct SSPVulkanImage image;
    VkDeviceSize image_size;
    VkDeviceMemory image_memory;
};

SSP_API enum SSP_ERROR_CODE spp_texture_create(struct SSPTexture *texture, struct SSPEngine *engine, const char *path);
SSP_API void ssp_texture_destroy(struct SSPEngine *engine, struct SSPTexture *texture);

#ifdef __cplusplus
    }
#endif