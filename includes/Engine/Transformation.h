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
private:
    glm::mat4 transformMatrix{};

public:
    Transformation(){
        this->transformMatrix = Consts::idMatrix();
    }

    // Transformation(Transformation& transformation){
    //     this->transformMatrix = glm::mat4 (transformation.transformMatrix);
    // }

    explicit Transformation(Translate& translate){
        this->transformMatrix = translate.getMatrix();
    }

    explicit Transformation(Rotate& rotate){
        this->transformMatrix = rotate.getMatrix();
    }

    explicit Transformation(Scale& scale){
        this->transformMatrix = scale.getMatrix();
    }

    // Transformation(Transformation const &transformation) : transformMatrix(transformation.transformMatrix){}

    void appendTransformation(glm::mat4& transformation);

    void appendTransformation(Transformation& transformation);

    std::vector<Vertex> apply(std::vector<Vertex>& points);
};


#endif //CG_TRANSFORMATION_H
