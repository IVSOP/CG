#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vertex.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureArray.h"
#include <memory>

#include <vector>

#define TEX_ARRAY_SLOT 0
#define HDR_TEXTURE_SLOT 1

#define MAX_MATERIALS 8
#define MAX_LIGHTS 8

class Renderer {
public:

	Renderer() = delete;
	Renderer(GLsizei viewport_width, GLsizei viewport_height);
	~Renderer();

	GLsizei viewport_width, viewport_height;

	GLuint VAO, VAO_axis, VAO_hdr;
	GLuint vertexBuffer, vertexBuffer_axis, vertexBuffer_hdr;
	Shader lightingShader, axisShader, hdrShader;
    
	GLint u_MaterialBufferBlockIndex;
	GLuint UBO_materials;

	GLuint hdrTexture = 0;
	GLuint hdrFBO = 0;

	GLfloat gamma = 2.2, exposure = 1.0;

	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, const glm::mat4 &perspective, Camera &camera, GLFWwindow * window); // const
	void drawAxis(const glm::mat4 &MVP);

	std::unique_ptr<TextureArray> textureArray = nullptr; // pointer since it starts as null and gets initialized later. unique_ptr so it always gets deleted
	void loadShader(const char path[], GLenum shaderType, GLuint program) const;
	void loadTextures();
	void checkProgram(GLuint program);
	void resizeViewport(GLsizei viewport_width, GLsizei viewport_height);
	void generate_HDR_texture(); // makes the texture, needs to be called whenever viewport is resized (for now)
	void checkFrameBuffer(GLuint fbo);
};


#endif
