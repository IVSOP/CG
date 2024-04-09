#ifndef CG_TRANSFORMATION_H
#define CG_TRANSFORMATION_H

#include "Vertex.h"
#include "Consts.h"
#include "common.h"

#include <vector>

class Transform {
public:
    virtual glm::mat4 getMatrix(float t) = 0;
};

class Translate : public Transform {
public:
    float time, x, y, z;
    bool align, curve;
    std::vector<Vertex> curvePoints;

    Translate(float time, bool align, float x, float y, float z, bool curve, std::vector<Vertex>& curvePoints):
            time(time), align(align), x(x), y(y), z(z), curve(curve), curvePoints(curvePoints){}

    glm::mat4 getMatrix(float t) override {
        if(!this->curve) return Consts::translateMatrix(this->x, this->y, this->z);

        t %= this->time;

        return glm::mat4(1.0f);
    }
};

class Rotate : public Transform {
public:
    float angle, time, x, y, z;
    bool curve;

    Rotate(float angle, float time, float x, float y, float z, bool curve) : angle(angle), time(time), x(x), y(y), z(z),
                                                                             curve(curve) {}

    glm::mat4 getMatrix(float t) override {
        if(this->curve){
            t %= this->time;
            this->angle = 360.0f * t / this->time;
        }

        return Consts::rotMatrixVector(angle, x, y, z);
    }
};

class Scale : public Transform {
public:
    glm::mat4 matrix;
    int x, y, z;

    Scale(float x, float y, float z){
        this->matrix = Consts::scaleMatrix(x, y, z);
    }

    Scale(){
        this->matrix = Consts::scaleMatrix(1, 1, 1);
    }

    glm::mat4 getMatrix(float t) override {
        return this->matrix;
    }
};

struct Transformation {
public:
    glm::mat4 transformMatrix;

    Transformation()
    : transformMatrix(Consts::idMatrix()) {}
    
    explicit Transformation(Transform& transform, float t){
        this->transformMatrix = transform.getMatrix(t);
    }

    void appendTransformation(glm::mat4& transformation);

    void appendTransformation(Transform& transform, float t);

    std::vector<Vertex> apply(std::vector<Vertex>& points);
};


#endif //CG_TRANSFORMATION_H
