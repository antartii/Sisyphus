#include "vulkan_context/vulkan_context.h"
#include "object.h"

static enum SSP_ERROR_CODE ssp_vulkan_resize(struct SSPVulkanContext *pContext, struct SSPWindow *window)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    if (window->height != 0 && window->width != 0) {
        ext_func->vkDeviceWaitIdle(pContext->logical_device);
        ssp_vulkan_swapchain_destroy(ext_func, pContext->swapchain_image_views,
            pContext->swapchain_images_count, pContext->swapchain_images, pContext->logical_device, pContext->swapchain);

        ssp_vulkan_create_swapchain(ext_func,
            pContext->physical_device, pContext->logical_device,
            pContext->surface, window,
            &pContext->swapchain_image_format, &pContext->swapchain_extent,
            pContext->queue_family_indices,
            &pContext->swapchain, &pContext->swapchain_images_count, &pContext->swapchain_images);
        ssp_vulkan_create_image_view(ext_func, pContext->swapchain_image_format,
            &pContext->swapchain_image_views, pContext->swapchain_images_count,
            pContext->swapchain_images, pContext->logical_device);

        ssp_vulkan_update_proj(pContext->cameraData, pContext->uniform_buffers_mapped, pContext->swapchain_extent);
        ssp_vulkan_update_view(pContext->cameraData, pContext->uniform_buffers_mapped);
    }

    window->resize_needed = false;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_draw_frame(struct SSPVulkanContext *pContext, struct SSPWindow *window, struct SSPDynamicArray *objects_to_draw)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    bool resize_after_present = false;
    
    ssp_vulkan_copy_buffer_round(ext_func, pContext->transfer_copy_buffer_queue, pContext->transfer_command_buffers[pContext->transfer_command_buffer_copy_index],
        pContext->queue_family_indices, pContext->logical_device, pContext->transfer_queue, &pContext->transfer_copy_buffer_fence);

    ext_func->vkWaitForFences(pContext->logical_device, 1, &(pContext->frames_in_flight_fences[pContext->current_frame]), VK_TRUE, UINT64_MAX);

    if (window->resize_needed)
        return ssp_vulkan_resize(pContext, window);

    VkResult result = ext_func->vkAcquireNextImageKHR(pContext->logical_device, pContext->swapchain, UINT64_MAX, pContext->present_complete_semaphores[pContext->current_frame], NULL, &pContext->image_index);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return ssp_vulkan_resize(pContext, window);
    else if (result == VK_SUBOPTIMAL_KHR)
        resize_after_present = true;

    ssp_vulkan_record_graphic_command_buffer(ext_func,
        pContext->graphic_command_buffers, pContext->current_frame,
        &pContext->swapchain_images, pContext->image_index,
        pContext->swapchain_image_views, pContext->swapchain_extent, pContext->graphic_pipeline,
        pContext->pipeline_layout, pContext->viewport,
        &pContext->descriptor_sets, objects_to_draw);

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pCommandBuffers = &(pContext->graphic_command_buffers[pContext->current_frame]);
    submit_info.commandBufferCount = 1;
    submit_info.pSignalSemaphores = &(pContext->render_finished_semaphores[pContext->current_frame]);
    submit_info.signalSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &(pContext->present_complete_semaphores[pContext->current_frame]);
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitDstStageMask = wait_stages;

    ext_func->vkResetFences(pContext->logical_device, 1, &(pContext->frames_in_flight_fences[pContext->current_frame]));
    ext_func->vkQueueSubmit(pContext->graphic_queue, 1, &submit_info, pContext->frames_in_flight_fences[pContext->current_frame]);

    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &(pContext->render_finished_semaphores[pContext->current_frame]);
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &pContext->swapchain;
    present_info.pImageIndices = &pContext->image_index;

    result = ext_func->vkQueuePresentKHR(pContext->present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR
        || result == VK_SUBOPTIMAL_KHR
        || window->resize_needed
        || resize_after_present)
        ssp_vulkan_resize(pContext, window);

    pContext->current_frame = (pContext->current_frame + 1) % SSP_MAX_FRAMES_IN_FLIGHT;

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
        || (err_code = ssp_vulkan_init_physical_device(ext_func, pContext->instance, &pContext->physical_device))
        || (err_code = ssp_vulkan_create_logical_device(ext_func, window, pContext->physical_device, &pContext->logical_device, pContext->surface, &pContext->queue_family_indices, &pContext->transfer_queue, &pContext->present_queue, &pContext->graphic_queue))
        || (err_code = ssp_vulkan_create_swapchain(ext_func,
                pContext->physical_device, pContext->logical_device,
                pContext->surface, window,
                &pContext->swapchain_image_format, &pContext->swapchain_extent,
                pContext->queue_family_indices,
                &pContext->swapchain, &pContext->swapchain_images_count, &pContext->swapchain_images))
        || (err_code = ssp_vulkan_create_descriptor_set_layout(ext_func, pContext->logical_device, &pContext->descriptor_set_layout))
        || (err_code = ssp_vulkan_create_graphic_pipeline(ext_func,
            pContext->logical_device, &pContext->viewport, pContext->swapchain_extent, pContext->descriptor_set_layout,
            &pContext->pipeline_layout, pContext->swapchain_image_format, &pContext->graphic_pipeline))
        || (err_code = ssp_vulkan_create_command_pools(ext_func,
            pContext->queue_family_indices, &pContext->command_pools,
            pContext->logical_device, &pContext->graphic_command_pool_index,
            &pContext->transfer_command_pool_index, &pContext->command_pools_size))
        || (err_code = ssp_vulkan_create_uniform_buffers(ext_func, &pContext->uniform_buffers,
            &pContext->uniform_buffers_memory, &pContext->uniform_buffers_mapped,
            pContext->logical_device, pContext->physical_device))
        || (err_code = ssp_vulkan_create_image_view(ext_func, pContext->swapchain_image_format,
            &pContext->swapchain_image_views, pContext->swapchain_images_count,
            pContext->swapchain_images, pContext->logical_device))
        || (err_code = ssp_vulkan_create_descriptor_pool(ext_func, pContext->logical_device, &pContext->descriptor_pool))
        || (err_code = ssp_vulkan_create_descriptor_sets(ext_func, &pContext->descriptor_sets,
            pContext->descriptor_set_layout, pContext->descriptor_pool, pContext->logical_device, pContext->uniform_buffers))
        || (err_code = ssp_vulkan_create_sync_objects(ext_func, &pContext->present_complete_semaphores,
            &pContext->render_finished_semaphores, &pContext->frames_in_flight_fences, pContext->logical_device,
            &pContext->transfer_copy_buffer_fence, &pContext->transfer_copy_buffer_to_image_fence))
        || (err_code = ssp_vulkan_allocate_command_buffers(ext_func,
            pContext->command_pools[pContext->graphic_command_pool_index], &pContext->graphic_command_buffers,
            pContext->logical_device,
            pContext->command_pools[pContext->transfer_command_pool_index], &pContext->transfer_command_buffers,
            &pContext->transfer_command_buffer_copy_index,
            &pContext->transfer_command_buffer_image_copy_index,
            &pContext->transfer_copy_buffer_queue, &pContext->transfer_copy_buffer_to_image_queue));

    return err_code;
}

