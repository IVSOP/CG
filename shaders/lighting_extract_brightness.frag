#version 410 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

struct Material { // !!!! IMPORTANT everything vec4 so that cpu matches gpu layout, opengl being stupid and me being lazy
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	vec4 shininess; // float
	uvec4 texture_id; // uint
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

#define MAX_LIGHTS 8
#define MAX_MATERIALS 8

// lookup buffer for materials
layout(std140) uniform u_MaterialBuffer { // binding 0
    Material materials[8];
};

// TEMPORARY
uniform PointLight u_PointLight;

in vec2 v_TexCoord;
flat in float v_MaterialID;
// these are both in view space
in vec3 v_Normal;
in vec3 v_FragPos;

uniform sampler2DArray u_TextureArraySlot;
uniform mat4 u_View; // view from the MVP

uniform float u_BloomThreshold = 1.0;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);

void main() {
	// get material from material array
	Material material = materials[int(trunc(v_MaterialID))]; // careful with this cast, need to change it out of a float

	// get texture from texture array
	vec4 res_color = texture(u_TextureArraySlot, vec3(v_TexCoord.xy, material.texture_id.x));

	// normal and viewDir
	// !!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANTE
	// daqui para a frente vou tar sempre a fazer coisas manhosas com isto e a view matrix
	// decidi usar view space em vez de world space, e por isso preciso da matriz em vez da posicao da camera. Vai dar tudo ao mesmo mas as contas sao mais manhosas, mas permite usar tecnicas igualmente manhosas no futuro
	vec3 viewDir = normalize(-v_FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir = (0,0,0) - FragPosition <=> viewDir = -FragPosition
	vec3 normal = normalize(v_Normal); // do this here or in the vertex shader?

	// apply colors
	res_color.rgb += CalcPointLight(u_PointLight, normal, v_FragPos, viewDir, material);

	// add emissive
	res_color.rgb += material.emissive.rgb;

	color = res_color;


	// extract bright colors into the separate color attachment
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)); // common approximation of luminance based on human perception of color (or so I'm told)
    if(brightness > u_BloomThreshold) {
        brightColor = vec4(color.rgb, 1.0);
	} else {
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material)
{
	// ambient
    vec3 ambient = light.ambient * material.ambient.xyz;

	// diffuse 
    vec3 lightDir = normalize(- (mat3(u_View) * light.direction)); // pretty sure this is bad but it works fine??????????????
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse.xyz);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess.x);
    vec3 specular = light.specular * (spec * material.specular.xyz);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
	vec3 viewSpace_position = vec3(u_View * vec4(light.position, 1.0));

    vec3 lightDir = normalize(viewSpace_position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess.x);
    // attenuation
    float distance    = length(viewSpace_position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			    			  light.quadratic * (distance * distance));    

    return ((light.ambient * material.ambient.xyz)
			 + (light.diffuse * diff * material.diffuse.xyz)
			 + (light.specular * spec * material.specular.xyz)) * attenuation;

    // vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // ambient  *= attenuation;
    // diffuse  *= attenuation;
    // specular *= attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
	vec3 viewSpace_position = vec3(u_View * vec4(light.position, 1.0));

    vec3 lightDir = normalize(viewSpace_position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess.x);
    // attenuation
    float distance = length(viewSpace_position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
	return ((light.ambient * material.ambient.xyz)
			+ (light.diffuse * diff * material.diffuse.xyz)
			+ (light.specular * spec * material.specular.xyz)) * attenuation * intensity;
}
