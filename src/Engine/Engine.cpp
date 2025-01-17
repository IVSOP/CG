#include <iostream>
#include <vector>
#include <cstdio>
#include <filesystem>

#include "Engine.h"

#include <chrono>
#include <thread>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")

#endif

#ifndef linux
	#define NO_PHYS_THREAD
#endif

#ifdef NO_PHYS_THREAD
	#pragma message("Not using physics thread")
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
    double frameStartTime, frameEndTime, deltaTime = PHYS_STEP, physDeltaTime = PHYS_STEP; // starts as PHYS_STEP to prevent errors
	double physTotalTime = 0.0;
	double physDeltaTime_renderer_copy, physProcessingDeltaTime_renderer_copy; // for case of using physics thread
	// deltaTime is actual time taken for the frame, so I can sleep, move the camera etc
	// phys time can be changed to change the physisc without touching the camera

	SceneLights lights = xmlParser.getLights();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // at the start due to imgui (??) test moving it to after the unlock()

		frameStartTime = glfwGetTime();

        // printf("delta is %f (%f fps)\n", deltaTime, 1.0f / deltaTime);
        inputHandler.get()->applyToCamera(*camera.get(), windowWidth, windowHeight, static_cast<GLfloat>(deltaTime));


#ifdef NO_PHYS_THREAD
		// draw_points = points; 
		draw_objectInfo = this->renderer.get()->translateEngineObjectInfo(this->xmlParser.getObjectInfo(physTotalTime));
        draw_curvePoints = xmlParser.getCurvePoints(physTotalTime, 100); // Tesselation level;

		renderer.get()->draw(draw_curvePoints, draw_points, draw_objectInfo,
			// xmlParser.sunVertices, xmlParser.sunInfo, physTotalTime,
			projection, *camera.get(), window,
			deltaTime, physDeltaTime, deltaTime,
			lights); // deltas will be wrong for this case
#else
        std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
			draw_points = points;
			draw_objectInfo = objectInfo;
			draw_curvePoints = curvePoints;
			physDeltaTime_renderer_copy = physDeltaTime_renderer;
			physProcessingDeltaTime_renderer_copy = physProcessingDeltaTime_renderer;
        lock.unlock();
		// frame end time here or after??????????????????????????
		renderer.get()->draw(draw_curvePoints, draw_points, draw_objectInfo,
			// xmlParser.sunVertices, xmlParser.sunInfo, physTotalTime, // time not entirely accurate but good enough for now
			projection, *camera.get(), window,
			deltaTime, physDeltaTime_renderer_copy, physProcessingDeltaTime_renderer_copy,
			lights);
#endif

        frameEndTime = glfwGetTime();
        deltaTime = frameEndTime - frameStartTime;
		physDeltaTime = deltaTime * static_cast<double>(*engine_speed);
		physTotalTime += physDeltaTime;

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
}

void Engine::physLoopNonDeterministic () {
	double frameStartTime, frameEndTime, deltaTime = 0.0f;
	double totalTime = 0.0;

	phys_points = points;

	while (!kill) {
		frameStartTime = glfwGetTime();
		// phys_points = ...
		phys_objectInfo = this->renderer.get()->translateEngineObjectInfo(this->xmlParser.getObjectInfo(totalTime));
		phys_curvePoints = xmlParser.getCurvePoints(totalTime, 100); // Tesselation level

		std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
			// copy all the buffers into draw buffers
			points = phys_points;
			curvePoints = phys_curvePoints;
			objectInfo = phys_objectInfo;
			physDeltaTime_renderer = deltaTime;
			physProcessingDeltaTime_renderer = deltaTime;
		lock.unlock();

		frameEndTime = glfwGetTime();
		deltaTime = frameEndTime - frameStartTime;

		totalTime += (frameEndTime - frameStartTime) * static_cast<double>(*engine_speed);
    }

	// signal to renderer that I have exited
	std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(kill_mtx);
	killCondition.notify_all();
}

