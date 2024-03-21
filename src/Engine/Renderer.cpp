#include "Renderer.h"

#include <iostream>
#include <vector>
#include <cstdio>

#include "Vertex.h"
#include "Material.h"
#include "Camera.h"

struct PointLight {
    glm::vec3 position;

    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;  

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

// quad filling entire screen
ViewportVertex viewportVertices[] = {
	{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
	{1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}
};

void Renderer::drawAxis(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
	const AxisVertex vertices[] = {
		// x
		{-100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},

		// y
		{0.0f, -100.0f, 0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f},

		// z
		{0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 1.0f}
	};

	// bind VAO, VBO
	GLCall(glBindVertexArray(this->VAO_axis));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_axis));

	// load vertices
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	axisShader.use();
	// axisShader.setMat4("u_Model", model);
	// axisShader.setMat4("u_View", view);
	// axisShader.setMat4("u_Projection", projection);
	axisShader.setMat4("u_MVP", projection * view * model);

	GLCall(glDrawArrays(GL_LINES, 0, 6)); // 6 pontos, 3 linhas
}

// the vertices are copied on purpose!!!
void Renderer::drawNormals(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection, const std::vector<Vertex> &vertices) {

	// VAO and VBO are the same as the normal ones, so I will not rebind them

	normalsShader.use();
	normalsShader.setMat4("u_Model", model);
	normalsShader.setMat4("u_View", view);
	normalsShader.setMat4("u_Projection", projection);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, vertices.size()));
}

Renderer::Renderer(GLsizei viewport_width, GLsizei viewport_height)
: viewport_width(viewport_width), viewport_height(viewport_height), VAO(0), vertexBuffer(0),
  lightingShader("shaders/lighting_extract_brightness_with_gs.vert", "shaders/lighting_extract_brightness_with_gs.frag", "shaders/explode.gs"),
  axisShader("shaders/basic.vert", "shaders/basic.frag"),
  normalsShader("shaders/normals.vert", "shaders/normals.frag", "shaders/normals.gs"),
  blurShader("shaders/blur.vert", "shaders/blur.frag"),
  hdrBbloomMergeShader("shaders/hdrBloomMerge.vert", "shaders/hdrBloomMerge.frag")
{
	//////////////////////////// LOADING VAO ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO));
	GLCall(glBindVertexArray(this->VAO));

	//////////////////////////// LOADING VBO ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_normal_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_normal_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_normal_layout, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, normal)));
		// GLCall(glVertexAttribDivisor(vertex_normal_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 2;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex

		GLuint vertex_matid_layout = 3;
		GLCall(glEnableVertexAttribArray(vertex_matid_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_matid_layout, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, material_id)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per triangle, but I am not using instancing
	}

	//////////////////////////// LOADING VAO FOR AXIS ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO_axis));
	GLCall(glBindVertexArray(this->VAO_axis));

	//////////////////////////// LOADING VBO FOR AXIS ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer_axis));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_axis));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), (const void *)offsetof(AxisVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_color_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_color_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_color_layout, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), (const void *)offsetof(AxisVertex, color)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex
	}

	//////////////////////////// LOADING VAO FOR HDR ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO_viewport));
	GLCall(glBindVertexArray(this->VAO_viewport));

	//////////////////////////// LOADING VBO FOR HDR ////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer_viewport));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_viewport));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void *)offsetof(ViewportVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void *)offsetof(ViewportVertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_normal_layout, 0)); // values are per vertex
	}

	//////////////////////////// LOADING SHADER UNIFORMS ///////////////////////////
	lightingShader.use();
	lightingShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
	lightingShader.setMat4("u_Model", glm::mat4(1.0f)); // load identity just for safety
	lightingShader.setMat4("u_View", glm::mat4(1.0f)); // load identity just for safety
	lightingShader.setMat4("u_Projection", glm::mat4(1.0f)); // load identity just for safety


	GLCall(glGenBuffers(1, &materialBuffer));
	GLCall(glBindBuffer(GL_TEXTURE_BUFFER, materialBuffer));
	GLCall(glGenTextures(1, &materialTBO));


	// for axis shader
	axisShader.use();
	// axisShader.setMat4("u_Model", glm::mat4(1.0f)); // load identity just for safety
	// axisShader.setMat4("u_View", glm::mat4(1.0f)); // load identity just for safety
	// axisShader.setMat4("u_Projection", glm::mat4(1.0f)); // load identity just for safety
	axisShader.setMat4("u_MVP", glm::mat4(1.0f));

	//////////////////////////// LOADING TEXTURES ///////////////////////////
	loadTextures();


	//////////////////////////// LOADING FRAMEBUFFERS AND TEXTURE ATTACHMENTS ////////////////////////////
	GLCall(glGenFramebuffers(1, &lightingFBO));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
		generate_FBO_texture(&lightingTexture, GL_COLOR_ATTACHMENT0);
		generate_FBO_texture(&brightTexture, GL_COLOR_ATTACHMENT1);
		GLCall(checkFrameBuffer());
	GLCall(glGenFramebuffers(2, pingpongFBO));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
		generate_FBO_texture(&pingpongTextures[0], GL_COLOR_ATTACHMENT0);
		GLCall(checkFrameBuffer());
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
		generate_FBO_texture(&pingpongTextures[1], GL_COLOR_ATTACHMENT0);
		GLCall(checkFrameBuffer());
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));



	//////////////////////////// CLEANUP ///////////////////////////
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

