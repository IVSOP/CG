#ifndef CG_SPHERE_H
#define CG_SPHERE_H

#include <vector>

#include "Vertex.h"

class Sphere {
public:
    static std::vector<Vertex> createSpherePoints(int radius, int slices, int stacks);
};


#endif //CG_SPHERE_H
