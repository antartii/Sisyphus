#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include "engine.h"
#include <memory>
#include "window.hpp"
#include "utils.hpp"
#include "config.hpp"
#include <string>
#include "renderer.hpp"

namespace Sisyphus {
    class Engine {
        public:
            Engine(Config &config);
            ~Engine();

            bool run();

        private :
            Window _window;
            Renderer _renderer;
    };
}

#endif
