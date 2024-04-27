#version 410 core

// per vertex
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;
#define aObjectID 0


// normal not needed, always emissive only

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
	flat vec3 v_Emissive;
	flat float v_TexID;
	flat mat4 v_Transf;
} vs_out;


uniform mat4 u_View;
uniform samplerBuffer u_ObjectInfoTBO;
uniform float u_Time = 0.0;


void main()
{
	ObjectInfo objectInfo;
	// mat4
	objectInfo.transf[0] = texelFetch(u_ObjectInfoTBO, 0 + (aObjectID * VEC4_IN_OBJECTINFO));
	objectInfo.transf[1] = texelFetch(u_ObjectInfoTBO, 1 + (aObjectID * VEC4_IN_OBJECTINFO));
	objectInfo.transf[2] = texelFetch(u_ObjectInfoTBO, 2 + (aObjectID * VEC4_IN_OBJECTINFO));
	objectInfo.transf[3] = texelFetch(u_ObjectInfoTBO, 3 + (aObjectID * VEC4_IN_OBJECTINFO));

	// material
	objectInfo.material.emissive.xyz = texelFetch(u_ObjectInfoTBO, 6 + (aObjectID * VEC4_IN_OBJECTINFO)).yzw;
	objectInfo.material.texture_id = trunc( texelFetch(u_ObjectInfoTBO, 7 + (aObjectID * VEC4_IN_OBJECTINFO)).y );

	// vs_out.v_TexCoord = aTexCoord;
	vs_out.v_Emissive = objectInfo.material.emissive;
	vs_out.v_TexID = objectInfo.material.texture_id;
	vs_out.v_Transf = objectInfo.transf;
	

	// vs_out.v_Normal = normalize(mat3(transpose(inverse(u_View * objectInfo.transf))) * aNormal); // inversion is costly and should be done on the CPU, this is temporary

	const vec3 sun_center = vec3(0, 0, 0);
	vec3 dir = normalize(aPos.xyz - sun_center);
	vec3 new_dir = dir;

	new_dir += ((cos((dir.y + (u_Time * 0.005)) * 45.0) * 0.1) + (sin((dir.x + (u_Time * 0.005)) * 45.0) * 0.1)) * 0.1;

	vec2 uv = aTexCoord.xy;
	uv.y += ((cos((uv.y + (u_Time * 0.04)) * 45.0) * 0.0019) + (cos((uv.y + (u_Time * 0.1)) * 10.0) * 0.002)) * 0.25;
    uv.x += ((sin((uv.y + (u_Time * 0.07)) * 15.0) * 0.0029) + (sin((uv.y + (u_Time * 0.1)) * 15.0) * 0.002)) * 0.25;
	vs_out.v_TexCoord = uv;

	gl_Position = aPos + vec4(new_dir, 0.0);
}
