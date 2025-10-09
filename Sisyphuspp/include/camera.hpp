#ifndef _SISYPHUSPP_CAMERA_HPP
#define _SISYPHUSPP_CAMERA_HPP

#include "export.hpp"

#include <memory>

struct SSPCamera;

namespace Sisyphus {
    class Engine;

    class SSP_API Camera {
        public:
            Camera(Engine &engine);
            ~Camera();

            struct SSPCamera *data();

        private:
            std::unique_ptr<struct SSPCamera> _camera;
    };
};

#endif