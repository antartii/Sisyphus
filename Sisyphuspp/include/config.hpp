#ifndef _SISYPHUSPP_CONFIG_HPP
#define _SISYPHUSPP_CONFIG_HPP

#include <string>
#include "utils.hpp"
#include <memory>
#include <cstring>
#include "export.hpp"

struct SSPConfig;

namespace Sisyphus {
    class SSP_API Config {
        public :
            Config(const std::string appName = "Sisyphus app", const Version version = Version(), const std::string windowTitle = "Sisyphus window");
            ~Config();

            struct SSPConfig *data() {return _config.get();}
        
        private:
            std::unique_ptr<struct SSPConfig> _config;
    };
}

#endif
