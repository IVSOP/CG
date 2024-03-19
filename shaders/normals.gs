#version 410 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 v_Normal;
} gs_in[];

out GS_OUT {
	vec4 g_Color;
} gs_out;

uniform mat4 u_Projection;

void GenerateLine(int index)
{
	const float MAGNITUDE = 1.0;

    gl_Position = u_Projection * gl_in[index].gl_Position;
	gs_out.g_Color = vec4(0.0, 0.0, 1.0, 1.0);
    EmitVertex();

    gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(gs_in[index].v_Normal, 0.0) * MAGNITUDE);
	gs_out.g_Color = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}  




// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];
