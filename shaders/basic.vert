#version 450 core

// per vertex
layout (location = 0) in vec4 aPos;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * aPos;
}
