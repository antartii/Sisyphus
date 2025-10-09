#ifndef _SISYPHUSPP_UTILS_HPP
#define _SISYPHUSPP_UTILS_HPP

#include <iostream>
#include <memory>
#include <new>
#include <cstdlib>
#include "export.hpp"

struct SSPVersion;

namespace Sisyphus {
    class SSP_API Version {
        public:
            int _major;
            int _minor;
            int _patch;

            Version(const int major = 0, const int minor = 0, const int patch = 0);
            ~Version();

            struct SSPVersion data() const;

        private:
    };

    SSP_API void libraryPrintInfo();
}

#endif
