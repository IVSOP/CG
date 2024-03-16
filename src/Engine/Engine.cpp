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
#include <thread>
#include <mutex>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

///////////////////////////////////////////////////////// main objects. temporario, depois vai sair daqui
InputHandler inputHandler;
XmlParser xmlParser;
bool resize = false;
glm::mat4 projection = glm::mat4(1.0f);

std::vector<Vertex> points;
std::vector<Vertex> draw_points;

// sempre que dou resize, ele manda um mouse callback que lixa tudo, tive de fazer isto para esse callback ser ignorado
// isto e extremamente roto, mas nas docs basicamente diz que sou burro se tentar fazer como no glut e meter GLFW_CURSOR_HIDDEN e estar sempre a centra-lo, diz para usar GLFW_CURSOR_DISABLED
// ou seja acho que vai ter de ficar assim
std::mutex mtx;

void setWindow(GLFWwindow* window, int windowWidth, int windowHeight) {
    // cursed, devia sair daqui, os valores nunca se alteram desde que o xml e carregado
    const GLdouble windowFov = xmlParser.getWindowFov();
    const GLdouble windowZNear = xmlParser.getWindowZNear();
    const GLdouble windowZFar = xmlParser.getWindowZFar();

    if (windowWidth == 0 || windowHeight == 0) {
        fprintf(stderr, "Detected window size 0, ignoring resize operation\n");
        return;
    }

    // Compute window's ration
    GLfloat aspectRatio = static_cast<GLfloat>(windowWidth) / static_cast<GLfloat>(windowHeight);

    // Set perspective
    projection = glm::perspective(glm::radians(static_cast<GLfloat>(windowFov)), static_cast<GLfloat>(aspectRatio), static_cast<GLfloat>(windowZNear), static_cast<GLfloat>(windowZFar));

    // Set viewport to be the entire window
    glViewport(0, 0, windowWidth, windowHeight);

    // printf("window set to %d %d. half is %d %d\n", windowWidth, windowHeight, windowWidth / 2, windowHeight / 2);
    resize = true;
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
        glfwGetCursorPos(window, &xpos, &ypos);
        resize = false;
        inputHandler.centerMouseTo(xpos, ypos);
    }
}

void renderLoop(GLFWwindow *window, Camera &camera, Renderer &renderer) {
    double lastFrameTime, currentFrameTime, deltaTime = PHYS_STEP; // to prevent errors when this is first ran, I initialize it to the physics substep
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // at the start due to imgui (??) test moving it to after the unlock()

        lastFrameTime = glfwGetTime();
        int windowWidth = xmlParser.getWindowWidth();
        int windowHeight = xmlParser.getWindowHeight();

        // printf("delta is %f (%f fps)\n", deltaTime, 1.0f / deltaTime);
        inputHandler.applyToCamera(camera, windowWidth, windowHeight, static_cast<GLfloat>(deltaTime));


        std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
        // auto s = draw_points.size();
        // printf("%f %f %f %lu\n", draw_points[s -1].getX(), draw_points[s -1].getY(), draw_points[s -1].getZ(), s);
        renderer.draw(draw_points, projection, camera, window);
        lock.unlock();

        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // no need for sleep, vsync takes care of mantaining timings
    }
}

auto physLoop = [](GLFWwindow *window) {
    double lastFrameTime, currentFrameTime, deltaTime;
	// for (unsigned int i = 0; i < points.size(); i++) {
	// 	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	// 	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	// 	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	// 	points[i].color.r = r;
	// 	points[i].color.g = g;
	// 	points[i].color.b = b;
		// points[i].tex_id = 1.0f;
	// }
	
    while (!glfwWindowShouldClose(window)) {
        lastFrameTime = glfwGetTime();
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

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Not enough arguments");

        return 1;
    }

    xmlParser.parseXML(argv[1]);

    int windowWidth = xmlParser.getWindowWidth();
    int windowHeight = xmlParser.getWindowHeight();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        perror("GLFW window failed to initiate");
    }

#if defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 410";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 410";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only isto faz o que??????????????????????????????????????????????????????????????????????????????????????????????????????
#endif

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "CG", NULL, NULL);
    if (window == NULL) {
        perror("GLFW window failed to create");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		perror("GLEW failed");
		return 1;
	}

    glfwSwapInterval(1); // hardcoded sync with monitor fps

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

	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	// During init, enable debug output (not for macs tho skill issue)

#if not (defined(__APPLE__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__))
	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( openglCallbackFunction, NULL );
#endif

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // std::cout << glGetString(GL_VERSION) << std::endl;

    // During init, enable debug output
    // glEnable( GL_DEBUG_OUTPUT );
    // glDebugMessageCallback( openglCallbackFunction, NULL );

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

    Camera camera = Camera(glm::vec3(cameraXPos, cameraYPos, cameraZPos), glm::vec3(cameraXLook, cameraYLook, cameraZLook), glm::vec3(cameraXUp, cameraYUp, cameraZUp));
    Renderer renderer;

    setWindow(window, static_cast<GLdouble>(windowWidth), static_cast<GLdouble>(windowHeight));

    points = xmlParser.getPoints();

	// temporary to test engine while generator does not have tex coords
	points = std::vector<Vertex>();
	points.push_back(Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0));
	points.push_back(Vertex(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0));
	points.push_back(Vertex(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0));
	points.push_back(Vertex(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0));
	points.push_back(Vertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0));
	points.push_back(Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0));
	
	points.push_back(Vertex(-1.0f, -1.0f + 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0));
	points.push_back(Vertex(1.0f, -1.0f + 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0));
	points.push_back(Vertex(1.0f, 1.0f + 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0));
	points.push_back(Vertex(1.0f, 1.0f + 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0));
	points.push_back(Vertex(-1.0f, 1.0f + 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0));
	points.push_back(Vertex(-1.0f, -1.0f + 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0));

	points.push_back(Vertex(-1.0f, -1.0f + 4.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0));
	points.push_back(Vertex(1.0f, -1.0f + 4.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0));
	points.push_back(Vertex(1.0f, 1.0f + 4.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0));
	points.push_back(Vertex(1.0f, 1.0f + 4.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0));
	points.push_back(Vertex(-1.0f, 1.0f + 4.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0));
	points.push_back(Vertex(-1.0f, -1.0f + 4.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0));

	


    draw_points = points; // early copy to allow renderer to display something

    std::thread thread_object(physLoop, window);

    renderLoop(window, camera, renderer);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
