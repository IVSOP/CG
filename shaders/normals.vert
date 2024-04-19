#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in int aObjectID;

struct Material {
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	vec3 emissive;
	float shininess;
	float texture_id;

	// there are 2 padding floats here. please see Material struct in the source code to understand
};

struct ObjectInfo {
	mat4 transf;
	Material material;
};
#define VEC4_IN_OBJECTINFO 8

out VS_OUT {
	vec3 v_Normal;
} vs_out;


uniform mat4 u_View;
uniform samplerBuffer u_ObjectInfoTBO;


void main()
{
	ObjectInfo objectInfo;
	// mat4
	objectInfo.transf[0] = texelFetch(u_ObjectInfoTBO, 0 + (aObjectID * VEC4_IN_OBJECTINFO));
	objectInfo.transf[1] = texelFetch(u_ObjectInfoTBO, 1 + (aObjectID * VEC4_IN_OBJECTINFO));
	objectInfo.transf[2] = texelFetch(u_ObjectInfoTBO, 2 + (aObjectID * VEC4_IN_OBJECTINFO));
	objectInfo.transf[3] = texelFetch(u_ObjectInfoTBO, 3 + (aObjectID * VEC4_IN_OBJECTINFO));

	vs_out.v_Normal = normalize(mat3(transpose(inverse(u_View * objectInfo.transf))) * aNormal); // inversion is costly and should be done on the CPU, this is temporary

	// projection is not multiplied here. everything goes into view space so it can be calculated on the gs, which will multiply by proj
	gl_Position = u_View * objectInfo.transf * aPos;
}
