#ifndef CG_SPHERE_H
#define CG_SPHERE_H

#include <vector>

#include "../common/Vertex.h"

class Sphere {
public:
    static std::vector<Vertex> createSpherePoints(const float radius, const int slices, const int stacks);
};


#endif //CG_SPHERE_H
