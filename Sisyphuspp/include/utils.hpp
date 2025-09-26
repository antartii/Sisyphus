#ifndef _SISYPHUSPP_UTILS_HPP
#define _SISYPHUSPP_UTILS_HPP

#include "utils.h"
#include <iostream>
#include <memory>
#include <new>
#include <cstdlib>

namespace Sisyphus {
    class Version {
        public:
            int _major;
            int _minor;
            int _patch;

            Version(const int major = 0, const int minor = 0, const int patch = 0);
            ~Version();

            struct SSPVersion data() const;

        private:
    };
}

#endif
