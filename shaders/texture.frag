#version 410 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexLayer;
uniform sampler2DArray u_TextureArraySlot;

void main() {
	// get texture from texture array
	vec4 texture_color = texture(u_TextureArraySlot, vec3(v_TexCoord.xy, v_TexLayer));


	// color = vec4(1.0);
	color = texture_color;
}
