#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include "GLErrors.h"
#include <string>
#include <unordered_map>
#include <vector>

struct Shader { // this is actually a program but whatever
public:
	Shader() = delete;
	Shader(const char vertFile[], const char fragFile[]);
	Shader(const char vertFile[], const char fragFile[], const char geoFile[]);
	~Shader();

	void use() const;
	void setInt(const std::string &uniformName, GLint num);
	void setFloat(const std::string &uniformName, GLfloat num);
	void setVec3(const std::string &uniformName, const glm::vec3 &vec);
	void setVec3(const std::string &uniformName, GLfloat x, GLfloat y, GLfloat z);
	void setMat4(const std::string &uniformName, const glm::mat4 &mat);

	GLuint programID;
	std::unordered_map<std::string, GLint> uniformCache; // stores the uniform locations. when retrieving for the first time, goes to the program to get their location and stores it. obviously worse than just hardcoding for each shader but this it is cleaner

	// helper functions
	void loadShader(const char path[], GLenum shaderType, GLuint _program) const; // loads shader, not the program (eg fragment shader)
	
	// this should be called optionaly before rendering, if you have any issues. if the program is not ready to go it might fail
	// for example (since I am using 4.1 and cant specify the layout binding point) if I have 2 samplers and dont initialize them to texture slots with a texture it will throw an error (idfk)
	void validate() const;

	GLint getUniformLocation(const std::string &uniformName);
	// if needed make a getter for uniformCache
};

#endif
