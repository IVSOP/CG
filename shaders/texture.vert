#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexID;

out vec2 v_TexCoord;
out vec4 v_Color;
flat out float v_TexLayer;

uniform mat4 u_MVP;

void main()
{
	v_TexCoord = aTexCoord;
	v_TexLayer = aTexID;
	v_Color = aColor;
	gl_Position = u_MVP * aPos;
}
