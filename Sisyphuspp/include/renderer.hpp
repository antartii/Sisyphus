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
            void stop();

            struct SSPRenderer *data() {return _renderer.get();}
            struct SSPVulkanContext *dataVulkanContext() {return &_renderer->vulkan_context;}

        private:
            std::unique_ptr<struct SSPRenderer> _renderer;
    };
}

#endif
