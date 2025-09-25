#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <string>
#include "utils.hpp"
#include "config.h"
#include <memory>
#include <cstring>

namespace Sisyphus {
    class Config {
        public :
            Config(const std::string appName = "Sisyphus app", const Version version = Version(), const std::string windowTitle = "Sisyphus window");
            ~Config();

            struct SSPConfig *data() {return _config.get();}
        
        private:
            std::unique_ptr<struct SSPConfig> _config;
    };
}

#endif
