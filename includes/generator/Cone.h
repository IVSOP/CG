#ifndef CG_CONE_H
#define CG_CONE_H

#include <vector>

#include "Point.h"

class Cone {
public:
    static std::vector<Point> createConePoints(int radius, int height, int slices, int stacks);
};


#endif //CG_CONE_H
