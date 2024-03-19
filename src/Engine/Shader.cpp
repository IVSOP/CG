#include "Shader.h"

Shader::Shader(const char vertFile[], const char fragFile[]) {
	GLCall(this->programID = glCreateProgram());
	loadShader(vertFile, GL_VERTEX_SHADER, programID);
	loadShader(fragFile, GL_FRAGMENT_SHADER, programID);
	GLCall(glLinkProgram(programID));
	checkProgram();
}

Shader::Shader(const char vertFile[], const char fragFile[], const char geoFile[]) {
	puts("\n\n\n\nloading");
	puts(vertFile);
	GLCall(this->programID = glCreateProgram());
	loadShader(vertFile, GL_VERTEX_SHADER, programID);
	loadShader(fragFile, GL_FRAGMENT_SHADER, programID);
	loadShader(geoFile, GL_GEOMETRY_SHADER, programID);
	GLCall(glLinkProgram(programID));
	checkProgram();
	puts(vertFile);
	puts("finished\n\n\n\n");
}

Shader::~Shader() {
	GLCall(glDeleteProgram(programID));
}

void Shader::use() const {
	GLCall(glUseProgram(programID));
}

void Shader::checkProgram() const {
	checkProgramLinking(programID);
	validateProgram(programID);
}

void Shader::loadShader(const char path[], GLenum shaderType, GLuint _program) const {
	const GLchar *buff = readFromFile(path);
	GLCall(GLuint shader = glCreateShader(shaderType));
	GLCall(glShaderSource(shader, 1, &buff, NULL)); // ??? why &buff? does it set it to null on error??
	GLCall(glCompileShader(shader));
	GLCall(checkErrorInShader(shader));
	GLCall(glAttachShader(_program, shader));
	delete[] buff;

	GLCall(glDeleteShader(shader));
}

GLint Shader::getUniformLocation(const std::string &uniformName) {
	// can use of map here be better??????
	if (uniformCache.find(uniformName) != uniformCache.end()) {
		return uniformCache[uniformName];

	}

	// else uniform not in cache
	GLCall(int location = glGetUniformLocation(programID, uniformName.c_str()));

	if (location == -1) {
		fprintf(stderr, "Warning: uniform '%s' doesn't exist\n", uniformName.c_str());
		return -1;
	}
	
	uniformCache[uniformName] = location;
	return location;
}

void Shader::setInt(const std::string &uniformName, GLint num) {
	GLint location = getUniformLocation(uniformName);
	GLCall(glUniform1i(location, num));
}

void Shader::setFloat(const std::string &uniformName, GLfloat num) {
	GLint location = getUniformLocation(uniformName);
	GLCall(glUniform1f(location, num));
}

void Shader::setVec3(const std::string &uniformName, const glm::vec3 &vec) {
	GLint location = getUniformLocation(uniformName);
	GLCall(glUniform3fv(location, 1, glm::value_ptr(vec)));
}

void Shader::setVec3(const std::string &uniformName, GLfloat x, GLfloat y, GLfloat z) {
	GLint location = getUniformLocation(uniformName);
	GLCall(glUniform3f(location, x, y, z));
}

void Shader::setMat4(const std::string &uniformName, const glm::mat4 &mat) {
	GLint location = getUniformLocation(uniformName);
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
}
