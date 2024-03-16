#ifndef CG_VERTEX_H
#define CG_VERTEX_H

#include <iostream>
#include <glm/glm.hpp>

#include <GL/glew.h> // GLfloat

struct Vertex {
	// meti aqui os valores default senao ficava uma confusao enorme nos construtores
	glm::vec4 coords = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 normal = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec2 tex_coord = glm::vec2(0.0f, 0.0f);
	GLfloat material_id = 0.0f; // isto e uma float por simplicidade, vao ser poucos vertices e vao, senao podia ser uint8 ou 16

    friend std::ostream &operator<<(std::ostream &os, const Vertex &p) {
        os << "Vertex: { x: " << p.coords.x << ", y: " << p.coords.y << ", z: " << p.coords.z << " }";
        return os;
    }

	Vertex(float x, float y, float z, float normal_x, float normal_y, float normal_z, float tex_x, float tex_y, float _material_id)
	: coords(x, y, z, 1.0f), normal(normal_x, normal_y, normal_z), tex_coord(tex_x, tex_y), material_id(_material_id) {}
    Vertex(float x, float y, float z) : coords(x, y, z, 1.0f) {}
    Vertex() = default;

    //getters
    glm::vec4 &getCoords() {return coords;}


    float getX();
    float getY();
    float getZ();
};

struct AxisVertex {
	glm::vec4 coords = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	AxisVertex(float x, float y, float z, float R, float G, float B) : coords(x, y, z, 1.0f), color(R, G, B, 1.0f) {}
};

struct HDRVertex {
	glm::vec4 coords = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec2 tex_coord = glm::vec2(0.0f, 0.0f);

	HDRVertex(float x, float y, float z, float tex_x, float tex_y) : coords(x, y, z, 1.0f), tex_coord(tex_x, tex_y) {}
};

#endif //CG_VERTEX_H
