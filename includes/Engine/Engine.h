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
	void physLoop();
	void renderLoop();


	// infro from xml
	GLdouble windowFov;
	GLdouble windowZNear;
	GLdouble windowZFar;

	int windowWidth;
	int windowHeight;

	std::mutex mtx;
		std::vector<Vertex> points;
		std::vector<Vertex> draw_points;
		std::vector<RendererObjectInfo> objectInfo;
		std::vector<RendererObjectInfo> draw_objectInfo;

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
