#ifndef CG_BOX_H
#define CG_BOX_H

#include <vector>

#include "Point.h"

class Box {
public:
    static std::vector<Point> createBoxPoints(int length, int divisions);
};


#endif //CG_BOX_H
