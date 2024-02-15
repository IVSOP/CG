#ifndef CG_PLANE_H
#define CG_PLANE_H

#include <vector>

#include "Point.h"

class Plane {
public:
    static std::vector<Point> createPlanePoints(int length, int divisions);
};


#endif //CG_PLANE_H
