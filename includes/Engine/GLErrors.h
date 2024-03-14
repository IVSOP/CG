#ifndef GLERRORS_H
#define GLERRORS_H

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <signal.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define print_error(msg) printf("%s, %s, line %d:\n", __FILE__, __func__, __LINE__); perror(msg);

#define ASSERT(x) if (!(x)) raise(SIGTRAP);

// on mac and windows do nothing, skill issue
#if defined(__APPLE__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define GLCall(f) f;
#else
	#define GLCall(f) GLClearError();\
		f;\
		ASSERT(GLLogCall(#f, __FILE__, __LINE__))
#endif

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

void checkErrorInShader(GLuint shader);

void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void validateProgram(const GLuint program);
void checkProgramLinking(const GLuint program);

#endif