#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "tinyxml2.h"
#include "Vertex.h"
#include "XmlParser.h"

#if _WIN32
#include <windows_unistd.h>
#else
#include <unistd.h>
#endif

#include "Renderer.h"
#include "Camera.h"
#include "InputHandler.h"
#include <mutex>
#include <thread>

/* Test variables */
#define COUNTER_START 0.65
#define COUNTER_STOP 1.15

double counter = COUNTER_START;
bool increment = true;
/* End of test variables */


///////////////////////////////////////////////////////// main objects
Renderer renderer;
Camera camera;
InputHandler inputHandler;
XmlParser xmlParser;
bool resize = false;

std::vector<Vertex> points;
std::vector<Vertex> draw_points;

// sempre que dou resize, ele manda um mouse callback que lixa tudo, tive de fazer isto para esse callback ser ignorado
// isto e extremamente roto, mas nas docs basicamente diz que sou burro se tentar fazer como no glut e meter GLFW_CURSOR_HIDDEN e estar sempre a centra-lo, diz para usar GLFW_CURSOR_DISABLED
// ou seja acho que vai ter de ficar assim
std::mutex mtx;
int refreshRate = 60;

void setWindow(GLFWwindow* window, int width, int height) {
    xmlParser.setWindowWidth(width);
    xmlParser.setWindowHeight(height);

    int windowWidth = xmlParser.getWindowWidth();
    int windowHeight = xmlParser.getWindowHeight();

    GLdouble windowFov = xmlParser.getWindowFov();
    GLdouble windowZNear = xmlParser.getWindowZNear();
    GLdouble windowZFar = xmlParser.getWindowZFar();


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
	resize = true;

	refreshRate = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
}

void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
    inputHandler.pressKey(window, key, scancode, action, mods);
}

void handleMouseMov(GLFWwindow *window, double xpos, double ypos) {
    // printf("mouse callback is at %f %f\n", static_cast<GLfloat>(xpos), static_cast<GLfloat>(ypos));
    if (!resize) {
        inputHandler.moveMouseTo(xpos, ypos);
    } else {
        // fix para ao dar resize da janela coordenadas mudarem
        GLdouble xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        resize = false;
        inputHandler.centerMouseTo(xpos, ypos);
    }
}

void loop_step(GLFWwindow *window) {
    double lastFrameTime = glfwGetTime(), currentFrameTime, deltaTime;

	// do CPU things.........



	currentFrameTime = glfwGetTime();
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;


	// draw if delta allows it. sleep until target
	if (deltaTime < PHYS_STEP) {
		const double sleepTime = (PHYS_STEP - deltaTime) * 10E5; // multiply to get from seconds to microseconds, this is prob platform dependent and very bad
		usleep(sleepTime);
		// is it better to just sleep or should I already start another tick here?
	}
	renderer.draw(points, camera, window);
}

void renderLoop(GLFWwindow *window) {
    int windowWidth = xmlParser.getWindowWidth();
    int windowHeight = xmlParser.getWindowHeight();

	// era possivel o deltatime ser dinamico, mas tive preguica, meti ao refresh rate do monitor
	// no need for time or sleep, vsync takes care of it
	inputHandler.applyToCamera(window, camera, windowWidth, windowHeight, 1.0f / static_cast<GLfloat>(refreshRate));


	std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
	auto s = draw_points.size();
	// printf("%f %f %f %lu\n", draw_points[s -1].getX(), draw_points[s -1].getY(), draw_points[s -1].getZ(), s);
	renderer.draw(draw_points, camera, window);
	lock.unlock();

	glfwPollEvents();
}

auto physLoop = [](GLFWwindow *window) {
	while (!glfwWindowShouldClose(window)) {
		double lastFrameTime = glfwGetTime(), currentFrameTime, deltaTime;
		// perform calculations............................

		std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
		draw_points = points; // copy the buffer
		lock.unlock();

		currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// draw if delta allows it. sleep until target
		if (deltaTime < PHYS_STEP) {
			const double sleepTime = (PHYS_STEP - deltaTime) * 10E5; // multiply to get from seconds to microseconds, this is prob platform dependent and very bad
			usleep(sleepTime);
		}
	}
};

int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Not enough arguments");

        return 1;
    }

    xmlParser.parseXML(argv[1]);

    int windowWidth = xmlParser.getWindowWidth();
    int windowHeight = xmlParser.getWindowHeight();

    if (!glfwInit()) {
        perror("GLFW window failed to initiate");
    }
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "CG", NULL, NULL);
    if (window == NULL) {
        perror("GLFW window failed to create");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    ///////////////////////// CALLBAKCS
    glfwSetFramebufferSizeCallback(window, setWindow);
    glfwSetKeyCallback(window, handleKey);
    // TEMPORARY
    inputHandler.handleMouseMov = handleMouseMov;
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

    GLdouble cameraXPos = xmlParser.getCameraXPos();
    GLdouble cameraYPos = xmlParser.getCameraYPos();
    GLdouble cameraZPos = xmlParser.getCameraZPos();
    GLdouble cameraXLook = xmlParser.getCameraXLook();
    GLdouble cameraYLook = xmlParser.getCameraYLook();
    GLdouble cameraZLook = xmlParser.getCameraZLook();
    GLdouble cameraXUp = xmlParser.getCameraXUp();
    GLdouble cameraYUp = xmlParser.getCameraYUp();
    GLdouble cameraZUp = xmlParser.getCameraZUp();

    camera = Camera(glm::vec3(cameraXPos, cameraYPos, cameraZPos), glm::vec3(cameraXLook, cameraYLook, cameraZLook), glm::vec3(cameraXUp, cameraYUp, cameraZUp));

    setWindow(window, static_cast<GLdouble>(windowWidth), static_cast<GLdouble>(windowHeight));

    points = xmlParser.getPoints();
	draw_points = points; // early copy to allow renderer to display something

	std::thread thread_object(physLoop, window);

	while (!glfwWindowShouldClose(window)) {
		renderLoop(window);
	}

    return 0;
}
