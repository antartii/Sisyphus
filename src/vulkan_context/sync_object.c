#include "vulkan_context/sync_object.h"

enum SSP_ERROR_CODE ssp_vulkan_create_sync_objects(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    command_context->present_complete_semaphores = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    command_context->render_finished_semaphores = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    command_context->frames_in_flight_fences = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));

    VkSemaphoreCreateInfo semaphore_create_info = {0};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_create_info = {0};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        if (ext_func->vkCreateSemaphore(device->logical_device, &semaphore_create_info, NULL, &(command_context->present_complete_semaphores[i])))
            return SSP_ERROR_CODE_VULKAN_CREATE_PRESENT_COMPLETE_SEMAPHORES;
        if (ext_func->vkCreateSemaphore(device->logical_device, &semaphore_create_info, NULL, &(command_context->render_finished_semaphores[i])))
            return SSP_ERROR_CODE_VULKAN_CREATE_RENDER_FINISHED_SEMAPHORES;
        if (ext_func->vkCreateFence(device->logical_device, &fence_create_info, NULL, &(command_context->frames_in_flight_fences[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_FRAMES_IN_FLIGHT_FENCES;
    }

    if (ext_func->vkCreateFence(device->logical_device, &fence_create_info, NULL, &command_context->transfer_copy_buffer_fence) != VK_SUCCESS
        || ext_func->vkCreateFence(device->logical_device, &fence_create_info, NULL, &command_context->transfer_copy_buffer_to_image_fence) != VK_SUCCESS)

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_sync_objects_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    VkDevice logical_device = device->logical_device;

    if(command_context->frames_in_flight_fences) {
        for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
            ext_func->vkDestroyFence(logical_device, command_context->frames_in_flight_fences[i], NULL);
        free(command_context->frames_in_flight_fences);
    }

    if (command_context->present_complete_semaphores) {
        for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
            ext_func->vkDestroySemaphore(logical_device, command_context->present_complete_semaphores[i], NULL);
        free(command_context->present_complete_semaphores);
    }

    if (command_context->render_finished_semaphores) {
        for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
            ext_func->vkDestroySemaphore(logical_device, command_context->render_finished_semaphores[i], NULL);
        free(command_context->render_finished_semaphores);
    }
    ext_func->vkDestroyFence(logical_device, command_context->transfer_copy_buffer_fence, NULL);
    ext_func->vkDestroyFence(logical_device, command_context->transfer_copy_buffer_to_image_fence, NULL);
}
