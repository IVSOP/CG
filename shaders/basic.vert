#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aColor;

out vec4 v_Color;

uniform mat4 u_MVP;

void main()
{
	v_Color = vec4(aColor, 1.0f);
	gl_Position = u_MVP * aPos;
}
