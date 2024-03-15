#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aMaterialID;

out vec2 v_TexCoord;
flat out float v_MaterialID; // flat since it is always the same between all vertices

uniform mat4 u_MVP;

void main()
{
	v_TexCoord = aTexCoord;
	v_MaterialID = aMaterialID;
	gl_Position = u_MVP * aPos;
}
