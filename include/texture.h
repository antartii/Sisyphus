#ifndef _SISYPHUS_TEXTURE_H
#define _SISYPHUS_TEXTURE_H

#include <vulkan/vulkan.h>

struct SSPTexture {
    int width;
    int height;
    int channels;

    VkDeviceSize image_size;

    VkImage image;
    VkDeviceMemory image_memory;
};

struct SSPTexture *spp_texture_create(const char *path);
void ssp_texture_destroy(struct SSPTexture *texture);

#endif