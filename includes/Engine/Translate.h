#ifndef CG_TRANSLATE_H
#define CG_TRANSLATE_H

#include "Consts.h"

struct Translate {
public:
    glm::mat4 matrix;

    Translate(float x, float y, float z){
        this->matrix = Consts::translateMatrix(x, y, z);
    }

    Translate(){
        this->matrix = Consts::translateMatrix(0, 0, 0);
    }

    glm::mat4 getMatrix() {
        return this->matrix;
    }
};


#endif //CG_TRANSLATE_H
