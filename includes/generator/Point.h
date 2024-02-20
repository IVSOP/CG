#ifndef CG_POINT_H
#define CG_POINT_H

#include <iostream>

class Point {
private:
    int x;
    int y;
    int z;

    friend std::ostream &operator<<(std::ostream &os, const Point &p) {
        os << "Point: { x: " << p.x << ", y: " << p.y << ", z: " << p.z << " }";
        return os;
    }

public:
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
};

#endif //CG_POINT_H
