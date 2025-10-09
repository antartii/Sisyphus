#ifndef _SISYPHUSPP_RENDERER_HPP
#define _SISYPHUSPP_RENDERER_HPP

#include "config.hpp"
#include "window.hpp"
#include <memory>
#include "export.hpp"

struct SSPRenderer;
struct SSPVulkanContext;

namespace Sisyphus {
    class SSP_API Renderer {
        public :
            Renderer(Window &window, Config &config);
            ~Renderer();

            bool draw_frame(Window &window);
            void stop();

            struct SSPRenderer *data();
            struct SSPVulkanContext *dataVulkanContext();

        private:
            std::unique_ptr<struct SSPRenderer> _renderer;
    };
}

#endif
