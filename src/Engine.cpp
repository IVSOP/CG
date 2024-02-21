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

/* Test variables */
#define COUNTER_START 0.65
#define COUNTER_STOP 1.15

double counter = COUNTER_START;
bool increment = true;
/* End of test variables */

GLdouble windowFov = 45.0f;
int windowWidth = 512;
int windowHeight = 512;
GLdouble windowZNear = 1.0f;
GLdouble windowZFar = 1000.0f;
GLdouble cameraXPos = 0.0f;
GLdouble cameraYPos = 0.0f;
GLdouble cameraZPos = 5.0f;
GLdouble cameraXLook = 0.0f;
GLdouble cameraYLook = 0.0f;
GLdouble cameraZLook = -1.0f;
GLdouble cameraXUp = 0.0f;
GLdouble cameraYUp = 0.0f;
GLdouble cameraZUp = -1.0f;
std::vector<Vertex> points = std::vector<Vertex>();

//mover câmera
float angleY = 0.0f;

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

int parseXML(char * xmlFile) {
    tinyxml2::XMLDocument xmlDoc;

    /*
     * There has been an error loading the given file
     * Either file doesn't exist (wrong path) or there has been another error
     */
    if (xmlDoc.LoadFile(xmlFile) != tinyxml2::XML_SUCCESS) {
        perror("There has been an error reading the given file.");

        return 1;
    }

    tinyxml2::XMLElement *root = xmlDoc.RootElement();

    /* The given file doesn't have any root element */
    if (root == nullptr) {
        perror("The given XML file has no root element to read.");

        return 1;
    }

    /* Acquire all needed elements from the XML file */
    tinyxml2::XMLElement *window = root->FirstChildElement("window");
    tinyxml2::XMLElement *camera = root->FirstChildElement("camera");
    tinyxml2::XMLElement *group = root->FirstChildElement("group");

    /* Acquire all needed sub-elements from the camera element */
    tinyxml2::XMLElement *position = camera->FirstChildElement("position");
    tinyxml2::XMLElement *lookAt = camera->FirstChildElement("lookAt");
    tinyxml2::XMLElement *up = camera->FirstChildElement("up");
    tinyxml2::XMLElement *projection = camera->FirstChildElement("projection");

    tinyxml2::XMLElement *models = group->FirstChildElement("models");

    /* Acquire all needed values to draw the window */
    windowWidth = window->IntAttribute("width");
    windowHeight = window->IntAttribute("height");
    windowFov = projection->DoubleAttribute("fov");
    windowZNear = projection->DoubleAttribute("near");
    windowZFar = projection->DoubleAttribute("far");
    cameraXPos = position->DoubleAttribute("x");
    cameraYPos = position->DoubleAttribute("y");
    cameraZPos = position->DoubleAttribute("z");
    cameraXLook = lookAt->DoubleAttribute("x");
    cameraYLook = lookAt->DoubleAttribute("y");
    cameraZLook = lookAt->DoubleAttribute("z");
    cameraXUp = up->DoubleAttribute("x");
    cameraYUp = up->DoubleAttribute("y");
    cameraZUp = up->DoubleAttribute("z");

    // TODO Process group element

    for (tinyxml2::XMLElement* model = models->FirstChildElement("model"); model != nullptr; model = model->NextSiblingElement("model")){


        std::cout << model->Attribute("file") << std::endl;
        FILE* f = std::fopen(model->Attribute("file"), "rb");
        int size;

        std::fread(&size, sizeof(int), 1, f);

        Vertex* pts = new Vertex[size];
        fread(pts, sizeof(Vertex), size, f);

        points = std::vector<Vertex>(pts, pts + size);
        delete[] pts;
    }

    return 0;
}

void setWindow(int width, int height) {
    width = std::max(1, width);
    height = std::max(1, height);

    // Compute window's ration
    float aspectRatio = (float) (width) * 1.0f / (float) height;

    // Set projection matrix
    glMatrixMode(GL_PROJECTION);

    // Load identity matrix
    glLoadIdentity();

    // Set perspective
    gluPerspective(windowFov, aspectRatio, windowZNear, windowZFar);

    // Return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);

    // Set viewport to be the entire window
    glViewport(0, 0, width, height);
}

void renderScene() {
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load identity matrix
    glLoadIdentity();

    // Set camera position an angle
    gluLookAt(cameraXPos, cameraYPos, cameraZPos, // Camera position
              cameraXLook, cameraYLook, cameraZLook, // Place to where the camera is looking at
              cameraXUp, cameraYUp, cameraZUp);

    drawAxis();

    // Drawing instructions
    // TODO Check with teacher
    glRotatef(angleY, 0, 1, 0);

    glPolygonMode(GL_FRONT, GL_LINE);

    glBegin(GL_TRIANGLES);

    for(int i = 0; i < points.size(); i++){
        Vertex v = points[i];

        glVertex3f(v.getX(), v.getY(), v.getZ());
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

//DEBUG
void rotateCenter(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT: angleY -= 10.0f; break;
        case GLUT_KEY_RIGHT: angleY += 10.0f; break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    std::cout << std::filesystem::current_path() << std::endl;

    if (argc != 2) {
        perror("Not enough arguments");

        return 1;
    }

    parseXML(argv[1]);

    /* GLUT init */
    glutInit(&argc, argv); // Initialize glut

    /* Create glut window */
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    /*
     * The values passed here are only an indication, they may not be used as passed
     * TODO which values should we use
     */
    glutInitWindowPosition(100, 100);

    /*
     * The values passed here are only an indication, they may not be used as passed
     * Currently passing the values read on the XML file
     */
    glutInitWindowSize(windowWidth, windowHeight);

    glutCreateWindow("Test");

    /* Callback registry */

    /* Function responsible rendering the scene when the window is created */
    glutDisplayFunc(renderScene);

    /* Function responsible for continuously rendering the scene */
    // glutIdleFunc(renderScene);

    /* Function responsible for resizing the window */
    glutReshapeFunc(setWindow);

    //DEBUG
    glutSpecialFunc(rotateCenter);

    /* OpenGL settings */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* GLUTS main cycle */
    glutMainLoop();

    return 0;
}
