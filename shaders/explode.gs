#version 410 core

// NOTE: settin normal and fragPos into viewspace was done in vertex shader, but since they are being changed I moved it to here
// The result was the same, wtf?? I am going insane

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in VS_OUT {
	vec2 v_TexCoord;
	vec3 v_Normal;
	flat vec3 v_Diffuse;
	flat vec3 v_Ambient;
	flat vec3 v_Specular;
	flat vec3 v_Emissive;
	flat float v_Shininess;
	flat float v_TexID;
	flat mat4 v_Transf;
} gs_in[];

out GS_OUT {
	vec2 g_TexCoord;
	vec3 g_Normal;
	vec3 g_FragPos;
	flat vec3 g_Diffuse;
	flat vec3 g_Ambient;
	flat vec3 g_Specular;
	flat vec3 g_Emissive;
	flat float g_Shininess;
	flat float g_TexID;
} gs_out;

uniform float u_ExplodeCoeff = 0.0;
uniform mat4 u_View;
uniform mat4 u_Projection;

vec4 explode(vec4 position, vec3 normal) {
	position.xyz += u_ExplodeCoeff * normal;
	return position;
}

vec3 getAvgNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(b, a));
}

void main() {
	vec3 normal = getAvgNormal();
	vec4 res;

	res = explode(gl_in[0].gl_Position, normal);
    gl_Position = u_Projection * u_View * gs_in[0].v_Transf * res;

    gs_out.g_TexCoord = gs_in[0].v_TexCoord;
	// calculate these things into view space
	gs_out.g_Normal = gs_in[0].v_Normal;
	gs_out.g_FragPos = vec3(u_View * gs_in[0].v_Transf * res);

	gs_out.g_Diffuse = gs_in[0].v_Diffuse;
	gs_out.g_Ambient = gs_in[0].v_Ambient;
	gs_out.g_Specular = gs_in[0].v_Specular;
	gs_out.g_Emissive = gs_in[0].v_Emissive;
	gs_out.g_Shininess = gs_in[0].v_Shininess;
	gs_out.g_TexID = gs_in[0].v_TexID;

	EmitVertex();

	res = explode(gl_in[1].gl_Position, normal);
	gl_Position = u_Projection * u_View * gs_in[1].v_Transf * res;

    gs_out.g_TexCoord = gs_in[1].v_TexCoord;
	// calculate these things into view space
	gs_out.g_Normal = gs_in[1].v_Normal;
	gs_out.g_FragPos = vec3(u_View * gs_in[1].v_Transf * res);

	gs_out.g_Diffuse = gs_in[1].v_Diffuse;
	gs_out.g_Ambient = gs_in[1].v_Ambient;
	gs_out.g_Specular = gs_in[1].v_Specular;
	gs_out.g_Emissive = gs_in[1].v_Emissive;
	gs_out.g_Shininess = gs_in[1].v_Shininess;
	gs_out.g_TexID = gs_in[1].v_TexID;

	EmitVertex();


	res = explode(gl_in[2].gl_Position, normal);
	gl_Position = u_Projection * u_View * gs_in[2].v_Transf * res;
    
    gs_out.g_TexCoord = gs_in[2].v_TexCoord;
	// calculate these things into view space
	gs_out.g_Normal = gs_in[2].v_Normal;
	gs_out.g_FragPos = vec3(u_View * gs_in[2].v_Transf * res);

	gs_out.g_Diffuse = gs_in[2].v_Diffuse;
	gs_out.g_Ambient = gs_in[2].v_Ambient;
	gs_out.g_Specular = gs_in[2].v_Specular;
	gs_out.g_Emissive = gs_in[2].v_Emissive;
	gs_out.g_Shininess = gs_in[2].v_Shininess;
	gs_out.g_TexID = gs_in[2].v_TexID;

	EmitVertex();
    
	
	EndPrimitive();

}  




// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];
