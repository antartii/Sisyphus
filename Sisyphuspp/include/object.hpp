#ifndef _SISYPHUSPP_OBJECT_HPP
#define _SISYPHUSPP_OBJECT_HPP

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "object.h"

namespace Sisyphus {
    class Engine;

    class Object {
        public:
            Object(Engine &engine, std::vector<glm::vec2> verticesPos, glm::vec3 color, std::vector<uint16_t> indices, uint32_t verticesCount);
            ~Object();

            void FreeBuffers(Engine &engine);
            SSPObject *data() {return _object.get();}

        protected:
            std::unique_ptr<struct SSPObject> _object;
    };

    class Rectangle : public Object {
        public:
            Rectangle(Engine &engine, glm::vec2 pos, glm::vec2 size, glm::vec3 color);
            ~Rectangle();

        private:
            std::vector<glm::vec2> generateVerticesPos(glm::vec2 pos, glm::vec2 size);
            
            static const std::vector<uint16_t> _indices;
    };
}

#endif