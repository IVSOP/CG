#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aMaterialID;

out vec2 v_TexCoord;
flat out float v_MaterialID; // flat since it is always the same between all vertices
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_MVP;
uniform mat4 u_View; // view from the MVP

void main()
{
	v_TexCoord = aTexCoord;
	v_MaterialID = aMaterialID;

	// calculate these things into view space (multiply view my identity?????)
	v_Normal = mat3(transpose(inverse(u_View))) * aNormal; // inversion is costly and should be done on the CPU, this is temporary
	v_FragPos = vec3(u_View * aPos); // is aPos.z always 1.0???????

	vec4 idk = aPos;
	// idk +=  vec4(aNormal, 1.0);
	gl_Position = u_MVP * idk;
}
