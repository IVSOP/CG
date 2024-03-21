#ifndef CG_PLANE_H
#define CG_PLANE_H

#include <vector>

#include "../common/Vertex.h"

class Plane {
public:
    static std::vector<Vertex> createPlanePoints(const float length, const int divisions);
    static std::vector<Vertex> createPlanePointsNoTranslate(const float length, const int divisions);

    static std::vector<Vertex> createPlanePoints_TextSquareBySquare(const float length, const int divisions);
    static std::vector<Vertex> createPlanePoints_TextWholeObj(const float length, const int divisions);
};


#endif //CG_PLANE_H
