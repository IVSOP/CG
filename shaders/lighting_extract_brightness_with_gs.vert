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
	// vec3 v_FragPos;
} vs_out;


uniform mat4 u_Model;
uniform mat4 u_View;

void main()
{
	vs_out.v_TexCoord = aTexCoord;
	vs_out.v_MaterialID = aMaterialID;

	vs_out.v_Normal = mat3(transpose(inverse(u_View * u_Model))) * aNormal; // inversion is costly and should be done on the CPU, this is temporary


	// I left these values untouched since the geometry shader will change the position of the vertices, this way it takes care of everything at once
	// vs_out.v_FragPos = vec3(u_View * aPos);
	// gl_Position = u_Projection * u_View * u_Model * aPos;
	gl_Position = aPos;
}
