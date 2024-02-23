#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <GLUT/glut.h>
// #include <GL/freeglut.h>
#else
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#define FPS 60.0f
#define PHYS_STEP 1.0f / FPS

#endif
