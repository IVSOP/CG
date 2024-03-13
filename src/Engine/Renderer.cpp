#include "Renderer.h"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "Vertex.h"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>

#include "Camera.h"

void Renderer::drawAxis(const glm::mat4 &MVP) const {

	const Vertex vertices[] = {
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


	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCall(glUseProgram(this->program));
	GLCall(glUniformMatrix4fv(this->u_MVP, 1, GL_FALSE, glm::value_ptr(MVP)));

	GLCall(glDrawArrays(GL_LINES, 0, 6)); // 6 pontos, 3 linhas
}

// cursed, era so para ter uma forma rapida de ler os shaders
const GLchar *readFromFile(char *filepath) {
    std::ifstream inFile(filepath, std::ios::binary);

    if (inFile.is_open()) {
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);


        GLchar* ret = new GLchar[fileSize + 1];

        inFile.read(reinterpret_cast<GLchar*>(ret), fileSize);

        inFile.close();

        ret[fileSize] = '\0';

        /*if (charread. != fileSize) {
            fprintf(stderr, "Did not read all chars: %ld vs %ld\n", size, charread);
            exit(1);
        }*/

        return ret;
    }

    std::cerr << "Invalid file path " << filepath << std::endl;

    exit(1);
}

Renderer::Renderer()
: VAO(0), vertexBuffer(0), program(0), u_MVP(-1)
{

	//////////////////////////// LOADING VAO ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO));
	GLCall(glBindVertexArray(this->VAO));

	//////////////////////////// LOADING VBOS ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, coords)));
		GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_color_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_color_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_color_layout, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color)));
		GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex

	//////////////////////////// LOADING SHADERS ////////////////////////////	

	GLCall(this->program = glCreateProgram());

	GLuint VS, FS;
	{
		char filename[] = "shaders/basic.vert";
		const GLchar *vertex_shader = readFromFile(filename);
		GLCall(VS = glCreateShader(GL_VERTEX_SHADER));
		GLCall(glShaderSource(VS, 1, &vertex_shader, NULL));
		GLCall(glCompileShader(VS));
		GLCall(checkErrorInShader(VS));
		GLCall(glAttachShader(program, VS));
		delete[] vertex_shader;
	}

	{
		char filename[] = "shaders/basic.frag";
		const GLchar *fragment_shader = readFromFile(filename);
		GLCall(FS = glCreateShader(GL_FRAGMENT_SHADER));
		GLCall(glShaderSource(FS, 1, &fragment_shader, NULL));
		GLCall(glCompileShader(FS));
		GLCall(checkErrorInShader(FS));
		GLCall(glAttachShader(program, FS));
		delete[] fragment_shader;
	}
	
	GLCall(glLinkProgram(program));
		checkProgramLinking(program);
		validateProgram(program);


	GLCall(glDeleteShader(VS));
	GLCall(glDeleteShader(FS));


	//////////////////////////// LOADING SHADER UNIFORMS ///////////////////////////
	GLCall(glUseProgram(program));
	GLCall(this->u_MVP = glGetUniformLocation(program, "u_MVP")); // missing error checking, could be -1
	GLCall(glUniformMatrix4fv(this->u_MVP, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)))); // load identity just for safety

	//////////////////////////// CLEANUP ///////////////////////////
	// GLCall(glDeleteShader(VS));
	// GLCall(glDeleteShader(FS));
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

Renderer::~Renderer() {
	GLCall(glDeleteProgram(program));
	GLCall(glDeleteBuffers(1, &vertexBuffer));
	GLCall(glDeleteVertexArrays(1, &VAO));
}

void Renderer::draw(std::vector<Vertex> &verts, const glm::mat4 &projection, Camera &camera, GLFWwindow * window) const {
    // Clear buffers // isto funcionava em baixo do ImGui::Begin, buguei
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	// ImGui::ShowDemoWindow();
	ImGui::Text("Facing x:%f y:%f z:%f", camera.Front.x, camera.Front.y, camera.Front.z);
	ImGui::InputFloat3("Position:", glm::value_ptr(camera.Position));

	constexpr glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = camera.GetViewMatrix();
	const glm::mat4 MVP = projection * view * model; // nao so nao sei, como nao quero saber. vou assumir que a maneira que o glm faz as coisas influencia isto


	// bind VAO, VBO
	GLCall(glBindVertexArray(this->VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));

	// load vertices
	GLCall(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW));

	// bind program, load MVP
	GLCall(glUseProgram(this->program));
	GLCall(glUniformMatrix4fv(this->u_MVP, 1, GL_FALSE, glm::value_ptr(MVP)));

	// draw
	GLCall(glPolygonMode(GL_FRONT_AND_BACK,GL_LINE));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, verts.size()));

	drawAxis(MVP);

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
