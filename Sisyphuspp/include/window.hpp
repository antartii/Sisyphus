#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include "window.h"
#include "config.hpp"
#include "utils.hpp"
#include <memory>

namespace Sisyphus {
    class Window {
        public:
            Window(Config &config);
            ~Window();

            bool should_close() const {return _window->should_close;}
            bool run();
            struct SSPWindow *data() {return _window;}
        
        private:
            struct SSPWindow *_window;
            struct SSPConfig _config;
    };
}

#endif