Renderer::~Renderer() {
	GLCall(glDeleteBuffers(1, &vertexBuffer));
	GLCall(glDeleteBuffers(1, &materialBuffer));
	GLCall(glDeleteBuffers(1, &vertexBuffer_axis));
	GLCall(glDeleteBuffers(1, &vertexBuffer_viewport));

	GLCall(glDeleteVertexArrays(1, &VAO));
	GLCall(glDeleteVertexArrays(1, &VAO_axis));
	GLCall(glDeleteVertexArrays(1, &VAO_viewport));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glDeleteTextures(1, &lightingTexture));
	GLCall(glDeleteTextures(1, &materialBuffer));
	GLCall(glDeleteTextures(2, pingpongTextures));

	GLCall(glDeleteFramebuffers(1, &lightingFBO));
	GLCall(glDeleteFramebuffers(2, pingpongFBO));
}

void Renderer::loadShader(const char path[], GLenum shaderType, GLuint _program) const {
	const GLchar *buff = readFromFile(path);
	GLCall(GLuint shader = glCreateShader(shaderType));
	GLCall(glShaderSource(shader, 1, &buff, NULL)); // ??? why &buff? does it set it to null on error??
	GLCall(glCompileShader(shader));
	GLCall(checkErrorInShader(shader));
	GLCall(glAttachShader(_program, shader));
	delete[] buff;

	GLCall(glDeleteShader(shader));
}

void Renderer::checkProgram(GLuint program) {
	GLCall(glLinkProgram(program));
		checkProgramLinking(program);
		validateProgram(program);
}

void Renderer::loadTextures() {
	// load texture array instance
	this->textureArray = std::make_unique<TextureArray>(TEXTURE_WIDTH, TEXTURE_HEIGHT, TEXTURE_LAYERS);

	TextureArray *tex = textureArray.get();
	tex->addTexture("textures/missing_texture.png"); // 0
	tex->addTexture("textures/black.png"); // 1
	tex->addTexture("textures/grass.png"); // 2
	tex->addTexture("textures/oak_log.png"); // 3
	tex->addTexture("textures/white.png"); // 4

	tex->setTextureArrayToSlot(TEX_ARRAY_SLOT);
}

