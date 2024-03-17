#ifndef COMMON_H
#define COMMON_H

#include <filesystem>
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLErrors.h"

#define PHYS_FPS 60.0f
#define PHYS_STEP 1.0f / PHYS_FPS

const GLchar *readFromFile(const char *filepath);

#endif
