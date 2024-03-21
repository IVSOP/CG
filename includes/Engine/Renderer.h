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
#define BRIGHT_TEXTURE_SLOT 1 // slot to go into blur shader and final bloom shader
#define SCENE_TEXTURE_SLOT 2 // slot to go into final bloom shader
#define MATERIAL_TEXTURE_BUFFER_SLOT 3

#define MAX_MATERIALS 8
#define MAX_LIGHTS 8

class Renderer {
public:

	Renderer() = delete;
	Renderer(GLsizei viewport_width, GLsizei viewport_height);
	~Renderer();

	GLsizei viewport_width, viewport_height;
    
	GLuint materialBuffer, materialTBO;


	// TODO test: apply gamma and exposure and only then extract colors

	// lighting FBO into wich scene gets rendered normally, but bright colors are extracted for bloom
	GLuint VAO, VAO_axis;
	GLuint vertexBuffer, vertexBuffer_axis;
	Shader lightingShader, axisShader, normalsShader;
	GLuint lightingFBO = 0;
	GLuint lightingTexture = 0; // color atttachment 0, scene renders into this
	GLuint brightTexture = 0; // color atttachment 1, extraction of brightly lit areas

	// ping pong FBOs, to perform two-pass Gaussian blur on the extracted bright colors
	GLuint VAO_viewport;
	GLuint vertexBuffer_viewport;
	GLuint pingpongFBO[2];
	GLuint pingpongTextures[2];
	Shader blurShader;

	// finally, in the normal FBO we merge the resulting blur with the original scene and apply gamma and exposure to everything at once
	// GLuint VAO_viewport;
	// GLuint vertexBuffer_viewport;
	Shader hdrBbloomMergeShader;
	// !! could have reused fbo and textures, but this is simpler and more flexible and less painful to manage
	

	GLfloat gamma = 2.2f, exposure = 1.0f, bloomThreshold = 1.0f, texOffsetCoeff = 1.0f, explodeCoeff = 0.0f;
	bool showAxis = false;
	bool showNormals = false;
	bool explode = false;




	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, const glm::mat4 &perspective, Camera &camera, GLFWwindow * window); // const
	void drawAxis(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection);
	void drawNormals(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection, const std::vector<Vertex> &vertices); // vector is copied over on purpose

	std::unique_ptr<TextureArray> textureArray = nullptr; // pointer since it starts as null and gets initialized later. unique_ptr so it always gets deleted
	void loadShader(const char path[], GLenum shaderType, GLuint program) const;
	void loadTextures();
	void checkProgram(GLuint program);
	void resizeViewport(GLsizei viewport_width, GLsizei viewport_height);
	void generate_FBO_texture(GLuint *textureID, GLenum attachmentID); // makes the texture, needs to be called whenever viewport is resized (for now)
	void checkFrameBuffer();
};


#endif
