#ifndef GLERRORS_H
#define GLERRORS_H

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <signal.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m" 
#define BOLDGREEN   "\033[1m\033[32m" 
#define BOLDYELLOW  "\033[1m\033[33m" 
#define BOLDBLUE    "\033[1m\033[34m" 
#define BOLDMAGENTA "\033[1m\033[35m" 
#define BOLDCYAN    "\033[1m\033[36m" 
#define BOLDWHITE   "\033[1m\033[37m" 

#define print_error(msg) printf("%s, %s, line %d:\n", __FILE__, __func__, __LINE__); perror(msg);

#define ASSERT(x) if (!(x)) raise(SIGTRAP);

// on mac and windows do nothing, skill issue
#if defined(__APPLE__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define GLCall(f) GLClearError();\
        f
#else
	#define GLCall(f) GLClearError();\
		f;\
		ASSERT(GLLogCall(#f, __FILE__, __LINE__))
#endif

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

void checkErrorInShader(GLuint shader, GLenum shaderType);

void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void validateProgram(const GLuint program);
void checkProgramLinking(const GLuint program);

#endif
