#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vertex.h"
#include "Camera.h"
#include "Shader.h"
#include "RendererObjectInfo.h"
#include "TextureArray.h"

#include <memory>
#include <vector>
#include <unordered_map>

class Renderer {
public:

	Renderer() = delete;
	Renderer(GLsizei viewport_width, GLsizei viewport_height);
	~Renderer();

	std::unordered_map<std::string, GLfloat> texture_id_map;

	GLsizei viewport_width, viewport_height;

	GLuint objectInfoBuffer, objectInfoTBO;
	GLuint pointLightBuffer, pointLightTBO;
	GLuint dirLightBuffer, dirLightTBO;
	GLuint spotLightBuffer, spotLightTBO;


	// lighting FBO into wich scene gets rendered normally, but bright colors are extracted for bloom
	GLuint VAO, VAO_axis;
	GLuint vertexBuffer, vertexBuffer_axis;
	Shader lightingShader, axisShader, normalsShader;
	GLuint lightingFBO = 0, lightingFBODepthBuffer = 0;
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
	
				// default was 2.2 1.0
	GLfloat gamma = 1.0f, exposure = 1.0f, bloomThreshold = 1.0f, bloomOffset = 1.0f, explodeCoeff = 0.0f;
	int bloomBlurPasses = 0; // default was 5
	bool showAxis = false;
	bool showNormals = false;
	bool explode = false;
	bool limitFPS = false;
	double fps = 60.0f;
	bool showCurveNormals = false;

	std::unique_ptr<TextureArray> textureArray = nullptr; // pointer since it starts as null and gets initialized later. unique_ptr so it always gets deleted


	std::vector<RendererObjectInfo> translateEngineObjectInfo(const std::vector<Engine_Object_Info> &engineObjectInfo);
	void draw(const std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>>& curvePoints, const std::vector<Vertex> &verts, const std::vector<RendererObjectInfo> &objectInfo, const glm::mat4 &projection, Camera &camera, GLFWwindow * window, GLfloat deltatime); // const
	void drawAxis(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection);
	void drawNormals(const glm::mat4 &view, const glm::mat4 &projection, const std::vector<Vertex> &vertices); // vector is copied over on purpose
    void drawCurves(const glm::mat4 &view, const glm::mat4 &projection, const std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>>& points);

	GLfloat getTextureID(const std::string &name); // will load it into the texture buffer if it is not in the map
	void resizeViewport(GLsizei viewport_width, GLsizei viewport_height);
	void generate_FBO_depth_buffer(GLuint *depthBuffer) const;
	void generate_FBO_texture(GLuint *textureID, GLenum attachmentID); // makes the texture, needs to be called whenever viewport is resized (for now)
	void checkFrameBuffer();

private:
	void prepareFrame(Camera &camera, GLfloat deltatime);
	void drawLighting(const std::vector<Vertex> &verts, const std::vector<RendererObjectInfo> &objectInfo, const glm::mat4 &projection, const glm::mat4 &view, const Camera &camera); // camera is for debugging
	void bloomBlur(int passes);
	void merge();
	void endFrame(GLFWwindow * window);
};

#endif
