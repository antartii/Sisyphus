#include "utils.hpp"

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
}
