#include "vulkan_context/sync_object.h"

enum SSP_ERROR_CODE ssp_vulkan_create_sync_objects(struct SSPVulkanContextExtFunc *ext_func,
    VkSemaphore **present_complete_semaphores,
    VkSemaphore **render_finished_semaphores,
    VkFence **frames_in_flight_fences,
    VkDevice logical_device,
    VkFence *transfer_copy_buffer_fence,
    VkFence *transfer_copy_buffer_to_image_fence)
{
    *present_complete_semaphores = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    *render_finished_semaphores = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    *frames_in_flight_fences = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));

    VkSemaphoreCreateInfo semaphore_create_info = {0};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fence_create_info = {0};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        if (ext_func->vkCreateSemaphore(logical_device, &semaphore_create_info, NULL, &((*present_complete_semaphores)[i])))
            return SSP_ERROR_CODE_VULKAN_CREATE_PRESENT_COMPLETE_SEMAPHORES;
        if (ext_func->vkCreateSemaphore(logical_device, &semaphore_create_info, NULL, &((*render_finished_semaphores)[i])))
            return SSP_ERROR_CODE_VULKAN_CREATE_RENDER_FINISHED_SEMAPHORES;
        if (ext_func->vkCreateFence(logical_device, &fence_create_info, NULL, &((*frames_in_flight_fences)[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_FRAMES_IN_FLIGHT_FENCES;
    }

    if (ext_func->vkCreateFence(logical_device, &fence_create_info, NULL, transfer_copy_buffer_fence) != VK_SUCCESS
        || ext_func->vkCreateFence(logical_device, &fence_create_info, NULL, transfer_copy_buffer_to_image_fence) != VK_SUCCESS)

    return SSP_ERROR_CODE_SUCCESS;
}
