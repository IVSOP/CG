#ifndef CG_SCALE_H
#define CG_SCALE_H

#include "Consts.h"

struct Scale {
public:
    glm::mat4 matrix;
    int x, y, z;

    Scale(float x, float y, float z){
        this->matrix = Consts::scaleMatrix(x, y, z);
    }

    Scale(){
        this->matrix = Consts::scaleMatrix(1, 1, 1);
    }

    glm::mat4 getMatrix() {
        return this->matrix;
    }
};


#endif //CG_SCALE_H
