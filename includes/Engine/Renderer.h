#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vertex.h"
#include "Camera.h"

#include <vector>

class Renderer {
public:

	Renderer() = default;
	~Renderer() = default;

	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, Camera &camera, GLFWwindow * window) const;
};


#endif
