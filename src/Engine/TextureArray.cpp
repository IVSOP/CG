#include "TextureArray.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

TextureArray::TextureArray(GLsizei _width, GLsizei _height, GLsizei _depth)
: width(_width), height(_height), depth(_depth), sp(0)
{
	GLsizei numMipmaps = 3;
	GLCall(glGenTextures(1, &this->ID));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID));

												// 3 + base level
	GLCall(glTexStorage3D(GL_TEXTURE_2D_ARRAY, numMipmaps + 1, GL_RGBA8, _width, _height, _depth)); // allocate storage for texture
	// uncommenting code below also works, idk if mipmaps work though
	// GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, _width, _height, _depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	// GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	// GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // BORDER???
	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // BORDER???
}

// is this all that is needed??
TextureArray::~TextureArray() {
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	GLCall(glDeleteTextures(1, &ID));
}

// texture array needs to be bound but not necessarily activated
void TextureArray::addTexture(const char path[]) {
	if (this->sp >= this->depth - 1) {
		fprintf(stderr, "Error in %s, sp exceeds max depth. This class is not prepared to handle such cases, change it to have multiple texture arrays and manage them or something\n", __func__);
		exit(EXIT_FAILURE);
	}

	stbi_set_flip_vertically_on_load(true);
	int _width, _height, BPP;
	unsigned char *buffer =	stbi_load(path, &_width, &_height, &BPP, 4); // 4 -> RGBA or just use STBI_rgb_alpha

	if (!buffer) {
		fprintf(stderr, "Error loading image in %s\n", __func__);
		exit(EXIT_FAILURE);
	}

	if (_width != this->width || _height != this->height) {
		#ifndef __WIN32
			fprintf(stderr, "%s: %sWARNING%s image dimensions for %s: Expected %d %d got %d %d. The image will be automatically resized.\n", __PRETTY_FUNCTION__, YELLOW, RESET, path, this->width, this->height, _width, _height);
		#else
			fprintf(stderr, "WARNING: image dimensions for %s: Expected %d %d got %d %d. The image will be automatically resized.\n", path, this->width, this->height, _width, _height);
		#endif
		unsigned char * resized_buffer = (unsigned char*) malloc(this->width * this->height * STBIR_RGBA); // STBIR_RGBA???????
		stbir_resize_uint8_linear(buffer, _width, _height, 0, resized_buffer, this->width, this->height, 0, STBIR_RGBA);

		// to simplify just pretend the buffer is the resized image
		free(buffer);
		buffer = resized_buffer;
	}

	if (BPP != 4) {
		fprintf(stderr, "%sWARNING:%s %s is not RGBA, BPP is %d\n", YELLOW, RESET, path, BPP);
	}

	// in the future, if needed, can use
	/*
	#define STB_IMAGE_RESIZE_IMPLEMENTATION
	#include "stb_image_resize2.h"

	unsigned char * resized_image = (unsigned char*) malloc(new_width * new_height * STBIR_RGBA);
	stbir_resize_uint8_linear(image, width, height, 0, resized_image, new_width, new_height, 0, STBIR_RGBA);
	*/
	// to resize the image

	constexpr GLint LOD = 0,
	xoffset = 0,
	yoffset = 0;
	// Z-offset is used to place the image in the correct place
	constexpr GLsizei depth = 1; 
	// depth is actually if the image were 3D. it isnt so I just set it to 1
	GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LOD, xoffset, yoffset, this->sp, this->width, this->height, depth, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
	// IMPORTANT this is very bad since it generates mipmaps for ALL the textures, but since they are not changed at runtime it's not too bad (for now)
	GLCall(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

	this->sp ++;
	free(buffer);
}

void TextureArray::setTextureArrayToSlot(const GLuint slot) {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, ID));
}