enum SSP_ERROR_CODE ssp_vulkan_context_destroy(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    if (pContext->logical_device) {
        ext_func->vkDeviceWaitIdle(pContext->logical_device);

        ext_func->vkDestroyDescriptorPool(pContext->logical_device, pContext->descriptor_pool, NULL);
        ext_func->vkDestroyDescriptorSetLayout(pContext->logical_device, pContext->descriptor_set_layout, NULL);
        ext_func->vkDestroyPipeline(pContext->logical_device, pContext->graphic_pipeline, NULL);
        ext_func->vkDestroyPipelineLayout(pContext->logical_device, pContext->pipeline_layout, NULL);

        if (pContext->uniform_buffers_mapped) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkUnmapMemory(pContext->logical_device, pContext->uniform_buffers_memory[i]);
            free(pContext->uniform_buffers_mapped);
        }

        if (pContext->uniform_buffers_memory) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkFreeMemory(pContext->logical_device, pContext->uniform_buffers_memory[i], NULL);
            free(pContext->uniform_buffers_memory);
        }

        if (pContext->uniform_buffers) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroyBuffer(pContext->logical_device, pContext->uniform_buffers[i], NULL);
            free(pContext->uniform_buffers);
        }

        if(pContext->frames_in_flight_fences) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroyFence(pContext->logical_device, pContext->frames_in_flight_fences[i], NULL);
            free(pContext->frames_in_flight_fences);
        }

        if (pContext->present_complete_semaphores) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroySemaphore(pContext->logical_device, pContext->present_complete_semaphores[i], NULL);
            free(pContext->present_complete_semaphores);
        }

        if (pContext->render_finished_semaphores) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroySemaphore(pContext->logical_device, pContext->render_finished_semaphores[i], NULL);
            free(pContext->render_finished_semaphores);
        }
        ext_func->vkDestroyFence(pContext->logical_device, pContext->transfer_copy_buffer_fence, NULL);
        ext_func->vkDestroyFence(pContext->logical_device, pContext->transfer_copy_buffer_to_image_fence, NULL);

        ssp_vulkan_swapchain_destroy(ext_func, pContext->swapchain_image_views,
            pContext->swapchain_images_count, pContext->swapchain_images, pContext->logical_device, pContext->swapchain);

        if (pContext->command_pools) {
            ext_func->vkFreeCommandBuffers(pContext->logical_device, pContext->command_pools[pContext->transfer_command_pool_index], SSP_TRANSFER_COMMAND_BUFFERS_COUNT, pContext->transfer_command_buffers);
            free(pContext->transfer_command_buffers);
            ext_func->vkFreeCommandBuffers(pContext->logical_device, pContext->command_pools[pContext->graphic_command_pool_index], SSP_MAX_FRAMES_IN_FLIGHT, pContext->graphic_command_buffers);
            free(pContext->graphic_command_buffers);
            for (int i = 0; i < pContext->command_pools_size; ++i)
                ext_func->vkDestroyCommandPool(pContext->logical_device, pContext->command_pools[i], NULL);
            free(pContext->command_pools);
        }
        ext_func->vkDestroyDevice(pContext->logical_device, NULL);
    }
    if (pContext->instance) {
        if (pContext->surface)
            ext_func->vkDestroySurfaceKHR(pContext->instance, pContext->surface, NULL);
        #ifdef DEBUG
        ext_func->vkDestroyDebugUtilsMessengerEXT(pContext->instance, pContext->debug_messenger, NULL);
        #endif
    }
    ext_func->vkDestroyInstance(pContext->instance, NULL);

    ssp_dynamic_array_free(pContext->transfer_copy_buffer_queue);
    ssp_dynamic_array_free(pContext->transfer_copy_buffer_to_image_queue);

    return SSP_ERROR_CODE_SUCCESS;
}
