#ifndef _SISYPHUSPP_RENDERER_HPP
#define _SISYPHUSPP_RENDERER_HPP

#include "config.hpp"
#include "window.hpp"
#include <memory>
#include "renderer.h"

namespace Sisyphus {
    class Renderer {
        public :
            Renderer(Window &window, Config &config);
            ~Renderer();

            bool draw_frame(Window &window);

            struct SSPRenderer *data() {return _renderer.get();}

        private:
            std::unique_ptr<struct SSPRenderer> _renderer;
    };
}

#endif
