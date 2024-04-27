#ifndef CG_VERTEX_H
#define CG_VERTEX_H

#include <iostream>
#include <glm/glm.hpp>

#include <GL/glew.h> // GLfloat

struct Vertex {
	// Meti aqui os valores default senao ficava uma confusao enorme nos construtores
	glm::vec4 coords = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 normal = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 tex_coord = glm::vec2(0.0f, 0.0f);
	GLint object_id = 0;

    friend std::ostream &operator<<(std::ostream &os, const Vertex &p) {
        os << "Vertex: { {x: " << p.coords.x << ", y: " << p.coords.y << ", z: " << p.coords.z << "}, {n_x: " << p.normal.x << ", n_y: " << p.normal.y << ", n_z: " << p.normal.z << "}, {t_x: " << p.tex_coord.x << ", t_y: " << p.tex_coord.y << "}, " << p.object_id << " }";
        return os;
    }

	Vertex(float x, float y, float z, float normal_x, float normal_y, float normal_z, float tex_x, float tex_y, GLint _object_id)
	: coords(x, y, z, 1.0f), normal(normal_x, normal_y, normal_z), tex_coord(tex_x, tex_y), object_id(_object_id) {}

    Vertex(float x, float y, float z, float normal_x, float normal_y, float normal_z, float tex_x, float tex_y)
            : coords(x, y, z, 1.0f), normal(normal_x, normal_y, normal_z), tex_coord(tex_x, tex_y) {}

    Vertex(float x, float y, float z, float normal_x, float normal_y, float normal_z)
            : coords(x, y, z, 1.0f), normal(normal_x, normal_y, normal_z) {}

    Vertex(float x, float y, float z) : coords(x, y, z, 1.0f) {}

    Vertex(glm::vec4 coords, glm::vec3 normal, glm::vec2 tex_coord) : coords(coords), normal(normal), tex_coord(tex_coord) {}
	Vertex(glm::vec4 coords, glm::vec3 normal, glm::vec2 tex_coord, GLint _object_id) : coords(coords), normal(normal), tex_coord(tex_coord), object_id(_object_id) {}

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

	AxisVertex() : coords(0.0f, 0.0f, 0.0f, 1.0f), color(0.0f, 0.0f, 0.0f, 1.0f) {}
	AxisVertex(float x, float y, float z, float R, float G, float B) : coords(x, y, z, 1.0f), color(R, G, B, 1.0f) {}
};

struct ViewportVertex {
	glm::vec4 coords = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec2 tex_coord = glm::vec2(0.0f, 0.0f);

	ViewportVertex(float x, float y, float z, float tex_x, float tex_y) : coords(x, y, z, 1.0f), tex_coord(tex_x, tex_y) {}
};

struct SunVertex {
	glm::vec4 coords = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec2 tex_coord = glm::vec2(0.0f, 0.0f);

	SunVertex(const Vertex &vertex)
	: coords(vertex.coords), tex_coord(vertex.tex_coord) {}
};

#endif //CG_VERTEX_H
