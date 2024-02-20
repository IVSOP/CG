#ifndef CG_BOX_H
#define CG_BOX_H

#include <vector>

#include "../common/Vertex.h"

class Box {
public:
    static std::vector<Vertex> createBoxPoints(int length, int divisions);
};


#endif //CG_BOX_H
