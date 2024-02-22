#include "Renderer.h"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "tinyxml2.h"
#include "Vertex.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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

void Renderer::draw(std::vector<Vertex> &verts, Camera &camera) const {
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
    // Load identity matrix
    glLoadIdentity();
    // Set camera position and angle
	glMultMatrixf(glm::value_ptr(camera.GetViewMatrix()));

    drawAxis();

    glPolygonMode(GL_FRONT, GL_LINE);

    glBegin(GL_TRIANGLES);

    for(int i = 0; i < verts.size(); i++){
        Vertex &v = verts[i];
        
		// glColor3f(0.01f * i, 1.0f - 0.01f * i, 0.0f);
		glVertex3fv(glm::value_ptr(v.getCoords()));
    }

    glEnd();

    /* Test -> swap for loaded file instructions */
    /*
    glRotated(30 * (counter - COUNTER_START), 0, 0, 1);

    glutWireTeapot(counter);

    if (increment && counter >= COUNTER_STOP) increment = false;

    if (!increment && counter <= COUNTER_START) increment = true;

    counter = increment ? counter + 0.005 : counter - 0.005;
    */
    /* Test's end */

    // End of frame
    glutSwapBuffers();
}
