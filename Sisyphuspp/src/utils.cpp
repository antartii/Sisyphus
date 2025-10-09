#include "utils.hpp"
#include "utils.h"
#include "sisyphus.h"

namespace Sisyphus {
    Version::Version(const int major, const int minor, const int patch):
        _major(major),
        _minor(minor),
        _patch(patch)
    {
    }

    Version::~Version()
    {
    }

    struct SSPVersion Version::data() const
    {
        struct SSPVersion data;
        data.major = _major;
        data.minor = _minor;
        data.patch = _patch;

        return data;
    }

    void libraryPrintInfo()
    {
        ssp_library_print_info();
    }
}
