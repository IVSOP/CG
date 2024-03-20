#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"

struct Material {
	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec3 specular;
	glm::vec3 emissive;
	GLfloat shininess;
	// GLuint texture_id;
	GLfloat texture_id;

	// problem: acessing this data in the shader is done through a texture buffer, which can only(???) read vec4s
	// since there are 14 floats, the last vec4 reading ends in .y, if it is material [0]. but what if it is material [1]??
	// because I cant do wizardry with glsl, it is easier to pad this so that all accesses are consistent
	// this means adding 2 more floats
	// since this is extremely cursed and uncomfortable I will add an assertion below to make sure this manual padding has worked, so I can sleep at night
	GLfloat padding_1;
	GLfloat padding_2;

	Material() : diffuse(0.0f), ambient(0.0f), specular(0.0f), emissive(0.0f), shininess(0.0f), texture_id(0.0f) {}
	Material(const glm::vec3 &diffuse, const glm::vec3 &ambient, const glm::vec3 &specular, const glm::vec3 &emissive, GLfloat shininess, GLfloat texture_id)
	 : diffuse(diffuse), ambient(ambient), specular(specular), emissive(emissive), shininess(shininess), texture_id(texture_id) {}
	~Material() = default;
};
static_assert(sizeof(Material) == 4 * sizeof(glm::vec4), "Error: Material has unexpected size");

#endif
