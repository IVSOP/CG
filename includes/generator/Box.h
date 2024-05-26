#ifndef CG_BOX_H
#define CG_BOX_H

#include <vector>

#include "../common/Vertex.h"
#include "../common/Consts.h"

class Box {
public:
    static std::vector<Vertex> createBoxPoints(float length, int divisions);
};


#endif //CG_BOX_H
