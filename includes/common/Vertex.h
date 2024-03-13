#ifndef CG_VERTEX_H
#define CG_VERTEX_H

#include <iostream>
#include <glm/vec4.hpp>

struct Vertex {
	glm::vec4 coords;
	glm::vec4 color;

    friend std::ostream &operator<<(std::ostream &os, const Vertex &p) {
        os << "Vertex: { x: " << p.coords.x << ", y: " << p.coords.y << ", z: " << p.coords.z << " }";
        return os;
    }

	Vertex(float x, float y, float z, float R, float G, float B) : coords(x, y, z, 1.0f), color(R, G, B, 1.0f) {}
    Vertex(float x, float y, float z) : coords(x, y, z, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f) {}
    Vertex() : coords(0.0f, 0.0f, 0.0f, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f) {}

    //getters
    glm::vec4 &getCoords() {return coords;}


    float getX();
    float getY();
    float getZ();
};

#endif //CG_VERTEX_H
