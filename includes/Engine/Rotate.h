#ifndef CG_ROTATE_H
#define CG_ROTATE_H

#include "Consts.h"

struct Rotate {
public:
    glm::mat4 matrix;

    Rotate(float angle, float x, float y, float z){
        this->matrix = Consts::rotMatrixVector(angle, x, y, z);
    }

    Rotate(){
        this->matrix = Consts::rotMatrixVector(0, 0, 0, 0);
    }

    glm::mat4 getMatrix() {
        return this->matrix;
    }
};


#endif //CG_ROTATE_H
