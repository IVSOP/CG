#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vertex.h"
#include "Camera.h"
#include "TextureArray.h"
#include <memory>

#include <vector>

#define TEX_ARRAY_SLOT 0

class Renderer {
public:

	Renderer();
	~Renderer();

	GLuint VAO;
	GLuint vertexBuffer;
	GLuint program;
	GLint u_MVP, u_TextureArraySlot;

	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, const glm::mat4 &perspective, Camera &camera, GLFWwindow * window) const;
	void drawAxis(const glm::mat4 &MVP) const;

	std::unique_ptr<TextureArray> textureArray = nullptr; // pointer since it starts as null and gets initialized later. unique_ptr so it always gets deleted
	void loadTextures();
};


#endif
