#ifndef _SISYPHUS_HPP_TEXTURE_HPP
#define _SISYPHUS_HPP_TEXTURE_HPP

#include <string>
#include <memory>

#include "export.hpp"

struct SSPTexture;

namespace Sisyphus {
    class Engine;

    class SSP_API Texture {
        public:
            Texture(Engine &engine, const std::string &path);
            ~Texture();

            void FreeBuffers(Engine &engine);
            struct SSPTexture *data() {return _data.get();}

        private:
            std::unique_ptr<SSPTexture> _data;
    };
}

#endif
