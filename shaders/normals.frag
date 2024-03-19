#version 410 core

layout(location = 0) out vec4 color;

in GS_OUT {
	vec4 g_Color;
} fs_in;

void main() {
	color = fs_in.g_Color;
}
