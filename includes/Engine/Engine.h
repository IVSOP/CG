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
#include <condition_variable>

class Engine {
public:
    XmlParser &xmlParser;

	Engine(XmlParser &xmlParser);
	~Engine();

	void loop();
	void physLoopDeterministic();
	void physLoopNonDeterministic();
	void renderLoop();

	// infro from xml
	GLdouble windowFov;
	GLdouble windowZNear;
	GLdouble windowZFar;

	float *engine_speed = nullptr; // pointer for ease of use, it belongs to the renderer due to imgui. can easily be changed

	int windowWidth;
	int windowHeight;

	std::mutex mtx;
		// phys_<var> is used by physics thread
		// <var> is where that value is copied to
		// draw_<var> is used by render thread. values are copied from <var>
		// this way the only critical section is copying buffers
		// could be optimized to better reuse the vectors

		std::vector<Vertex> phys_points;
			std::vector<Vertex> points; // protected by mutex
		std::vector<Vertex> draw_points;
		std::vector<RendererObjectInfo> phys_objectInfo;
			std::vector<RendererObjectInfo> objectInfo; // protected by mutex
		std::vector<RendererObjectInfo> draw_objectInfo;
        std::vector<Engine_Object_Curve> phys_curvePoints;
			std::vector<Engine_Object_Curve> curvePoints; // protected by mutex
		std::vector<Engine_Object_Curve> draw_curvePoints;

		std::condition_variable killCondition;

	GLFWwindow *window = nullptr;
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<Renderer> renderer = nullptr;
	std::unique_ptr<InputHandler> inputHandler = nullptr;

	glm::mat4 projection = glm::mat4(1.0f);

	// isto e extremamente roto, mas nas docs basicamente diz que sou burro se tentar fazer como no glut e meter GLFW_CURSOR_HIDDEN e estar sempre a centra-lo, diz para usar GLFW_CURSOR_DISABLED
	// mas ao usar isso, sempre que dou resize, ele manda um mouse callback que lixa tudo, tive de fazer isto para esse callback ser ignorado
	// ou seja acho que vai ter de ficar assim
	bool resize = false;

	std::mutex kill_mtx;
		volatile bool kill = false; // estava a ter um bug quantico em que isto nao dava se eu nao desse print ao valor
		// experimentar usar a lock cada vez que quero ler o kill, mas podia ter pior performance
		// ou entao std::atomic<bool>
};

#endif
