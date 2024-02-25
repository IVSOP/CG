#ifndef CG_ENGINE_OBJECT_H
#define CG_ENGINE_OBJECT_H

#include <vector>

#include "Transformation.h"
#include "Vertex.h"

class Engine_Object{
public:
    Transformation transformation;
    std::vector<Vertex> points;
    std::vector<Engine_Object> children_objects;

    Engine_Object(){
        this->transformation = Transformation();
        this->points = std::vector<Vertex>();
        this->children_objects = std::vector<Engine_Object>();
    }

    Engine_Object(const Transformation& transformation, std::vector<Vertex>& points, std::vector<Engine_Object>& children_objects){
        this->transformation = transformation;
        this->points = std::move(points);
        this->children_objects = std::move(children_objects);
    }

    std::vector<Vertex> getPoints();
};

#endif //CG_ENGINE_OBJECT_H
