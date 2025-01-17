#ifndef CG_CONSTS_H
#define CG_CONSTS_H

#include <glm/matrix.hpp>
#include <vector>
#include <Vertex.h>
class Consts {
private:
    struct myVec3 {
        float x, y, z;
    };

public:
    static glm::mat4 rotYMatrix(float angle);
    static glm::mat4 rotXMatrix(float angle);
    static glm::mat4 rotZMatrix(float angle);
    static glm::mat4 rotMatrixVector(float angle, float x, float y, float z);
    static glm::mat2 rotMatrix2D(float angle);
    static glm::mat4 translateMatrix(float x, float y, float z);
    static glm::mat4 scaleMatrix(float x, float y, float z);
    static glm::mat4 idMatrix();
    static glm::mat4 bezierCoefficients();
    static std::vector<Vertex> calcNormalAvg(std::vector<Vertex>& points);
};

#endif //CG_CONSTS_H
