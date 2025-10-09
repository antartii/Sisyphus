#include "camera.hpp"
#include "engine.hpp"
#include "camera.h"

namespace Sisyphus {
    Camera::Camera(Engine &engine):
        _camera(std::make_unique<struct SSPCamera>())
    {
        ssp_camera_init(_camera.get());
    }

    Camera::~Camera()
    {
        ssp_camera_destroy(_camera.get());
    }

    struct SSPCamera *Camera::data()
    {
        return _camera.get();
    }
}
