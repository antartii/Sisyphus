#ifndef _SISYPHUS_TEXTURE_H
#define _SISYPHUS_TEXTURE_H

#include <vulkan/vulkan.h>
#include "device.h"
#include "vulkan_context.h"

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
    VkDeviceMemory image_memory;
};

struct SSPTexture *ssp_texture_create_from_context(struct SSPVulkanContext *pContext, const char *path); // temporary for C++
struct SSPTexture *spp_texture_create(struct SSPEngine *engine, const char *path);
void ssp_texture_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPTexture *texture, struct SSPVulkanDevice *device);

#ifdef __cplusplus
    }
#endif

#endif