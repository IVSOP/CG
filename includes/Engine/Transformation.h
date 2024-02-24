#ifndef CG_TRANSFORMATION_H
#define CG_TRANSFORMATION_H

#include "Translate.h"
#include "Rotate.h"
#include "Scale.h"

struct Transformation {
public:
    Translate t;
    Rotate r;
    Scale s;

    Transformation(Translate t){
        this->t = t;
    }

    Transformation(Rotate r){
        this->r = r;
    }

    Transformation(Scale s){
        this->s = s;
    }
};


#endif //CG_TRANSFORMATION_H
