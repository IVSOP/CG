#ifndef CG_TORUS_H
#define CG_TORUS_H

#include <vector>

#include "../common/Vertex.h"

class Torus {
public:
    static std::vector<Vertex> createTorusPoints(float intRadius, float extRadius, int slices, int stacks);
};


#endif //CG_TORUS_H
