#include "vulkan_context/command_buffer.h"

enum SSP_ERROR_CODE ssp_vulkan_create_command_pools(struct SSPVulkanContextExtFunc *ext_func,
    struct SSPVulkanQueueFamiliesIndices queue_family_indices,
    VkCommandPool **command_pools,
    VkDevice logical_device,
    int *graphic_command_pool_index,
    int *transfer_command_pool_index,
    int *command_pools_size)
{
    VkCommandPoolCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = queue_family_indices.graphic;

    int index = 0;
    *command_pools = calloc(queue_family_indices.queue_count, sizeof(VkCommandPool));

    ext_func->vkCreateCommandPool(logical_device, &create_info, NULL, &((*command_pools)[index]));
    *graphic_command_pool_index = index++;
    
    if (queue_family_indices.transfer == queue_family_indices.graphic)
        *transfer_command_pool_index = *graphic_command_pool_index;
    else {
        ext_func->vkCreateCommandPool(logical_device, &create_info, NULL, &((*command_pools)[index]));
        *transfer_command_pool_index = index++;
    }

    *command_pools_size = index;
    #ifdef DEBUG
    printf("Using %d command buffers\n", index);
    #endif

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_allocate_command_buffers(struct SSPVulkanContextExtFunc *ext_func,
    VkCommandPool graphic_command_pool,
    VkCommandBuffer **graphic_command_buffers,
    VkDevice logical_device,
    VkCommandPool transfer_command_pool,
    VkCommandBuffer **transfer_command_buffers,
    int *transfer_command_buffer_copy_index,
    int *transfer_command_buffer_image_copy_index,
    struct SSPDynamicArray **transfer_copy_buffer_queue,
    struct SSPDynamicArray **transfer_copy_buffer_to_image_queue)
{
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    alloc_info.commandPool = graphic_command_pool;
    alloc_info.commandBufferCount = SSP_MAX_FRAMES_IN_FLIGHT;
    *graphic_command_buffers = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkCommandBuffer));
    if (ext_func->vkAllocateCommandBuffers(logical_device, &alloc_info, *graphic_command_buffers) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_GRAPHIC;

    alloc_info.commandPool = transfer_command_pool;
    alloc_info.commandBufferCount = SSP_TRANSFER_COMMAND_BUFFERS_COUNT;
    *transfer_command_buffers = calloc(SSP_TRANSFER_COMMAND_BUFFERS_COUNT, sizeof(VkCommandBuffer));
    *transfer_command_buffer_copy_index = 0;
    *transfer_command_buffer_image_copy_index = *transfer_command_buffer_copy_index + 1;

    *transfer_copy_buffer_queue = ssp_dynamic_array_init(SSP_DEFAULT_COPY_BUFFER_QUEUE_POOL_SIZE, sizeof(struct SSPVulkanCopyBufferData), false);
    *transfer_copy_buffer_to_image_queue = ssp_dynamic_array_init(SSP_DEFAULT_COPY_BUFFER_TO_IMAGE_QUEUE_POOL_SIZE, sizeof(struct SSPVulkanCopyBufferToImageData), false);

    if (ext_func->vkAllocateCommandBuffers(logical_device, &alloc_info, *transfer_command_buffers) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_TRANSFER;

    return SSP_ERROR_CODE_SUCCESS;
}