void Renderer::draw(std::vector<Vertex> &verts, const glm::mat4 &projection, Camera &camera, GLFWwindow * window) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	// ImGui::ShowDemoWindow();
	ImGui::Text("Facing x:%f y:%f z:%f", camera.Front.x, camera.Front.y, camera.Front.z);
	ImGui::InputFloat3("Position:", glm::value_ptr(camera.Position));
	ImGui::SliderFloat("gamma", &gamma, 0.0f, 10.0f, "gamma = %.3f");
	ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f, "exposure = %.3f");
	ImGui::SliderFloat("bloomThreshold", &bloomThreshold, 0.0f, 5.0f, "bloomThreshold = %.3f");
	// texOffsetCoeff = static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX) * 10.0f;
	ImGui::SliderFloat("texOffsetCoeff", &texOffsetCoeff, 0.0f, 10.0f, "texOffsetCoeff = %.3f");
	ImGui::Checkbox("Show normals", &showNormals);
	ImGui::Checkbox("Explode", &explode);

	constexpr glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = camera.GetViewMatrix();
	// const glm::mat4 MVP = projection * view * model;

	//////////////////////////////////////////////// 1. the normal scene is drawn into the lighting framebuffer, where the bright colors are then separated
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// GLCall(glPolygonMode(GL_FRONT_AND_BACK,GL_LINE));

		// drawAxis(MVP);

		GLCall(glBindVertexArray(this->VAO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));

		// load vertices
		GLCall(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW));

		// bind program, load uniforms
		lightingShader.use();

		// load MVP, texture array and view
		this->textureArray.get()->setTextureArrayToSlot(TEX_ARRAY_SLOT);
		lightingShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
		lightingShader.setMat4("u_Model", model);
		lightingShader.setMat4("u_View", view);
		lightingShader.setMat4("u_Projection", projection);

		lightingShader.setFloat("u_BloomThreshold", bloomThreshold);

		// load UBO
		Material materials[8];
		materials[0] = {
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			// glm::vec3(2.99f, 0.72f, 0.0745f),
			glm::vec3(0.1f),
			32.0f,
			1
		};

		GLCall(glBindBuffer(GL_TEXTURE_BUFFER, materialBuffer));
		GLCall(glBufferData(GL_TEXTURE_BUFFER, MAX_MATERIALS * sizeof(Material), materials, GL_STATIC_DRAW));
		GLCall(glActiveTexture(GL_TEXTURE0 + MATERIAL_TEXTURE_BUFFER_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_BUFFER, materialTBO));
		GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, materialBuffer)); // bind the buffer to the texture
		lightingShader.setInt("u_MaterialTBO", MATERIAL_TEXTURE_BUFFER_SLOT);

		// load test light
		lightingShader.setFloat("u_PointLight.constant", 1.0f);
		lightingShader.setFloat("u_PointLight.linear", 0.09f);
		lightingShader.setFloat("u_PointLight.quadratic", 0.032f);
		lightingShader.setVec3("u_PointLight.position", 0.0f, 2.0f, 5.0f);
		lightingShader.setVec3("u_PointLight.ambient", 0.2f, 0.2f, 0.0f);
		lightingShader.setVec3("u_PointLight.diffuse", 0.78f, 0.78f, 0.0f);
		lightingShader.setVec3("u_PointLight.specular", 1.0f, 1.0f, 1.0f);
		// lightingShader.setVec3("u_PointLight.ambient", 0.0f, 0.0f, 0.0f);
		// lightingShader.setVec3("u_PointLight.diffuse", 0.0f, 0.0f, 0.0f);
		// lightingShader.setVec3("u_PointLight.specular", 0.0f, 0.0f, 0.0f);

		// specify 2 attachments
		constexpr GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		GLCall(glDrawBuffers(2, attachments));

		lightingShader.setFloat("u_ExplodeCoeff", explodeCoeff);
		if (explode) {
			ImGui::SliderFloat("explodeCoeff", &explodeCoeff, 0.0f, 10.0f, "explodeCoeff = %.3f");
		} else {
			if (explodeCoeff > 0.0f) {
				lightingShader.setFloat("u_ExplodeCoeff", 0.0f);
			}
		}

		// lightingShader.validate();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, verts.size()));
	
		if (showNormals) {
			drawNormals(model, view, projection, verts);
		}

	//////////////////////////////////////////////// 2. now, we run the ping pong gaussian blur several times
	blurShader.use();
	GLCall(glBindVertexArray(this->VAO_viewport));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_viewport));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(viewportVertices), viewportVertices, GL_STATIC_DRAW));
	constexpr GLint amount = 5; // 5 each

	// manually doing the first passes, since I need to get the texture from the scene
	// horizontal pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		blurShader.setInt("u_Horizontal", 0);
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, this->brightTexture)); // use texture from scene
		blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	// vertical pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		blurShader.setInt("u_Horizontal", 1);
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[0])); // use texture from ping
		blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	for (GLint i = 0; i < amount - 1; i++) {
		blurShader.setFloat("u_TexOffsetCoeff", texOffsetCoeff);

		// horizontal pass
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
			blurShader.setInt("u_Horizontal", 0);
			GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
			GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[1])); // use texture from pong
			blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

		// vertical pass
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
			blurShader.setInt("u_Horizontal", 1);
			GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
			GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[0])); // use texture from ping
			blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	}

	//////////////////////////////////////////////// 3. finally, we join the blur to the scene and apply gamma and exposure
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GLCall(glBindVertexArray(this->VAO_viewport));
		// GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_viewport));
		// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(viewportVertices), viewportVertices, GL_STATIC_DRAW));

		hdrBbloomMergeShader.use();
		GLCall(glActiveTexture(GL_TEXTURE0 + SCENE_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, this->lightingTexture));
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[1]));

		hdrBbloomMergeShader.setInt("u_SceneBuffer", SCENE_TEXTURE_SLOT);
		hdrBbloomMergeShader.setInt("u_BrightBuffer", BRIGHT_TEXTURE_SLOT);
		hdrBbloomMergeShader.setFloat("u_Gamma", gamma);
		hdrBbloomMergeShader.setFloat("u_Exposure", exposure);

		// hdrBbloomMergeShader.validate();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));


	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

