#ifndef CG_ENGINE_OBJECT_H
#define CG_ENGINE_OBJECT_H

#include <vector>

#include "Transformation.h"
#include "Vertex.h"

class Engine_Object{
public:
    std::vector<Transformation> transformations;
    std::vector<Vertex> points;
    std::vector<Engine_Object> engine_objects;

    Engine_Object(std::vector<Transformation> transformations, std::vector<Vertex> points, std::vector<Engine_Object> engine_objects){
        this->transformations = std::vector<Transformation>(transformations);
        this->points = std::vector<Vertex>(points);
        this->engine_objects = std::vector<Engine_Object>(engine_objects);
    }
};

#endif //CG_ENGINE_OBJECT_H
