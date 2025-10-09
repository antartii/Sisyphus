#include "vulkan_context/surface.h"

enum SSP_ERROR_CODE ssp_vulkan_create_surface(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, VkSurfaceKHR *surface, struct SSPWindow *window)
{
    enum SSP_ERROR_CODE result = SSP_ERROR_CODE_VULKAN_INVALID_SURFACE;

    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VkWaylandSurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surface_create_info.display = window->surface_context.display;
    surface_create_info.surface = window->surface_context.surface;
    if (ext_func->vkCreateWaylandSurfaceKHR(instance, &surface_create_info, NULL, surface) == VK_SUCCESS)
        result = SSP_ERROR_CODE_SUCCESS;
    #endif

    return result;
}
