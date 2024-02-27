#include "Consts.h"

// TODO talvez apenas importar em windows?????????
#define _USE_MATH_DEFINES
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

glm::mat4 Consts::translateMatrix(float x, float y, float z) {
    glm::mat4 translateYMatrix = glm::mat4(1.0f);

    translateYMatrix[3][0] = x;
    translateYMatrix[3][1] = y;
    translateYMatrix[3][2] = z;

    return translateYMatrix;
}

glm::mat4 Consts::idMatrix(){
    return glm::mat4(1.0f);
}

// basiei-me aqui http://www.songho.ca/opengl/gl_rotate.html
// rodrigues rotation formula -> mais eficiente que decompor rotações em torno de eixos x,y,z
// matriz é transposta em relação ao que ta no site porque OpenGL usa column major order
glm::mat4 Consts::rotMatrixVector(float angle, float x, float y, float z){
    glm::mat4 rotMatrix = glm::mat4(1.0f);
    float rad_angle = angle * (M_PI / 180);
    double c = cos(rad_angle);
    double s = sin(rad_angle);
    double oneMinusCos = 1 - c;

    rotMatrix[0][0] = oneMinusCos * x * x + c ;
    rotMatrix[0][1] = oneMinusCos * x * y  + s * z;
    rotMatrix[0][2] = oneMinusCos * x * z - s * y;
    rotMatrix[1][0] = oneMinusCos * x * y - s * z;
    rotMatrix[1][1] = oneMinusCos * y * y  + c;
    rotMatrix[1][2] = oneMinusCos * y * z + s * x;
    rotMatrix[2][0] = oneMinusCos * x * z + s * y;
    rotMatrix[2][1] = oneMinusCos * y * z - s * x;
    rotMatrix[2][2] = oneMinusCos * z * z + c;
    return rotMatrix;
}

glm::mat4 Consts::scaleMatrix(float x, float y, float z) {
    glm::mat4 translateYMatrix = glm::mat4(1.0f);

    translateYMatrix[0][0] = x;
    translateYMatrix[1][1] = y;
    translateYMatrix[2][2] = z;

    return translateYMatrix;
}
