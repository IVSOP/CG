#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aMaterialID;

out VS_OUT {
	vec3 v_Normal;
} vs_out;


uniform mat4 u_Model;
uniform mat4 u_View;

void main()
{
	// normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));????????????????????????
	vs_out.v_Normal = mat3(transpose(inverse(u_View * u_Model))) * aNormal; // inversion is costly and should be done on the CPU, this is temporary

	// projection is not multiplied here. everything goes into view space so it can be calculated on the gs, which will multiply by proj
	gl_Position = u_View * u_Model * aPos;
}