// this does NOT take into acount currently used textures slots etc, only here for organisation
// make sure fbo is bound before calling this
void Renderer::generate_FBO_texture(GLuint *textureID, GLenum attachmentID) {
	// delete existing texture, if needed
	if (*textureID != 0) { // for safety, delete the texture entirely. maybe does not need to be done
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glDeleteTextures(1, textureID));
	}

	GLCall(glGenTextures(1, textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, *textureID));
	// you can get the default behaviour by either not using the framebuffer or setting this to GL_RGBA
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->viewport_width, this->viewport_height, 0, GL_RGBA, GL_FLOAT, NULL));  // change to 32float if needed

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// attach to fbo
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentID, GL_TEXTURE_2D, *textureID, 0));
}

// regenerate the textures for all the FBOs
void Renderer::resizeViewport(GLsizei viewport_width, GLsizei viewport_height) {
	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
	generate_FBO_texture(&lightingTexture, GL_COLOR_ATTACHMENT0);
	generate_FBO_texture(&brightTexture, GL_COLOR_ATTACHMENT1);
	GLCall(checkFrameBuffer());

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
	generate_FBO_texture(&pingpongTextures[0], GL_COLOR_ATTACHMENT0);
	GLCall(checkFrameBuffer());
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
	generate_FBO_texture(&pingpongTextures[1], GL_COLOR_ATTACHMENT0);
	GLCall(checkFrameBuffer());

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// needs to be improved
void Renderer::checkFrameBuffer() {
	GLCall(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

	if (status != GL_FRAMEBUFFER_COMPLETE) {
    	puts("Error in fbo");
		exit(1);
	}
}
