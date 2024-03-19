#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aMaterialID;

out VS_OUT {
	vec2 v_TexCoord;
	flat float v_MaterialID; // flat since it is always the same between all vertices
	vec3 v_Normal;
} vs_out;


uniform mat4 u_MVP;

void main()
{
	vs_out.v_TexCoord = aTexCoord;
	vs_out.v_MaterialID = aMaterialID;
	vs_out.v_Normal = aNormal;

	gl_Position = u_MVP * aPos;
}
