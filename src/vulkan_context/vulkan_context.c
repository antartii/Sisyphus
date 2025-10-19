#include "vulkan_context/vulkan_context.h"
#include "object.h"

static enum SSP_ERROR_CODE ssp_vulkan_resize(struct SSPVulkanContext *pContext, struct SSPWindow *window)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    struct SSPVulkanSwapchain *swapchain = &pContext->swapchain;

    if (window->height != 0 && window->width != 0) {
        ext_func->vkDeviceWaitIdle(pContext->device.logical_device);
        ssp_vulkan_swapchain_destroy(ext_func, swapchain, &pContext->device);

        ssp_vulkan_create_swapchain(ext_func, swapchain, &pContext->device, pContext->surface, window);
        ssp_vulkan_create_image_view(ext_func, swapchain, &pContext->device);

        ssp_vulkan_update_proj(pContext->cameraData, &pContext->pipeline_context, swapchain->extent);
        ssp_vulkan_update_view(pContext->cameraData, &pContext->pipeline_context);
    }

    window->resize_needed = false;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_draw_frame(struct SSPVulkanContext *pContext, struct SSPWindow *window, struct SSPDynamicArray *objects_to_draw)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    struct SSPVulkanDevice *device = &pContext->device;
    struct SSPVulkanCommandContext *command_context = &pContext->command_context;
    struct SSPVulkanPipelineContext *pipeline_context = &pContext->pipeline_context;
    bool resize_after_present = false;

    ssp_vulkan_copy_buffer_round(ext_func, &pContext->command_context, device);
    ssp_vulkan_copy_image_buffer_queue_round(ext_func, &pContext->command_context, &pContext->device);

    ext_func->vkWaitForFences(device->logical_device, 1, &(command_context->frames_in_flight_fences[pipeline_context->current_frame]), VK_TRUE, UINT64_MAX);

    if (window->resize_needed)
        return ssp_vulkan_resize(pContext, window);

    VkResult result = ext_func->vkAcquireNextImageKHR(device->logical_device, pContext->swapchain.vk_swapchain, UINT64_MAX, command_context->present_complete_semaphores[pipeline_context->current_frame], NULL, &pipeline_context->image_index);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return ssp_vulkan_resize(pContext, window);
    else if (result == VK_SUBOPTIMAL_KHR)
        resize_after_present = true;

    ssp_vulkan_record_graphic_command_buffer(ext_func, &pContext->swapchain, &pContext->command_context, pipeline_context, objects_to_draw);

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pCommandBuffers = &(pContext->command_context.graphic_command_buffers[pipeline_context->current_frame]);
    submit_info.commandBufferCount = 1;
    submit_info.pSignalSemaphores = &(command_context->render_finished_semaphores[pipeline_context->current_frame]);
    submit_info.signalSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &(command_context->present_complete_semaphores[pipeline_context->current_frame]);
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitDstStageMask = wait_stages;

    ext_func->vkResetFences(device->logical_device, 1, &(command_context->frames_in_flight_fences[pipeline_context->current_frame]));
    ext_func->vkQueueSubmit(device->graphic_queue, 1, &submit_info, command_context->frames_in_flight_fences[pipeline_context->current_frame]);

    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &(command_context->render_finished_semaphores[pipeline_context->current_frame]);
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &pContext->swapchain.vk_swapchain;
    present_info.pImageIndices = &pipeline_context->image_index;

    result = ext_func->vkQueuePresentKHR(device->present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR
        || result == VK_SUBOPTIMAL_KHR
        || window->resize_needed
        || resize_after_present)
        ssp_vulkan_resize(pContext, window);

    pipeline_context->current_frame = (pipeline_context->current_frame + 1) % SSP_MAX_FRAMES_IN_FLIGHT;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_context_init(struct SSPVulkanContext *pContext, struct SSPWindow *window, const char *app_name, struct SSPVersion app_version)
{
    enum SSP_ERROR_CODE err_code = SSP_ERROR_CODE_SUCCESS;
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    (err_code = ssp_vulkan_ext_func_instance_global(ext_func))
        || (err_code = ssp_vulkan_create_instance(ext_func, &pContext->instance, app_name, app_version))
        || (err_code = ssp_vulkan_ext_func_instance_nglobal(ext_func, pContext->instance))
        #ifdef DEBUG
        || (err_code = ssp_vulkan_setup_debug_messenger(ext_func, pContext->instance, &pContext->debug_messenger))
        #endif
        || (err_code = ssp_vulkan_create_surface(ext_func, pContext->instance, &pContext->surface, window))
        || (err_code = ssp_vulkan_init_physical_device(ext_func, pContext->instance, &pContext->device))
        || (err_code = ssp_vulkan_create_logical_device(ext_func, pContext->instance, &pContext->device, pContext->surface))
        || (err_code = ssp_vulkan_create_swapchain(ext_func, &pContext->swapchain, &pContext->device, pContext->surface, window))
        || (err_code = ssp_vulkan_create_descriptor_set_layout(ext_func, &pContext->device, &pContext->pipeline_context))
        || (err_code = ssp_vulkan_create_graphic_pipeline(ext_func, &pContext->pipeline_context, &pContext->device, &pContext->swapchain))
        || (err_code = ssp_vulkan_create_command_pools(ext_func, &pContext->command_context, &pContext->device))
        || (err_code = ssp_vulkan_create_uniform_buffers(ext_func, &pContext->pipeline_context, &pContext->device))
        || (err_code = ssp_vulkan_create_image_view(ext_func, &pContext->swapchain, &pContext->device))
        || (err_code = ssp_vulkan_create_descriptor_pool(ext_func, &pContext->device, &pContext->pipeline_context))
        || (err_code = ssp_vulkan_create_descriptor_sets(ext_func, &pContext->pipeline_context, &pContext->device))
        || (err_code = ssp_vulkan_create_sync_objects(ext_func, &pContext->command_context, &pContext->device))
        || (err_code = ssp_vulkan_allocate_command_buffers(ext_func, &pContext->command_context, &pContext->device));

    return err_code;
}

enum SSP_ERROR_CODE ssp_vulkan_context_destroy(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    struct SSPVulkanDevice *device = &pContext->device;
    VkDevice logical_device = device->logical_device;

    if (logical_device) {
        ext_func->vkDeviceWaitIdle(logical_device);

        ssp_vulkan_pipeline_destroy(ext_func, &pContext->pipeline_context, logical_device);
        ssp_vulkan_sync_objects_destroy(ext_func, &pContext->command_context, device);
        ssp_vulkan_swapchain_destroy(ext_func, &pContext->swapchain, device);
        ssp_vulkan_command_buffer_destroy(ext_func, &pContext->command_context, device);

        ext_func->vkDestroyDevice(logical_device, NULL);
    }
    if (pContext->instance) {
        if (pContext->surface)
            ext_func->vkDestroySurfaceKHR(pContext->instance, pContext->surface, NULL);
        #ifdef DEBUG
        ext_func->vkDestroyDebugUtilsMessengerEXT(pContext->instance, pContext->debug_messenger, NULL);
        #endif
    }
    ext_func->vkDestroyInstance(pContext->instance, NULL);

    return SSP_ERROR_CODE_SUCCESS;
}
