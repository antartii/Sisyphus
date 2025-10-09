#include "config.hpp"
#include "config.h"

namespace Sisyphus {
    Config::Config(const std::string appName, const Version version, const std::string windowTitle):
        _config(std::make_unique<struct SSPConfig>())
    {
        _config->app_name = strdup(appName.c_str());
        _config->app_version = version.data();
        _config->window_title = strdup(windowTitle.c_str());
    }

    Config::~Config()
    {
        free(_config->app_name);
        free(_config->window_title);
    }
}