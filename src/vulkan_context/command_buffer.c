#include "vulkan_context/command_buffer.h"

enum SSP_ERROR_CODE ssp_vulkan_create_command_pools(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    VkCommandPoolCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = device->queue_family_indices.graphic;

    int index = 0;
    command_context->command_pools = calloc(device->queue_family_indices.queue_count, sizeof(VkCommandPool));

    ext_func->vkCreateCommandPool(device->logical_device, &create_info, NULL, &(command_context->command_pools[index]));
    command_context->graphic_command_pool_index = index++;
    
    if (device->queue_family_indices.transfer == device->queue_family_indices.graphic)
        command_context->transfer_command_pool_index = command_context->graphic_command_pool_index;
    else {
        ext_func->vkCreateCommandPool(device->logical_device, &create_info, NULL, &(command_context->command_pools[index]));
        command_context->transfer_command_pool_index = index++;
    }

    command_context->command_pools_size = index;
    #ifdef DEBUG
    printf("Using %d command buffers\n", index);
    #endif

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_allocate_command_buffers(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    alloc_info.commandPool = command_context->command_pools[command_context->graphic_command_pool_index];
    alloc_info.commandBufferCount = SSP_MAX_FRAMES_IN_FLIGHT;
    command_context->graphic_command_buffers = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkCommandBuffer));
    if (ext_func->vkAllocateCommandBuffers(device->logical_device, &alloc_info, command_context->graphic_command_buffers) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_GRAPHIC;

    alloc_info.commandPool = command_context->command_pools[command_context->transfer_command_pool_index];
    alloc_info.commandBufferCount = SSP_TRANSFER_COMMAND_BUFFERS_COUNT;
    command_context->transfer_command_buffers = calloc(SSP_TRANSFER_COMMAND_BUFFERS_COUNT, sizeof(VkCommandBuffer));
    command_context->transfer_command_buffer_copy_index = 0;
    command_context->transfer_command_buffer_image_copy_index = command_context->transfer_command_buffer_copy_index + 1;

    command_context->transfer_copy_buffer_queue = ssp_dynamic_array_init(SSP_DEFAULT_COPY_BUFFER_QUEUE_POOL_SIZE, sizeof(struct SSPVulkanCopyBufferData), false);
    command_context->transfer_copy_buffer_to_image_queue = ssp_dynamic_array_init(SSP_DEFAULT_COPY_BUFFER_TO_IMAGE_QUEUE_POOL_SIZE, sizeof(struct SSPVulkanCopyBufferToImageData), false);

    if (ext_func->vkAllocateCommandBuffers(device->logical_device, &alloc_info, command_context->transfer_command_buffers) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_TRANSFER;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_command_buffer_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    if (command_context->command_pools) {
        ext_func->vkFreeCommandBuffers(device->logical_device, command_context->command_pools[command_context->transfer_command_pool_index], SSP_TRANSFER_COMMAND_BUFFERS_COUNT, command_context->transfer_command_buffers);
        free(command_context->transfer_command_buffers);
        ext_func->vkFreeCommandBuffers(device->logical_device, command_context->command_pools[command_context->graphic_command_pool_index], SSP_MAX_FRAMES_IN_FLIGHT, command_context->graphic_command_buffers);
        free(command_context->graphic_command_buffers);
        for (int i = 0; i < command_context->command_pools_size; ++i)
            ext_func->vkDestroyCommandPool(device->logical_device, command_context->command_pools[i], NULL);
        free(command_context->command_pools);
    }

    ssp_dynamic_array_free(command_context->transfer_copy_buffer_queue);
    ssp_dynamic_array_free(command_context->transfer_copy_buffer_to_image_queue);

    return SSP_ERROR_CODE_SUCCESS;
}
