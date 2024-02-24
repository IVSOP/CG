#ifndef CG_CILINDER_H
#define CG_CILINDER_H

#include <vector>

#include "Vertex.h"

class Cylinder {
public:
    static std::vector<Vertex> createCylinderPoints(float radius, float height, int slices, int stacks);
};


#endif //CG_CILINDER_H
