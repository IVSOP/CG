#ifndef RENDERER_OBJECT_INFO
#define RENDERER_OBJECT_INFO

#include "Material.h"
#include "Engine_Object.h"

struct RendererObjectInfo {
	glm::mat4 transf;  // mat4 alinha se com vec4
	Material material; // isto tbm se alinha, meti padding la dentro

	RendererObjectInfo() = default;

	// constructor to translate from an engine object info
	RendererObjectInfo(GLfloat texture_id, const Engine_Object_Info &info)
	: transf(info.transformation.transformMatrix), material(info.diffuse, info.ambient, info.specular, info.emissive, static_cast<GLfloat>(info.shininess), texture_id) {}
};
static_assert(sizeof(RendererObjectInfo) == 8 * sizeof(glm::vec4), "Error: PointLiRendererObjectInfoght has unexpected size");

#endif
