#include "BasicRenderer.h"

#define TEX_ARRAY_SLOT 0

BasicRenderer::BasicRenderer()
: basic("shaders/texture.vert", "shaders/texture.frag") {
	GLCall(glGenVertexArrays(1, &this->VAO));
	GLCall(glBindVertexArray(this->VAO));

	//////////////////////////// LOADING VBO ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (const void *)offsetof(BasicVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_color_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_color_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_color_layout, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (const void *)offsetof(BasicVertex, color)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 2;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (const void *)offsetof(BasicVertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex

		GLuint vertex_texid_layout = 3;
		GLCall(glEnableVertexAttribArray(vertex_texid_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texid_layout, 1, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (const void *)offsetof(BasicVertex, tex_id)));
		// GLCall(glVertexAttribDivisor(vertex_texid_layout, 0)); // values are per triangle, but I am not using instancing
	}

	basic.use();
	basic.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
	basic.setMat4("u_Model", glm::mat4(1.0f)); // load identity just for safety
	basic.setMat4("u_View", glm::mat4(1.0f)); // load identity just for safety
	basic.setMat4("u_Projection", glm::mat4(1.0f)); // load identity just for safety

	this->textureArray = std::make_unique<TextureArray>(TEXTURE_WIDTH, TEXTURE_HEIGHT, TEXTURE_LAYERS);
	TextureArray *tex = textureArray.get();
	tex->addTexture("textures/missing_texture.png"); // 0
	tex->addTexture("textures/black.png"); // 1
	tex->addTexture("textures/grass.png"); // 2
	tex->addTexture("textures/oak_log.png"); // 3
	tex->addTexture("textures/white.png"); // 4
	tex->addTexture("test_files_phase_4/earth.jpg"); // 5

	tex->setTextureArrayToSlot(TEX_ARRAY_SLOT);
}

BasicRenderer::~BasicRenderer() {
	GLCall(glDeleteBuffers(1, &vertexBuffer));

	GLCall(glDeleteVertexArrays(1, &VAO));
}

void BasicRenderer::draw(std::vector<Vertex> &verts, const glm::mat4 &projection, Camera &camera, GLFWwindow * window) {

	// translate all vertices to BasicVertex
	std::vector<BasicVertex> basic_verts(verts.size());
	for (Vertex& vert : verts) {
		basic_verts.emplace_back(
			vert.coords,
			glm::vec4(1.0f),
			vert.tex_coord,
			5.0f
		);
	}


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	// ImGui::ShowDemoWindow();
	ImGui::Text("Facing x:%f y:%f z:%f", camera.Front.x, camera.Front.y, camera.Front.z);
	ImGui::InputFloat3("Position:", glm::value_ptr(camera.Position));

	constexpr glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = camera.GetViewMatrix();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLCall(glBindVertexArray(this->VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));

	GLCall(glBufferData(GL_ARRAY_BUFFER, basic_verts.size() * sizeof(BasicVertex), basic_verts.data(), GL_STATIC_DRAW));
	basic.use();

	// load MVP, texture array and view
	this->textureArray.get()->setTextureArrayToSlot(TEX_ARRAY_SLOT);
	basic.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
	basic.setMat4("u_MVP", projection * view * model);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, basic_verts.size()));

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
