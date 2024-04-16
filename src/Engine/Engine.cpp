#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "Engine.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//////////////////////////// CALLBACKS FOR GLFW //////////////////////////////
void setWindow(GLFWwindow* window, int windowWidth, int windowHeight) {
	Engine *engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));

    if (windowWidth == 0 || windowHeight == 0) {
        fprintf(stderr, "Detected window size 0, ignoring resize operation\n");
        return;
    }

    // Compute window's ration
    GLfloat aspectRatio = static_cast<GLfloat>(windowWidth) / static_cast<GLfloat>(windowHeight);

    // Set perspective
    engine->projection = glm::perspective(glm::radians(static_cast<GLfloat>(engine->windowFov)), static_cast<GLfloat>(aspectRatio), static_cast<GLfloat>(engine->windowZNear), static_cast<GLfloat>(engine->windowZFar));

    // Set viewport to be the entire window
    glViewport(0, 0, windowWidth, windowHeight);

	engine->windowWidth = windowWidth;
	engine->windowHeight = windowHeight;

	engine->renderer.get()->resizeViewport(windowWidth, windowHeight);

    // printf("window set to %d %d. half is %d %d\n", windowWidth, windowHeight, windowWidth / 2, windowHeight / 2);
    engine->resize = true;
}

void handleKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	Engine *engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));

    engine->inputHandler.get()->pressKey(window, key, scancode, action, mods);
}

void handleMouseMov(GLFWwindow *window, double xpos, double ypos) {
	Engine *engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));

    // printf("mouse callback is at %f %f\n", static_cast<GLfloat>(xpos), static_cast<GLfloat>(ypos));
    if (!engine->resize) {
        engine->inputHandler.get()->moveMouseTo(xpos, ypos);
    } else {
        // fix para ao dar resize da janela coordenadas mudarem
        glfwGetCursorPos(window, &xpos, &ypos);
        engine->resize = false;
        engine->inputHandler.get()->centerMouseTo(xpos, ypos);
    }
}

void Engine::renderLoop() {
    double lastFrameTime, currentFrameTime, deltaTime = PHYS_STEP; // to prevent errors when this is first ran, I initialize it to the physics substep
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // at the start due to imgui (??) test moving it to after the unlock()

		lastFrameTime = glfwGetTime();

        // printf("delta is %f (%f fps)\n", deltaTime, 1.0f / deltaTime);
        inputHandler.get()->applyToCamera(*camera.get(), windowWidth, windowHeight, static_cast<GLfloat>(deltaTime));


        std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
        renderer.get()->draw(draw_points, draw_objectInfo, projection, *camera.get(), window, deltaTime);
        lock.unlock();

        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;

        // no need for sleep, vsync takes care of mantaining timings
		// HOWEVER macbooks as per usual do not work properly
		// so I made this bandaid fix
		if (renderer.get()->limitFPS) {
			const double fps_time = 1.0f / renderer.get()->fps;
			if (deltaTime < fps_time) {
				const double sleepTime = (fps_time - deltaTime) * 10E5; // multiply to get from seconds to microseconds, this is prob platform dependent and very bad
				usleep(sleepTime);
				deltaTime = fps_time;
			}
		}
    }
	kill = true;
}

void Engine::physLoop () {
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

    int i = 0;
	
    while (!kill) {
        lastFrameTime = glfwGetTime();
        // perform calculations............................

        std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
        draw_points = points; // copy the buffer
        // draw_objectInfo = objectInfo;
        draw_objectInfo = this->renderer.get()->translateEngineObjectInfo(this->xmlParser.getObjectInfo(static_cast<float>(i) * PHYS_STEP));

        lock.unlock();

        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // draw if delta allows it. sleep until target
        if (deltaTime < PHYS_STEP) {
            const double sleepTime = (PHYS_STEP - deltaTime) * 10E5; // multiply to get from seconds to microseconds, this is prob platform dependent and very bad
            usleep(sleepTime);
        }

        i++;
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

    XmlParser xmlParser;
	xmlParser.parseXML(argv[1]);

	Engine engine(xmlParser);
	engine.loop();

    return 0;
}

Engine::Engine(XmlParser &xmlParser) :xmlParser(xmlParser) {
	this->windowWidth = xmlParser.getWindowWidth();
    this->windowHeight = xmlParser.getWindowHeight();

    glfwSetErrorCallback(glfw_error_callback); // ?? isto nao devia tar depois??
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only isto faz o que??????????????????????????????????????????????????????????????????????????????????????????????????????
#endif

    this->window = glfwCreateWindow(windowWidth, windowHeight, "CG", NULL, NULL);
    if (window == NULL) {
        perror("GLFW window failed to create");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		perror("GLEW failed");
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(window, this);

	this->inputHandler = std::make_unique<InputHandler>();

    glfwSwapInterval(1); // hardcoded sync with monitor fps

    ///////////////////////// CALLBAKCS
    glfwSetFramebufferSizeCallback(window, setWindow);
    glfwSetKeyCallback(window, handleKey);
    // TEMPORARY
    inputHandler.get()->handleMouseMov = handleMouseMov;
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

    this->camera = std::make_unique<Camera>(Camera(glm::vec3(cameraXPos, cameraYPos, cameraZPos), glm::vec3(cameraXLook, cameraYLook, cameraZLook), glm::vec3(cameraXUp, cameraYUp, cameraZUp)));


	this->windowFov = xmlParser.getWindowFov();
    this->windowZNear = xmlParser.getWindowZNear();
    this->windowZFar = xmlParser.getWindowZFar();


	// por qualquer razao especificamente no mac nao da para criar shaders sem ter um VAO
#ifdef __APPLE__
	GLuint tempVAO;
	GLCall(glGenVertexArrays(1, &tempVAO));
	GLCall(glBindVertexArray(tempVAO));
#endif

	this->renderer = std::make_unique<Renderer>(static_cast<GLsizei>(this->windowWidth), static_cast<GLsizei>(this->windowHeight));

#ifdef __APPLE__
	GLCall(glDeleteVertexArrays(1, &tempVAO));
#endif

    setWindow(window, static_cast<GLdouble>(this->windowWidth), static_cast<GLdouble>(this->windowHeight));

	// vector of vectors, where [i] means the vertex has index [i]
    std::vector<std::vector<Vertex>> unparsed_points = xmlParser.getPoints();
	for (GLuint i = 0; i < unparsed_points.size(); i++) {
		for (const Vertex &vert : unparsed_points[i]) {
			Vertex newVert = vert;
			newVert.object_id = i;
			this->points.push_back(newVert);
		}
	}

    // TODO passar tempo atual

	// after getting the engine object info, translate them to renderer object info (yes very bad but only done once)
	std::vector<Engine_Object_Info> engineObjInfo = xmlParser.getObjectInfo(0.0f);
	this->objectInfo = this->renderer.get()->translateEngineObjectInfo(engineObjInfo);

	// early copy to allow renderer to display something
    this->draw_points = points;
	this->draw_objectInfo = objectInfo;
}

void Engine::loop() {
    std::thread physThread(&Engine::physLoop, this);
	physThread.detach();

	renderLoop();
}

Engine::~Engine() {
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
