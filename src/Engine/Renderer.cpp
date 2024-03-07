#include "Renderer.h"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "Vertex.h"

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

void drawAxis() {
        glBegin(GL_LINES);
// X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f( 100.0f, 0.0f, 0.0f);
// Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
// Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
}

// por favor nao ler isto, era so para ter uma forma rapida de ler os shaders
const GLchar *readFromFile(char *filepath) {
	FILE *fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s\n", filepath);
		perror("Invalid file path");
		exit(1);
	}
	fseek(fp, 0L, SEEK_END);
	unsigned long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	GLchar *ret = new GLchar[size + 1];
	size_t charread = fread(ret, sizeof(GLchar), size, fp);
	if (charread != size) {
		fprintf(stderr, "Did not read all chars: %ld vs %ld\n", size, charread);
		exit(1);
	}
	ret[charread] = '\0';
	fclose(fp);
	return ret;
}

Renderer::Renderer()
: VAO(0), vertexBuffer(0), u_MVP(0)
{

	//////////////////////////// LOADING VAO ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO));
	GLCall(glBindVertexArray(this->VAO));

	//////////////////////////// LOADING VBO ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, coords)));
		GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

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
	}
	
	GLCall(glLinkProgram(program));
		checkProgramLinking(program);
		validateProgram(program);
	GLCall(glUseProgram(program));

	//////////////////////////// LOADING SHADER UNIFORMS ///////////////////////////
	GLCall(this->u_MVP = glGetUniformLocation(program, "u_MVP")); // missing error checking, could be -1
	GLCall(glUniformMatrix4fv(this->u_MVP, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)))); // load identity just for safety


	//////////////////////////// CLEANUP ///////////////////////////
	GLCall(glDeleteShader(VS));
	GLCall(glDeleteShader(FS));
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

Renderer::~Renderer() {
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
	const glm::mat4 MVP = projection * view * model;


	// bind VAO, VBO, program
	GLCall(glBindVertexArray(this->VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	GLCall(glUseProgram(this->program));

	// load vertices
	GLCall(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW));

	// load MVP to shader
	GLCall(glUniformMatrix4fv(this->u_MVP, 1, GL_FALSE, glm::value_ptr(MVP)));

	// draw
	GLCall(glDrawArrays(GL_TRIANGLES, 0, verts.size()));

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
