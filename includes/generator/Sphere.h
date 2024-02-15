#ifndef CG_SPHERE_H
#define CG_SPHERE_H

#include <vector>

#include "Point.h"

class Sphere {
public:
    static std::vector<Point> createSpherePoints(int radius, int slices, int stacks);
};


#endif //CG_SPHERE_H
