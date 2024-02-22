#include "InputHandler.h"

InputHandler::InputHandler()
: keyInfo(std::make_unique<KeyInfo []>(MAX_KEYS_ID + 1)), curX(0.0f), curY(0.0f)
{

}

////////////////////////////////////////////////////////////////////////////////////////// e preciso reformular isto tudo completamente para usar os codigos ASCII
////////////////////////////////////////////////////////////////////////////////////////// tive um total de 0 paciencia deixei so WASD+espaco
void InputHandler::pressKey(unsigned char key, int x, int y) {
	KeyInfo *keys = this->keyInfo.get();
	switch (key) {
		case 'w':
			keys[ENGINE_KEY_W].press();
			break;
		case 'a':
			keys[ENGINE_KEY_A].press();
			break;
		case 's':
			keys[ENGINE_KEY_S].press();
			break;
		case 'd':
			keys[ENGINE_KEY_D].press();
			break;
		case ' ':
			keys[ENGINE_KEY_SPACE].press();
			break;
		case 27: // codigo ascii do ESC, cursed mas nao encontrei macro no glut
			keys[ENGINE_KEY_ESCAPE].press();
			if (inMenu) {
				glutSetCursor(GLUT_CURSOR_NONE);
				inMenu = false;
				glutWarpPointer(curX, curY);
			} else {
				glutSetCursor(GLUT_CURSOR_INHERIT);
				inMenu = true;
			}
		default:
			break;
	}
}

void InputHandler::releaseKey(unsigned char key, int x, int y) {
	KeyInfo *keys = this->keyInfo.get();
	// e preciso reformular isto tudo completamente para usar os codigos ASCII
	// tive um total de 0 paciencia deixei so WASD+espaco
	switch (key) {
		case 'w':
			keys[ENGINE_KEY_W].release();
			break;
		case 'a':
			keys[ENGINE_KEY_A].release();
			break;
		case 's':
			keys[ENGINE_KEY_S].release();
			break;
		case 'd':
			keys[ENGINE_KEY_D].release();
			break;
		case ' ':
			keys[ENGINE_KEY_SPACE].release();
			break;
		case 27: // codigo ascii do ESC, cursed mas nao encontrei macro no glut
			keys[ENGINE_KEY_ESCAPE].release();
		default:
			break;
	}
}

void InputHandler::pressSpecialKey(int key, int x, int y) {
	KeyInfo *keys = this->keyInfo.get();

	switch (key) {
#ifdef __APPLE__
		case GLUT_KEY_DOWN:
#else
		case GLUT_KEY_ALT_L:
#endif
			keys[ENGINE_KEY_LEFT_ALT].press();
			break;
		default:
			break;
	}
}

void InputHandler::releaseSpecialKey(int key, int x, int y) {
	KeyInfo *keys = this->keyInfo.get();

	switch (key) {
#ifdef __APPLE__
		case GLUT_KEY_DOWN:
#else
		case GLUT_KEY_ALT_L:
#endif
			keys[ENGINE_KEY_LEFT_ALT].release();
			break;
		default:
			break;
	}
}

void InputHandler::moveMouseTo(int x, int y) {
	if (!inMenu) {
		this->curX = static_cast<float>(x);
		this->curY = static_cast<float>(y);
	}
}

std::vector<KeyInfo> InputHandler::getKeysPressedOrHeld() const {
	std::vector<KeyInfo> res;
	const KeyInfo *keys = keyInfo.get();
	for (unsigned int i = 0; i < MAX_KEYS_ID + 1; i++) {
		if (keys[i].last_action != ENGINE_KEY_RELEASE) {
			res.push_back(keys[i]);
		}
	}
	return res;
}

// glm::vec2 InputHandler::getMouseMovDelta() const {
// 	return glm::vec2(curX - lastX, curY - lastY);
// }

void InputHandler::applyToCamera(Camera &camera, int windowWidth, int windowHeight) {
	// muito mal feito, tbm nao tive paciencia mas funcemina

	const KeyInfo *keys = keyInfo.get();

	if ((&keys[ENGINE_KEY_W])->last_action != ENGINE_KEY_RELEASE) {
		camera.ProcessKeyboard(FRONT, PHYS_STEP);
	}
	if ((&keys[ENGINE_KEY_S])->last_action != ENGINE_KEY_RELEASE) {
		camera.ProcessKeyboard(BACK, PHYS_STEP);
	}
	if ((&keys[ENGINE_KEY_A])->last_action != ENGINE_KEY_RELEASE) {
		camera.ProcessKeyboard(LEFT, PHYS_STEP);
	}
	if ((&keys[ENGINE_KEY_D])->last_action != ENGINE_KEY_RELEASE) {
		camera.ProcessKeyboard(RIGHT, PHYS_STEP);
	}
	if ((&keys[ENGINE_KEY_SPACE])->last_action != ENGINE_KEY_RELEASE) {
		camera.ProcessKeyboard(UP, PHYS_STEP);
	}
	if ((&keys[ENGINE_KEY_LEFT_ALT])->last_action != ENGINE_KEY_RELEASE) {
		camera.ProcessKeyboard(DOWN, PHYS_STEP);
	}

	if (!inMenu) {
		const int center_x = windowWidth / 2;
		const int center_y = windowHeight / 2;

		const float xoffset = curX - center_x;
		const float yoffset = center_y - curY; // reversed since y-coordinates go from bottom to top

		if (curX != center_x || curY != center_y) {
			glutWarpPointer(center_x, center_y);
			camera.ProcessMouseMovement(xoffset, yoffset);
		}
	}
}
