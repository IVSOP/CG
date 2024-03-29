#ifndef CG_TRANSFORMATION_H
#define CG_TRANSFORMATION_H

#include "Vertex.h"
#include "Consts.h"
#include "common.h"
#include "Translate.h"
#include "Rotate.h"
#include "Scale.h"

#include <vector>

struct Transformation {
public:
    glm::mat4 transformMatrix;

    Transformation()
    : transformMatrix(Consts::idMatrix()) {}
    
    explicit Transformation(Translate& translate){
        this->transformMatrix = translate.getMatrix();
    }

    explicit Transformation(Rotate& rotate){
        this->transformMatrix = rotate.getMatrix();
    }

    explicit Transformation(Scale& scale){
        this->transformMatrix = scale.getMatrix();
    }

    void appendTransformation(glm::mat4& transformation);

    void appendTransformation(Transformation& transformation);

    std::vector<Vertex> apply(std::vector<Vertex>& points);
};


#endif //CG_TRANSFORMATION_H
