#ifndef CG_VERTEX_H
#define CG_VERTEX_H

#include <iostream>

class Vertex {
private:
    float x;
    float y;
    float z;

    friend std::ostream &operator<<(std::ostream &os, const Vertex &p) {
        os << "Vertex: { x: " << p.x << ", y: " << p.y << ", z: " << p.z << " }";
        return os;
    }

public:
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
};

#endif //CG_VERTEX_H
