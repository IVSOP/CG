/////////////////////////////////////////////////////////////////////
/*
					Use this only for debugging
*/
////////////////////////////////////////////////////////////////////
#ifndef BASICRENDERER_H
#define BASICRENDERER_H

#include "common.h"
#include "Vertex.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureArray.h"
#include <memory>

#include <vector>

struct BasicVertex {
	glm::vec4 coords;
	glm::vec4 color;
	glm::vec2 tex_coord;
	GLfloat tex_id;

	BasicVertex() = default;

	BasicVertex(glm::vec4 coords, glm::vec4 color, glm::vec2 tex_coord, GLfloat tex_id)
	: coords(coords), color(color), tex_coord(tex_coord), tex_id(tex_id) {}
};

class BasicRenderer {
public:
	BasicRenderer();
	~BasicRenderer();

	void draw(std::vector<Vertex> &verts, const glm::mat4 &perspective, Camera &camera, GLFWwindow * window);
	GLsizei viewport_width, viewport_height;

	GLuint VAO, vertexBuffer;
	std::unique_ptr<TextureArray> textureArray = nullptr; // pointer since it starts as null and gets initialized later. unique_ptr so it always gets deleted
	Shader basic;
};


#endif
