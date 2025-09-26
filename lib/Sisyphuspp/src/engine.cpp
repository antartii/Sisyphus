#include "engine.hpp"

namespace Sisyphus {    
    Engine::Engine(Config &config):
        _window(config),
        _renderer(_window, config)
    {
    }

    Engine::~Engine()
    {
    }

    bool Engine::run()
    {
        return !_window.should_close()
            && _window.run()
            && _renderer.draw_frame(_window);
    }
}

