#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <memory>
#include <vector>
#include "common.h"
#include "Camera.h"


#define MAX_KEYS_ID GLFW_KEY_MENU


struct KeyInfo {
	KeyInfo() {
		last_action = GLFW_RELEASE;
		last_mods = 0;

		// current_action = GLFW_RELEASE;
		// current_mods = 0;
	}
	~KeyInfo() = default;

	void press(int mods = 0) {
		// pode ver se key ja foi clicada ou assim, por agora mudo so o estado para pressed
		last_action = GLFW_PRESS;
	}

	void newAction(int action, int mods) {
		last_action = action;
		last_mods = mods;
	}

	void release() {
		last_action = GLFW_RELEASE;
	}

	// bool pressedNow() const { // true se era released e passou a pressed
	// 	// fazer isto com | ??
	// 	if (last_action == GLFW_RELEASE && current_action == GLFW_PRESS) {
	// 		return true;
	// 	}

	// 	return false;
	// }

	int last_action; //, current_action;
	int last_mods; //, current_mods;
};

class InputHandler {
public:
	// Estado sobre teclas pressionadas, rato, etc
	std::unique_ptr<KeyInfo []> keyInfo; // [MAX_KEYS_ID + 1]

	// mouse
	GLdouble curX;
	GLdouble curY;
	GLdouble lastX;
	GLdouble lastY;

	bool inMenu; // faz o rato nao virar a camera


	// TEMPORARY
	GLFWcursorposfun handleMouseMov;

	InputHandler();
	~InputHandler() = default;

	void pressKey(GLFWwindow *window, int key, int scancode, int action, int mods);
	void centerMouseTo(GLdouble center_x, GLdouble center_y); // same as below but also changes the last position
	void moveMouseTo(GLdouble x, GLdouble y);

	// devolve teclas com estado diferente de RELEASE
	std::vector<KeyInfo> getKeysPressedOrHeld() const;
	// delta em que o rato se moveu
	// glm::vec2 getMouseMovDelta() const;


	// por simplicidade, inputHandler passa as coisas a camera diretamente
	void applyToCamera(GLFWwindow * window, Camera &camera, int windowWidth, int windowHeight);
};

#endif
