#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aObjectID;

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

// basicaly how many vec4s would fit in a ObjectInfo (14 + 2 padding + mat4 =  4 * vec4 + 4 * vec4)
#define VEC4_IN_OBJECTINFO 8

out VS_OUT {
	vec2 v_TexCoord;
	vec3 v_Normal;
	flat vec3 v_Diffuse;
	flat vec3 v_Ambient;
	flat vec3 v_Specular;
	flat vec3 v_Emissive;
	flat float v_Shininess;
	flat float v_TexID;
	flat mat4 v_Transf;
} vs_out;


uniform mat4 u_View;
uniform samplerBuffer u_ObjectInfoTBO;



// objectInfo is accessed here. could access half here and half in frag shader, or have 2 different TBOs, but this is the easiest solution and we are not really worried about performance



void main()
{
	ObjectInfo objectInfo;
	// mat4
	objectInfo.transf[0] = texelFetch(u_ObjectInfoTBO, 0 + (int(aObjectID * VEC4_IN_OBJECTINFO)));
	objectInfo.transf[1] = texelFetch(u_ObjectInfoTBO, 1 + (int(aObjectID * VEC4_IN_OBJECTINFO)));
	objectInfo.transf[2] = texelFetch(u_ObjectInfoTBO, 2 + (int(aObjectID * VEC4_IN_OBJECTINFO)));
	objectInfo.transf[3] = texelFetch(u_ObjectInfoTBO, 3 + (int(aObjectID * VEC4_IN_OBJECTINFO)));

	// material
	objectInfo.material.diffuse = texelFetch(u_ObjectInfoTBO, 4 + (int(aObjectID * VEC4_IN_OBJECTINFO))).xyz;
	objectInfo.material.ambient.x = texelFetch(u_ObjectInfoTBO, 4 + (int(aObjectID * VEC4_IN_OBJECTINFO))).w;
	objectInfo.material.ambient.yz = texelFetch(u_ObjectInfoTBO, 5 + (int(aObjectID * VEC4_IN_OBJECTINFO))).xy;
	objectInfo.material.specular.xy = texelFetch(u_ObjectInfoTBO, 5 + (int(aObjectID * VEC4_IN_OBJECTINFO))).zw;
	objectInfo.material.specular.z = texelFetch(u_ObjectInfoTBO, 6 + (int(aObjectID * VEC4_IN_OBJECTINFO))).x;
	objectInfo.material.emissive.xyz = texelFetch(u_ObjectInfoTBO, 6 + (int(aObjectID * VEC4_IN_OBJECTINFO))).yzw;
	objectInfo.material.shininess = texelFetch(u_ObjectInfoTBO, 7 + (int(aObjectID * VEC4_IN_OBJECTINFO))).x;
	objectInfo.material.texture_id = trunc( texelFetch(u_ObjectInfoTBO, 7 + (int(aObjectID * VEC4_IN_OBJECTINFO))).y );



	vs_out.v_TexCoord = aTexCoord;
	vs_out.v_Diffuse = objectInfo.material.diffuse;
	vs_out.v_Ambient = objectInfo.material.ambient;
	vs_out.v_Specular = objectInfo.material.specular;
	vs_out.v_Emissive = objectInfo.material.emissive;
	vs_out.v_Shininess = objectInfo.material.shininess;
	vs_out.v_TexID = objectInfo.material.texture_id;
	vs_out.v_Transf = objectInfo.transf;
	


	// normalize this??????????????????????????????????????????????
	vs_out.v_Normal = mat3(transpose(inverse(u_View * objectInfo.transf))) * aNormal; // inversion is costly and should be done on the CPU, this is temporary


	// I left these values untouched since the geometry shader will change the position of the vertices, this way it takes care of everything at once
	// vs_out.v_FragPos = vec3(u_View * aPos);
	// gl_Position = u_Projection * u_View * objectInfo.transf * aPos;
	gl_Position = aPos;
}
