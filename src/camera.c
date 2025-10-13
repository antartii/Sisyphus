#include "camera.h"
#include "engine.h"

struct SSPCamera *ssp_camera_create()
{
    struct SSPCamera *camera = calloc(1, sizeof(struct SSPCamera));
    ssp_camera_init(camera);

    return camera;
}

void ssp_camera_init(struct SSPCamera *camera)
{
    glm_vec3_copy((vec3) {0.0f, 0.0f, 1.0f}, camera->pos);
    glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, camera->target);
    glm_vec3_copy((vec3) {0.0f, 1.0f, 0.0f}, camera->up);

    glm_vec2_copy((vec2) {SSP_CAMERA_DEPTH_MIN_RENDER_DEFAULT, SSP_CAMERA_DEPTH_MAX_RENDER_DEFAULT}, camera->render_depth_range);
    camera->fov_in_radians = SSP_CAMERA_FOV_DEFAULT_DEGREES;
}

void ssp_camera_update(struct SSPEngine *pEngine, struct SSPCamera *camera)
{
    pEngine->renderer->vulkan_context.cameraData = camera;
    struct SSPVulkanContext *context = &pEngine->renderer->vulkan_context;

    ssp_vulkan_update_proj(camera, &context->pipeline_context, context->swapchain.extent);
    ssp_vulkan_update_view(camera, &context->pipeline_context);
}

void ssp_camera_destroy(struct SSPCamera *camera)
{
}
