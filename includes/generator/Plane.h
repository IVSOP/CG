#ifndef CG_PLANE_H
#define CG_PLANE_H

#include <vector>

#include "Vertex.h"

class Plane {
public:
    static std::vector<Vertex> createPlanePoints(int length, int divisions);
};


#endif //CG_PLANE_H