void Engine::physLoopDeterministic () {
    double frameStartTime, frameEndTime, deltaTime = PHYS_STEP; // this deltatime is just to know for how long to sleep
	constexpr GLfloat physDeltaTime = PHYS_STEP; // this is the delta time used in calculations
	double sleepTarget;

	phys_points = points;

	GLuint i = 0;
    
    while (!kill) {
		if (*engine_speed > 0.0) { // no sense it computing things with 0 time. also would cause an infinite sleep. will cause high cpu usage, but it is simple. cursed solution would be condition that waits for engine speed to not be 0
			sleepTarget = static_cast<double>(physDeltaTime / (*engine_speed));
			frameStartTime = glfwGetTime();
			// if this runs faster than renderer, more work will be done than can be displayed
			// alternative would be to skip frames by doing i++, but could cause problems later if frames should not be skipped (due to a physics system, etc)

			// phys_points = ...
			phys_curvePoints = xmlParser.getCurvePoints(static_cast<float>(i) * physDeltaTime, 100); // Tesselation level
			phys_objectInfo = this->renderer.get()->translateEngineObjectInfo(this->xmlParser.getObjectInfo(static_cast<float>(i) * physDeltaTime));

			std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(mtx);
				// copy all the buffers into draw buffers
				points = phys_points;
				curvePoints = phys_curvePoints;
				objectInfo = phys_objectInfo;
				physDeltaTime_renderer = physDeltaTime;
				physProcessingDeltaTime_renderer = deltaTime;
			lock.unlock();

			frameEndTime = glfwGetTime();
			deltaTime = frameEndTime - frameStartTime;


			// the only thing that engine speed changes is how many steps are taken per second, the steps are still the same
			// for high engine speeds, might become innacurate
			if (deltaTime < sleepTarget) {
				// const int64_t sleepTime = ...
				// std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
				const double sleepTime = (sleepTarget - deltaTime) * 10E5; // multiply to get from seconds to microseconds
				usleep(sleepTime);
			}
			// double end = glfwGetTime();
			// printf("total time of phys frame was %lf (%lf fps)\n", end - frameStartTime, 1.0 / (end - frameStartTime));

			i++;

			// just so that debug info is correct
			frameEndTime = glfwGetTime();
			deltaTime = frameEndTime - frameStartTime;
		}
		physProcessingDeltaTime_renderer = INFINITY; // so debug shows as 0
    }

	// signal to renderer that I have exited
	std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(kill_mtx);
	killCondition.notify_all();
}

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
	engine_speed = &(renderer.get()->engine_speed);

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

	// after getting the engine object info, translate them to renderer object info (yes very bad but only done once)
	std::vector<Engine_Object_Info> engineObjInfo = xmlParser.getObjectInfo(0.0f);
	this->objectInfo = this->renderer.get()->translateEngineObjectInfo(engineObjInfo);
    this->curvePoints = xmlParser.getCurvePoints(5.0f, 100); // Tesselation level

	// early copy to allow renderer to display something
    this->draw_points = points;
	this->draw_objectInfo = objectInfo;
    this->curvePoints = curvePoints;
}

void Engine::loop() {

// linux can sleep for microseconds without active wait
// others cannot do that. however active wait sometimes still lags (in debug build, using clion, etc)
// so this third state exists where everything is done on the render thread

#ifndef NO_PHYS_THREAD
	std::thread physThread(&Engine::physLoopDeterministic, this);
	// este caso e para espera ativa, nao usado
	// std::thread physThread(&Engine::physLoopNonDeterministic, this);

	physThread.detach();
	renderLoop();

	// stop rendering from leaving main and destroying everything
	// wait for physThread to tell us it has exited
	std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(kill_mtx);
		kill = true; // signal physThread that it should be killed, only right before waiting in the condition. since we already have the lock physThread can never notivy before we wait
		killCondition.wait(lock);
#else
	renderLoop();
#endif
}

Engine::~Engine() {
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
