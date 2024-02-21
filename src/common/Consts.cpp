#include "Consts.h"
#include <math.h>

glm::mat4 Consts::rotYMatrix(float angle) {
    float rad_angle = angle * (M_PI / 180);
    glm::mat4 rotYMatrix = glm::mat4(1.0f);
    rotYMatrix[0][0] = cos(rad_angle);
    rotYMatrix[0][2] = -sin(rad_angle);
    rotYMatrix[2][0] = sin(rad_angle);
    rotYMatrix[2][2] = cos(rad_angle);
    return rotYMatrix;
}

glm::mat4 Consts::rotXMatrix(float angle) {
    float rad_angle = angle * (M_PI / 180);
    glm::mat4 rotXMatrix = glm::mat4(1.0f);
    rotXMatrix[1][1] = cos(rad_angle);
    rotXMatrix[1][2] = sin(rad_angle);
    rotXMatrix[2][1] = -sin(rad_angle);
    rotXMatrix[2][2] = cos(rad_angle);
    return rotXMatrix;
}

glm::mat4 Consts::rotZMatrix(float angle) {
    float rad_angle = angle * (M_PI / 180);
    glm::mat4 rotZMatrix = glm::mat4(1.0f);
    rotZMatrix[0][0] = cos(rad_angle);
    rotZMatrix[0][1] = sin(rad_angle);
    rotZMatrix[1][0] = -sin(rad_angle);
    rotZMatrix[1][1] = cos(rad_angle);
    return rotZMatrix;
}


