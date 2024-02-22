#ifndef CG_CONE_H
#define CG_CONE_H

#include <vector>

#include "../common/Vertex.h"

class Cone {
public:
    static std::vector<Vertex> createConePoints(float radius, float height, int slices, int stacks);
};


#endif //CG_CONE_H
