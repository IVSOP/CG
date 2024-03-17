#version 410 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 v_TexCoord;

void main()
{             
    v_TexCoord = aTexCoord;
    gl_Position = aPos;
} 
