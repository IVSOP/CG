#version 410 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define VEC4_IN_DIRLIGHTS 3

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define VEC4_IN_POINTLIGHTS 4

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define VEC4_IN_SPOTLIGHTS 5

// output from geometry shader and not vertex shader
in GS_OUT {
	vec2 g_TexCoord;
	vec3 g_FragPos;
	flat vec3 g_Emissive;
	flat float g_TexID;
} fs_in;

uniform sampler2DArray u_TextureArraySlot;

uniform mat4 u_View; // view from the MVP

uniform float u_BloomThreshold = 1.0;

void main() {	
	vec4 res_color = vec4(0.0, 0.0, 0.0, 1.0);

	// in view space
	vec3 viewDir = normalize(-fs_in.g_FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir = (0,0,0) - FragPosition <=> viewDir = -FragPosition

	// add emissive
	res_color.rgb += fs_in.g_Emissive;

	// apply texture at the end, merge colors
	color = res_color * texture(u_TextureArraySlot, vec3(fs_in.g_TexCoord.xy, fs_in.g_TexID));


	// extract bright colors into the separate color attachment
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)); // common approximation of luminance based on human perception of color (or so I'm told)
    if(brightness > u_BloomThreshold) {
        brightColor = vec4(color.rgb, 1.0);
	} else {
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
