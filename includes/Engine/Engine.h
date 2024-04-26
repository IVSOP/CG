#ifndef ENGINE_H
#define ENGINE_H

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

class Engine {
public:
    XmlParser &xmlParser;

	Engine(XmlParser &xmlParser);
	~Engine();

	void loop();
	void physLoopDeterministic();
	void physLoopNonDeterministic();
	void renderLoop();

	bool rendered = false; // sets if the current data has been rendered, so threads can coordinate

	// infro from xml
	GLdouble windowFov;
	GLdouble windowZNear;
	GLdouble windowZFar;

	float *engine_speed = nullptr; // pointer for ease of use, it belongs to the renderer due to imgui. can easily be changed

	int windowWidth;
	int windowHeight;

	std::mutex mtx;
		// <var> are used by phys thread, copied into draw_<var> to be used in rendering
		// could be optimized so vectors are switched instead of being deep copied all the time
		std::vector<Vertex> points;
		std::vector<Vertex> draw_points;
		std::vector<RendererObjectInfo> objectInfo;
		std::vector<RendererObjectInfo> draw_objectInfo;
        std::vector<Engine_Object_Curve> curvePoints;
		std::vector<Engine_Object_Curve> draw_curvePoints;

	GLFWwindow *window = nullptr;
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<Renderer> renderer = nullptr;
	std::unique_ptr<InputHandler> inputHandler = nullptr;

	glm::mat4 projection = glm::mat4(1.0f);

	// isto e extremamente roto, mas nas docs basicamente diz que sou burro se tentar fazer como no glut e meter GLFW_CURSOR_HIDDEN e estar sempre a centra-lo, diz para usar GLFW_CURSOR_DISABLED
	// mas ao usar isso, sempre que dou resize, ele manda um mouse callback que lixa tudo, tive de fazer isto para esse callback ser ignorado
	// ou seja acho que vai ter de ficar assim
	bool resize = false;

	bool kill = false;

};

#endif
