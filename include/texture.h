#ifndef _SISYPHUS_TEXTURE_H
#define _SISYPHUS_TEXTURE_H

#include <vulkan/vulkan.h>
#include "vulkan_context/device.h"
#include "vulkan_context/vulkan_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPEngine;

struct SSPTexture {
    int width;
    int height;
    int channels;

    VkDeviceSize image_size;

    VkImage image;
    VkImageView image_view;
    VkDeviceMemory image_memory;
};

enum SSP_ERROR_CODE spp_texture_create(struct SSPTexture *texture, struct SSPEngine *engine, const char *path);
void ssp_texture_destroy(struct SSPEngine *engine, struct SSPTexture *texture);

#ifdef __cplusplus
    }
#endif

#endif