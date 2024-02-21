#ifndef CG_PLANE_H
#define CG_PLANE_H

#include <vector>

#include "../common/Vertex.h"

class Plane {
public:
    static std::vector<Vertex> createPlane(int length, int divisions);
    static std::vector<Vertex> createPlaneTriangles(std::vector<Vertex> &points, int divisions);
    static std::vector<Vertex> createPlanePoints(int length, int divisions);
    static std::vector<Vertex> createPlanePointsNoTranslate(int length, int divisions);
};


#endif //CG_PLANE_H
