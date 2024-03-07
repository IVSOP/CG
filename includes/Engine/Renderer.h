#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vertex.h"
#include "Camera.h"

#include <vector>

class Renderer {
public:

	Renderer();
	~Renderer();

	GLuint VAO;
	GLuint vertexBuffer;
	GLuint program;
	GLint u_MVP;

	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, const glm::mat4 &perspective, Camera &camera, GLFWwindow * window) const;
	void drawAxis(const glm::mat4 &MVP) const; 
};


#endif
