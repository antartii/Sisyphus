#ifndef _SISYPHUSPP_WINDOW_HPP
#define _SISYPHUSPP_WINDOW_HPP

#include "config.hpp"
#include "utils.hpp"
#include <memory>
#include "export.hpp"

struct SSPWindow;

namespace Sisyphus {
    class SSP_API Window {
        public:
            Window(Config &config);
            ~Window();

            bool should_close() const;
            bool run();
            struct SSPWindow *data() {return _window.get();}
        
        private:
            std::unique_ptr<struct SSPWindow> _window;
    };
}

#endif
