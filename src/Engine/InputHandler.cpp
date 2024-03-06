#include "InputHandler.h"

InputHandler::InputHandler()
: keyInfo(std::make_unique<KeyInfo []>(MAX_KEYS_ID + 1)), curX(0.0f), curY(0.0f), lastX(0.0f), lastY(0.0f), inMenu(false)
{

}

void handleMouseMovInMenu(GLFWwindow *window, double xpos, double ypos) {
	// maneira manhosa de meter o imgui clicavel, no futuro vai para o inputhandler
	ImGui::GetIO().AddMousePosEvent(xpos, ypos);
}

void handleMouseClickInMenu(GLFWwindow* window, int button, int action, int mods) {
	// maneira manhosa de meter o imgui clicavel, no futuro vai para o inputhandler
	ImGui::GetIO().AddMouseButtonEvent(button, action == GLFW_PRESS ? true : false);
}

void InputHandler::pressKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	KeyInfo *keys = this->keyInfo.get();

	keys[key].newAction(action, mods);

	// bandaid fix temporario isto estar aqui, presskey nao devia fazer isto
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		if (inMenu) { // then go into engine
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetMouseButtonCallback(window, nullptr);
			// ImGui::SetMouseCursor(ImGuiMouseCursor_None); // acho que isto nao e preciso
			inMenu = false;
			glfwSetCursorPosCallback(window, handleMouseMov);
			glfwSetCursorPos(window, curX, curY);
		} else { // then go into menu
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow); // acho que isto nao e preciso
			inMenu = true;
			glfwSetMouseButtonCallback(window, handleMouseClickInMenu);
			glfwSetCursorPosCallback(window, handleMouseMovInMenu);
		}
	}
}

void InputHandler::centerMouseTo(GLdouble center_x, GLdouble center_y) {
	curX = center_x;
	curY = center_y;
	lastX = center_x;
	lastY = center_y;
}

void InputHandler::moveMouseTo(GLdouble x, GLdouble y) {
	if (!inMenu) {
		this->curX = x;
		this->curY = y;
	}
}

std::vector<KeyInfo> InputHandler::getKeysPressedOrHeld() const {
	std::vector<KeyInfo> res;
	const KeyInfo *keys = keyInfo.get();
	for (unsigned int i = 0; i < MAX_KEYS_ID + 1; i++) {
		if (keys[i].last_action != GLFW_RELEASE) {
			res.push_back(keys[i]);
		}
	}
	return res;
}

// glm::vec2 InputHandler::getMouseMovDelta() const {
// 	return glm::vec2(curX - lastX, curY - lastY);
// }

void InputHandler::applyToCamera(Camera &camera, int windowWidth, int windowHeight, GLfloat deltatime) {
	// muito mal feito, tbm nao tive paciencia mas funcemina

	const KeyInfo *keys = keyInfo.get();

	if ((&keys[GLFW_KEY_W])->last_action != GLFW_RELEASE) {
		camera.ProcessKeyboard(FRONT, deltatime);
	}
	if ((&keys[GLFW_KEY_S])->last_action != GLFW_RELEASE) {
		camera.ProcessKeyboard(BACK, deltatime);
	}
	if ((&keys[GLFW_KEY_A])->last_action != GLFW_RELEASE) {
		camera.ProcessKeyboard(LEFT, deltatime);
	}
	if ((&keys[GLFW_KEY_D])->last_action != GLFW_RELEASE) {
		camera.ProcessKeyboard(RIGHT, deltatime);
	}
	if ((&keys[GLFW_KEY_SPACE])->last_action != GLFW_RELEASE) {
		camera.ProcessKeyboard(UP, deltatime);
	}
	if ((&keys[GLFW_KEY_LEFT_ALT])->last_action != GLFW_RELEASE) {
		camera.ProcessKeyboard(DOWN, deltatime);
	}

	if (!inMenu) {
		// const int center_x = windowWidth / 2;
		// const int center_y = windowHeight / 2;

		const float xoffset = static_cast<GLfloat>(curX) - static_cast<GLfloat>(lastX);
		const float yoffset = static_cast<GLfloat>(lastY) - static_cast<GLfloat>(curY); // reversed since y-coordinates go from bottom to top

		lastX = curX;
		lastY = curY;

		// printf("Camera moving mouse from %f %f to %f %f\n", lastX, lastY, curX, curY);
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}
