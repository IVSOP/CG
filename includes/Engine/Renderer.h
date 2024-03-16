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

	GLuint VAO, VAO_axis;
	GLuint vertexBuffer, vertexBuffer_axis;
	GLuint program, program_axis;
    
	GLint u_MVP, u_View, u_MaterialBufferBlockIndex, u_TextureArraySlot, u_MVP_axis;
	GLint u_PointLight_position, u_PointLight_constant, u_PointLight_linear, u_PointLight_quadratic, u_PointLight_ambient, u_PointLight_diffuse, u_PointLight_specular;
	GLuint UBO_materials;

	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, const glm::mat4 &perspective, Camera &camera, GLFWwindow * window) const;
	void drawAxis(const glm::mat4 &MVP) const;

	std::unique_ptr<TextureArray> textureArray = nullptr; // pointer since it starts as null and gets initialized later. unique_ptr so it always gets deleted
	void loadShader(const char path[], GLenum shaderType, GLuint program) const;
	void loadTextures();
	void checkProgram(GLuint program);
};


#endif
