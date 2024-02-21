#include "Consts.h"

glm::mat4 Consts::rotYMatrix(float angle) {
    glm::mat4 rotYMatrix = glm::mat4(1.0f);
    rotYMatrix[0][0] = cos(angle);
    rotYMatrix[0][2] = -sin(angle);
    rotYMatrix[2][0] = sin(angle);
    rotYMatrix[2][2] = cos(angle);
    return rotYMatrix;
}

glm::mat4 Consts::rotXMatrix(float angle) {
    glm::mat4 rotXMatrix = glm::mat4(1.0f);
    rotXMatrix[1][1] = cos(angle);
    rotXMatrix[1][2] = sin(angle);
    rotXMatrix[2][1] = -sin(angle);
    rotXMatrix[2][2] = cos(angle);
    return rotXMatrix;
}

glm::mat4 Consts::rotZMatrix(float angle) {
    glm::mat4 rotZMatrix = glm::mat4(1.0f);
    rotZMatrix[0][0] = cos(angle);
    rotZMatrix[0][1] = sin(angle);
    rotZMatrix[1][0] = -sin(angle);
    rotZMatrix[1][1] = cos(angle);
    return rotZMatrix;
}

glm::mat4 Consts::translateMatrix(float x, float y, float z) {
    glm::mat4 translateYMatrix = glm::mat4(1.0f);
    translateYMatrix[3][0] = x;
    translateYMatrix[3][1] = y;
    translateYMatrix[3][2] = z;
    return translateYMatrix;
}
