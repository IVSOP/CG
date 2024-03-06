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

void Renderer::draw(std::vector<Vertex> &verts, Camera &camera, GLFWwindow * window) const {
    // Clear buffers // isto funcionava em baixo do ImGui::Begin, buguei
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	// ImGui::ShowDemoWindow();
	ImGui::Text("Facing x:%f y:%f z:%f", camera.Front.x, camera.Front.y, camera.Front.z);
	ImGui::InputFloat3("Position:", glm::value_ptr(camera.Position));


	glMatrixMode(GL_MODELVIEW);
    // Load identity matrix
    glLoadIdentity();
    // Set camera position and angle
	glMultMatrixf(glm::value_ptr(camera.GetViewMatrix()));

    drawAxis();

    glPolygonMode(GL_FRONT, GL_LINE);

    glBegin(GL_TRIANGLES);

    for(unsigned int i = 0; i < verts.size(); i++){
        Vertex &v = verts[i];
        
		// glColor3f(0.01f * i, 1.0f - 0.01f * i, 0.0f);
		glVertex3fv(glm::value_ptr(v.getCoords()));
    }

    glEnd();


	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
