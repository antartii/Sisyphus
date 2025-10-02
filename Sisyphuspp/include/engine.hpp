#ifndef _SISYPHUSPP_ENGINE_HPP
#define _SISYPHUSPP_ENGINE_HPP

#include "engine.h"
#include <memory>
#include "window.hpp"
#include "utils.hpp"
#include "config.hpp"
#include <string>
#include "renderer.hpp"
#include "camera.hpp"
#include "object.hpp"

namespace Sisyphus {
    class Engine {
        public:
            Engine(Config &config);
            ~Engine();

            bool run();
            void updateCamera(Camera &camera);
            void draw(Object &object);

            Renderer &getRenderer() {return _renderer;}
            Window &getWindow() {return _window;}

        private :
            Window _window;
            Renderer _renderer;
    };
}

#endif
