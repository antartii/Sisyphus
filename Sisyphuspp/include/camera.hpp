#ifndef _SISYPHUSPP_CAMERA_HPP
#define _SISYPHUSPP_CAMERA_HPP

#include "camera.h"
#include <memory>

namespace Sisyphus {
    class Engine;

    class Camera {
        public:
            Camera(Engine &engine);
            ~Camera();

            struct SSPCamera *data() {return _camera.get();}

        private:
            std::unique_ptr<struct SSPCamera> _camera;
    };
};

#endif