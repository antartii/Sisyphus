#include "renderer.hpp"

namespace Sisyphus {
    Renderer::Renderer(Window &window, Config &config):
        _renderer(std::make_unique<struct SSPRenderer>())
    {
        ssp_renderer_create(_renderer.get(), window.data(), config.data());
    }

    Renderer::~Renderer()
    {
        ssp_renderer_destroy(_renderer.get());
    }

    bool Renderer::draw_frame(Window &window)
    {
        return (ssp_renderer_draw_frame(_renderer.get(), window.data()) == SSP_ERROR_CODE_SUCCESS);
    }
}
