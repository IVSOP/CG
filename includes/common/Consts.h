#ifndef CG_CONSTS_H
#define CG_CONSTS_H

#include <glm/matrix.hpp>

class Consts {

public:
    static glm::mat4 rotYMatrix(float angle);
    static glm::mat4 rotXMatrix(float angle);
    static glm::mat4 rotZMatrix(float angle);
};

#endif //CG_CONSTS_H
