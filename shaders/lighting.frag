#version 410 core

layout(location = 0) out vec4 color;

struct Material {
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	vec3 emissive;
	float shininess;

	float texture_id;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

// lookup buffer for materials
layout(std140) uniform u_MaterialBuffer {
    Material materials[];
} materialBuffer;

in vec2 v_TexCoord;
flat in float v_MaterialID;
uniform sampler2DArray u_TextureArraySlot;

void main() {
	// get texture from texture array
	// vec4 texture_color = texture(u_TextureArraySlot, vec3(v_TexCoord.xy, v_TexLayer));


	// color = texture_color;
	color = vec4(0.0, 0.0, 0.0, 1.0);
}
