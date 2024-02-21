#ifndef RENDERER_H
#define RENDERER_H

#include "Vertex.h"
#include "Camera.h"

#include <vector>

class Renderer {
public:

	Renderer() = default;
	~Renderer() = default;

	// isto devia ser const vec mas nao foi por causa de encapsulamentos estupidos parabens aos envolvidos
	void draw(std::vector<Vertex> &verts, Camera &camera) const;
};


#endif
