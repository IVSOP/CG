#version 410 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

struct Material {
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	vec3 emissive;
	float shininess;
	float texture_id;

	// there are 2 padding floats here. please see Material struct in the source code to understand
};
// basicaly how many vec4s would fit in a material (14 + 2 padding =  4 * vec4)
#define VEC4_IN_MATERIAL 4

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

// output from geometry shader and not vertex shader
in GS_OUT {
	vec2 g_TexCoord;
	flat uint g_MaterialID; // flat since it is always the same between all vertices
	vec3 g_Normal;
	vec3 g_FragPos;
} fs_in;

uniform sampler2DArray u_TextureArraySlot;
uniform samplerBuffer u_MaterialTBO;
uniform samplerBuffer u_PointLightTBO;
uniform mat4 u_View; // view from the MVP

uniform float u_BloomThreshold = 1.0;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);

void main() {
	// get material from material array
	// segurem se que vao haver manhosidades a ocorrer
	Material material;
	material.diffuse = texelFetch(u_MaterialTBO, 0 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).xyz;
	material.ambient.x = texelFetch(u_MaterialTBO, 0 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).w;
	material.ambient.yz = texelFetch(u_MaterialTBO, 1 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).xy;
	material.specular.xy = texelFetch(u_MaterialTBO, 1 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).zw;
	material.specular.z = texelFetch(u_MaterialTBO, 2 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).x;
	material.emissive.xyz = texelFetch(u_MaterialTBO, 2 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).yzw;
	material.shininess = texelFetch(u_MaterialTBO, 3 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).x;
	// material.texture_id = uintBitsToFloat(floatBitsToInt( texelFetch(u_MaterialTBO, 3 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).y ));
	material.texture_id = trunc( texelFetch(u_MaterialTBO, 3 + (int(trunc(fs_in.g_MaterialID) * VEC4_IN_MATERIAL))).y );




	// get texture from texture array
	vec4 res_color = texture(u_TextureArraySlot, vec3(fs_in.g_TexCoord.xy, material.texture_id));

	// normal and viewDir
	// !!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANTE
	// daqui para a frente vou tar sempre a fazer coisas manhosas com isto e a view matrix
	// decidi usar view space em vez de world space, e por isso preciso da matriz em vez da posicao da camera. Vai dar tudo ao mesmo mas as contas sao mais manhosas, mas permite usar tecnicas igualmente manhosas no futuro
	vec3 viewDir = normalize(-fs_in.g_FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir = (0,0,0) - FragPosition <=> viewDir = -FragPosition
	vec3 normal = normalize(fs_in.g_Normal);

	// apply colors
	// for now, there is only one point light, we need to get it from the array.
	// its index is hardcoded as 0
	PointLight pointLight;
	pointLight.position = texelFetch(u_PointLightTBO, 0 + (0 * VEC4_IN_POINTLIGHTS)).xyz;
    pointLight.constant = texelFetch(u_PointLightTBO, 0 + (0 * VEC4_IN_POINTLIGHTS)).w;
    pointLight.linear = texelFetch(u_PointLightTBO, 1 + (0 * VEC4_IN_POINTLIGHTS)).x;
    pointLight.quadratic = texelFetch(u_PointLightTBO, 1 + (0 * VEC4_IN_POINTLIGHTS)).y;
    pointLight.ambient.xy = texelFetch(u_PointLightTBO, 1 + (0 * VEC4_IN_POINTLIGHTS)).zw;
	pointLight.ambient.z = texelFetch(u_PointLightTBO, 2 + (0 * VEC4_IN_POINTLIGHTS)).x;
    pointLight.diffuse = texelFetch(u_PointLightTBO, 2 + (0 * VEC4_IN_POINTLIGHTS)).yzw;
    pointLight.specular = texelFetch(u_PointLightTBO, 3 + (0 * VEC4_IN_POINTLIGHTS)).xyz;

	res_color.rgb += CalcPointLight(pointLight, normal, fs_in.g_FragPos, viewDir, material);

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular.xyz);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
	vec3 viewSpace_position = vec3(u_View * vec4(light.position, 1.0));
	vec3 fragToLight = viewSpace_position - fragPos;

    vec3 lightDir = normalize(fragToLight);
    // diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(fragToLight);
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
	vec3 fragToLight = viewSpace_position - fragPos;

    vec3 lightDir = normalize(fragToLight);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(fragToLight);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
	return ((light.ambient * material.ambient.xyz)
			+ (light.diffuse * diff * material.diffuse.xyz)
			+ (light.specular * spec * material.specular.xyz)) * attenuation * intensity;
}
