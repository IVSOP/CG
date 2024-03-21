#ifndef TEXTUREARRAY_H
#define TEXTUREARRAY_H

#include "common.h"

// these are for convenience outside this class and are never used here
#define TEXTURE_LAYERS 64
#define TEXTURE_WIDTH 1024
#define TEXTURE_HEIGHT 1024

class TextureArray {
	public:
		TextureArray() = delete;
		TextureArray(GLsizei width,	GLsizei height, GLsizei depth); // create an array of depth images, each has those dimensions
		~TextureArray();

		// will check if width and height of provided image are correct
		void addTexture(const char path[]); // adds a texture to the array. ID is automatically set, like a stack
		void setTextureArrayToSlot(const GLuint slot);
	private:
		GLsizei width;
		GLsizei height;
		GLsizei depth;
		GLuint ID;
		GLsizei sp; // stack pointer
};

#endif
