#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "tinyxml2.h"
#include "Vertex.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
// #include <GLUT/freeglut_ext.h>
#else
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#endif

#include "Renderer.h"
#include "Camera.h"
#include "InputHandler.h"

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


///////////////////////////////////////////////////////// main objects
Renderer renderer;
Camera camera;
InputHandler inputHandler;
GLFWwindow *window;
void handleMouseMov(GLFWwindow *window, double xpos, double ypos);

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

void setWindow(GLFWwindow* window, int width, int height) {
	windowWidth = width;
	windowHeight = height;


	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setting up MSAA anti aliasing, needs to happen before window creation??
	// glfwWindowHint(GLFW_SAMPLES, 4);

	// Compute window's ration
    GLfloat aspectRatio = static_cast<GLfloat>(windowWidth) / static_cast<GLfloat>(windowHeight);

    // Set projection matrix
    glMatrixMode(GL_PROJECTION);

    // Load identity matrix
    glLoadIdentity();

    // Set perspective
	const glm::mat4 perspective = glm::perspective(glm::radians(static_cast<GLfloat>(windowFov)), static_cast<GLfloat>(aspectRatio), static_cast<GLfloat>(windowZNear), static_cast<GLfloat>(windowZFar));
    // gluPerspective(windowFov, aspectRatio, windowZNear, windowZFar);
	glMultMatrixf(glm::value_ptr(perspective));

    // Return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);

    // Set viewport to be the entire window
    glViewport(0, 0, windowWidth, windowHeight);

	// printf("window set to %d %d. half is %d %d\n", windowWidth, windowHeight, windowWidth / 2, windowHeight / 2);
	inputHandler.centerMouseTo(static_cast<GLdouble>(windowWidth / 2), static_cast<GLdouble>(windowHeight / 2));
}

void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	inputHandler.pressKey(key, scancode, action, mods);
}

void handleMouseMov(GLFWwindow *window, double xpos, double ypos) {
	// printf("mouse callback is at %f %f\n", static_cast<GLfloat>(xpos), static_cast<GLfloat>(ypos));
	inputHandler.moveMouseTo(xpos, ypos);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Not enough arguments");

        return 1;
    }

    parseXML(argv[1]);

		if (!glfwInit()) {
		perror("GLFW window failed to initiate");
	}
	window = glfwCreateWindow(windowWidth, windowHeight, "CG", NULL, NULL);
	if (window == NULL) {
		perror("GLFW window failed to create");
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);

	///////////////////////// CALLBAKCS
	glfwSetFramebufferSizeCallback(window, setWindow);
	glfwSetKeyCallback(window, handleKey);
	glfwSetCursorPosCallback(window, handleMouseMov);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    /* OpenGL settings */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// std::cout << glGetString(GL_VERSION) << std::endl;

	// During init, enable debug output
	///////////////////////////////////////// isto deve ser do glut, nao vou usar por agora
	// glEnable( GL_DEBUG_OUTPUT );
	// glDebugMessageCallback( openglCallbackFunction, NULL );

	glfwSwapInterval(1); // hardcoded sync with monitor fps

	// IMGUI_CHECKVERSION();
	// ImGui::CreateContext();
	// // ImGuiIO& io = ImGui::GetIO(); (void)io; // ????
	// ImGui_ImplGlfw_InitForOpenGL(window, true);
	// ImGui_ImplOpenGL3_Init("#version 450");
	// ImGui::StyleColorsDark();
	// // io.ConfigFlags |= .....


	// MSAA
	// glEnable(GL_MULTISAMPLE); 

	camera = Camera(glm::vec3(cameraXPos, cameraYPos, cameraZPos), glm::vec3(cameraXLook, cameraYLook, cameraZLook), glm::vec3(cameraXUp, cameraYUp, cameraZUp));

	setWindow(window, static_cast<GLdouble>(windowWidth), static_cast<GLdouble>(windowHeight));

	while (!glfwWindowShouldClose(window)) {
		inputHandler.applyToCamera(window, camera, windowWidth, windowHeight);
		renderer.draw(points, camera, window);
		glfwPollEvents();
	}

    return 0;
}
