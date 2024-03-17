#include "Renderer.h"

#include <iostream>
#include <vector>
#include <cstdio>

#include "Vertex.h"
#include "Camera.h"

// TODO temporary
struct Material {
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	glm::vec4 emissive;
	glm::vec4 shininess;
	glm::uvec4 texture_id;
};

struct PointLight {
    glm::vec3 position;

    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;  

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


void Renderer::drawAxis(const glm::mat4 &MVP) {
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

	// bind program, load MVP and texture array slot
	axisShader.use();
	axisShader.setMat4("u_MVP", MVP);

	GLCall(glDrawArrays(GL_LINES, 0, 6)); // 6 pontos, 3 linhas
}

Renderer::Renderer(GLsizei viewport_width, GLsizei viewport_height)
: viewport_width(viewport_width), viewport_height(viewport_height), VAO(0), vertexBuffer(0),
  lightingShader("shaders/lighting.vert", "shaders/lighting.frag"), axisShader("shaders/basic.vert", "shaders/basic.frag"),
  hdrShader("shaders/hdr.vert", "shaders/hdr.frag")
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
		GLCall(glVertexAttribPointer(vertex_matid_layout, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, material_id)));
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
	GLCall(glGenVertexArrays(1, &this->VAO_hdr));
	GLCall(glBindVertexArray(this->VAO_hdr));

	//////////////////////////// LOADING VBO FOR HDR ////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer_hdr));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_hdr));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(HDRVertex), (const void *)offsetof(HDRVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(HDRVertex), (const void *)offsetof(HDRVertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_normal_layout, 0)); // values are per vertex
	}

	//////////////////////////// LOADING SHADERS ////////////////////////////	

	//////////////////////////// LOADING SHADER UNIFORMS ///////////////////////////
	lightingShader.use();
	lightingShader.setMat4("u_MVP", glm::mat4(1.0f)); // load identity just for safety
	lightingShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
	lightingShader.setMat4("u_View", glm::mat4(1.0f)); // load identity just for safety

	// this is a UBO, not like other uniforms
	// TODO do this in shader class somehow, doing it here since it is never used after
	GLCall(this->u_MaterialBufferBlockIndex = glGetUniformBlockIndex(lightingShader.programID, "u_MaterialBuffer")); // missing error checking, could be -1
	// bind to 0
	glUniformBlockBinding(lightingShader.programID, u_MaterialBufferBlockIndex, 0);
	// generate the UBO
	glGenBuffers(1, &UBO_materials);
	
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_materials);
	glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(Material), NULL, GL_DYNAMIC_DRAW); // pre alocate data. hardcoded 8 for now
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO_materials, 0, 8 * sizeof(Material));



	// for axis shader
	axisShader.use();
	axisShader.setMat4("u_MVP", glm::mat4(1.0f)); // load identity just for safety



	// for hdr shader
	hdrShader.use();
				// GLCall(this->u_HdrBuffer = glGetUniformLocation(program_hdr, "u_HdrBuffer"));
				// GLCall(this->u_Gamma = glGetUniformLocation(program_hdr, "u_Gamma"));
				// GLCall(this->u_Exposure = glGetUniformLocation(program_hdr, "u_Exposure"));


	//////////////////////////// LOADING TEXTURES ///////////////////////////
	loadTextures();


	//////////////////////////// LOADING FRAMEBUFFER AND HDR TEXTURE ////////////////////////////
	GLCall(glGenFramebuffers(1, &hdrFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
	generate_HDR_texture();
	GLCall(checkFrameBuffer(hdrFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));



	//////////////////////////// CLEANUP ///////////////////////////
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

Renderer::~Renderer() {
	GLCall(glDeleteBuffers(1, &vertexBuffer));
	GLCall(glDeleteBuffers(1, &vertexBuffer_axis));
	GLCall(glDeleteBuffers(1, &vertexBuffer_hdr));
	GLCall(glDeleteVertexArrays(1, &VAO));
	GLCall(glDeleteVertexArrays(1, &VAO_axis));
	GLCall(glDeleteVertexArrays(1, &VAO_hdr));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glDeleteTextures(1, &hdrTexture));
	GLCall(glDeleteFramebuffers(1, &hdrFBO));
	// TODO do I need to delete the material UBO?????
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
    // Clear buffers // isto funcionava em baixo do ImGui::Begin, buguei
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	// ImGui::ShowDemoWindow();
	ImGui::Text("Facing x:%f y:%f z:%f", camera.Front.x, camera.Front.y, camera.Front.z);
	ImGui::InputFloat3("Position:", glm::value_ptr(camera.Position));
	ImGui::SliderFloat("gamma", &gamma, 0.0f, 10.0f, "gamma = %.3f");
	ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f, "exposure = %.3f");

	constexpr glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = camera.GetViewMatrix();
	const glm::mat4 MVP = projection * view * model; // nao so nao sei, como nao quero saber. vou assumir que a maneira que o glm faz as coisas influencia isto


	// bind VAO, VBO
	GLCall(glBindVertexArray(this->VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));

	// load vertices
	GLCall(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW));

	// bind program, load uniforms
	lightingShader.use();

	// load MVP, texture array and view
	lightingShader.setMat4("u_MVP", MVP);
	lightingShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
	lightingShader.setMat4("u_View", view);

	// load texture array for safety
	this->textureArray.get()->setTextureArrayToSlot(TEX_ARRAY_SLOT);

	// load UBO
	Material materials[8];

	// brown is (79,58,43) -> 0.31, 0.227, 0.169
	// materials[0].diffuse = glm::vec4(0.31f, 0.227f, 0.169f, 0.0f);
	// materials[0].ambient = glm::vec4(0.31f, 0.227f, 0.169f, 0.0f);
	// materials[0].specular = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // dirt cannot have this
	// materials[0].emissive = glm::vec4(0.31f, 0.227f, 0.169f, 0.0f);
	materials[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	materials[0].ambient = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	materials[0].specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	materials[0].emissive = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	materials[0].shininess = glm::vec4(32);
	materials[0].texture_id = glm::vec4(1);
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, UBO_materials));
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, 8 * sizeof(Material), materials));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));  // unbind

	// load test light
	// GLCall(glUniform1f(u_PointLight_constant, 1.0f));
	// GLCall(glUniform1f(u_PointLight_linear, 0.09f));
	// GLCall(glUniform1f(u_PointLight_quadratic, 0.032f));
	// GLCall(glUniform3f(u_PointLight_position, 0.0f, 5.0f, 2.0f));
	// GLCall(glUniform3f(u_PointLight_ambient, 0.2f, 0.2f, 0.2f));
	// GLCall(glUniform3f(u_PointLight_diffuse, 0.5f, 0.5f, 0.5f));
	// GLCall(glUniform3f(u_PointLight_specular, 1.0f, 1.0f, 1.0f));

	lightingShader.setFloat("u_PointLight.constant", 1.0f);
	lightingShader.setFloat("u_PointLight.linear", 0.09f);
	lightingShader.setFloat("u_PointLight.quadratic", 0.032f);
	lightingShader.setVec3("u_PointLight.position", 0.0f, 2.0f, 5.0f);
	lightingShader.setVec3("u_PointLight.ambient", 0.2f, 0.2f, 0.0f);
	lightingShader.setVec3("u_PointLight.diffuse", 0.78f, 0.78f, 0.0f);
	lightingShader.setVec3("u_PointLight.specular", 0.0f, 0.0f, 0.0f);

	// draw into the hdr framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	// GLCall(glPolygonMode(GL_FRONT_AND_BACK,GL_LINE));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, verts.size()));
		drawAxis(MVP);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// switch to hdr shader, and rerender
	HDRVertex hdrvertices[] = {
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
		{1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}
	};
	GLCall(glBindVertexArray(this->VAO_hdr));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_hdr));

	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(hdrvertices), hdrvertices, GL_STATIC_DRAW));


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	hdrShader.use();
	GLCall(glActiveTexture(GL_TEXTURE0 + HDR_TEXTURE_SLOT));
	GLCall(glBindTexture(GL_TEXTURE_2D, this->hdrTexture));
	hdrShader.setInt("u_HdrBuffer", HDR_TEXTURE_SLOT);
	hdrShader.setFloat("u_Gamma", gamma);
	hdrShader.setFloat("u_Exposure", exposure);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));


	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

// this does NOT take into acount currently used textures slots etc, only here for organisation
// make sure fbo is bound before calling this
void Renderer::generate_HDR_texture() {
	// delete existing texture, if needed
	if (this->hdrTexture != 0) {
		GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // for safety
		GLCall(glDeleteTextures(1, &hdrTexture));
	}

	GLCall(glGenTextures(1, &this->hdrTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, this->hdrTexture));
	// you can get the default behaviour by either not using the framebuffer or setting this to GL_RGBA
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->viewport_width, this->viewport_height, 0, GL_RGBA, GL_FLOAT, NULL));  // change to 32float if needed

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// attatch to fbo
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTexture, 0));
}

// regenerate the hdr texture
void Renderer::resizeViewport(GLsizei viewport_width, GLsizei viewport_height) {
	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
	generate_HDR_texture();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// needs to be improved
void Renderer::checkFrameBuffer(GLuint fbo) {
	GLCall(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

	if (status != GL_FRAMEBUFFER_COMPLETE) {
    	puts("Error in fbo");
		exit(1);
	}
}
