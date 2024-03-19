#version 410 core

// NOTE: settin normal and fragPos into viewspace was done in vertex shader, but since they are being changed I moved it to here
// The result was the same, wtf?? I am going insane

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// info received from vertex shader. it is just passed through by the out below
in VS_OUT {
	vec2 v_TexCoord;
	flat float v_MaterialID; // flat since it is always the same between all vertices
	vec3 v_Normal;
	vec3 v_FragPos;
} gs_in[];

out GS_OUT {
	vec2 g_TexCoord;
	flat float g_MaterialID; // flat since it is always the same between all vertices
	vec3 g_Normal;
	vec3 g_FragPos;
} gs_out;

uniform float u_ExplodeCoeff = 1.0;

vec4 explode(vec4 position, vec3 normal) {
	position.xyz += u_ExplodeCoeff * normal;
	return position;
}

vec3 getAvgNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

void main() {
	vec3 normal = getAvgNormal();
	vec4 res;

    gl_Position = explode(gl_in[0].gl_Position, normal);

    gs_out.g_TexCoord = gs_in[0].v_TexCoord;
	gs_out.g_MaterialID = gs_in[0].v_MaterialID;
	// calculate these things into view space
	gs_out.g_Normal = gs_in[0].v_Normal;
	gs_out.g_FragPos = gs_in[0].v_FragPos;

    
	EmitVertex();

    
	gl_Position = explode(gl_in[1].gl_Position, normal);

    gs_out.g_TexCoord = gs_in[1].v_TexCoord;
	gs_out.g_MaterialID = gs_in[1].v_MaterialID;
	// calculate these things into view space
	gs_out.g_Normal = gs_in[1].v_Normal;
	gs_out.g_FragPos = gs_in[1].v_FragPos;
    
	EmitVertex();
    

	gl_Position = explode(gl_in[2].gl_Position, normal);
    
    gs_out.g_TexCoord = gs_in[2].v_TexCoord;
	gs_out.g_MaterialID = gs_in[2].v_MaterialID;
	// calculate these things into view space
	gs_out.g_Normal = gs_in[2].v_Normal;
	gs_out.g_FragPos = gs_in[2].v_FragPos;
    
	EmitVertex();
    
	
	EndPrimitive();

}  




// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];
