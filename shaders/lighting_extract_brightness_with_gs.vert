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
	vec3 v_FragPos;
} vs_out;


uniform mat4 u_MVP;
uniform mat4 u_View;

void main()
{
	vs_out.v_TexCoord = aTexCoord;
	vs_out.v_MaterialID = aMaterialID;

	vs_out.v_Normal = mat3(transpose(inverse(u_View))) * aNormal; // inversion is costly and should be done on the CPU, this is temporary
	vs_out.v_FragPos = vec3(u_View * aPos);

	gl_Position = u_MVP * aPos;
}
