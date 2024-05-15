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
	vec3 g_Normal;
	vec3 g_FragPos;
	flat vec3 g_Diffuse;
	flat vec3 g_Ambient;
	flat vec3 g_Specular;
	flat vec3 g_Emissive;
	flat float g_Shininess;
	flat float g_TexID;
} fs_in;

uniform sampler2DArray u_TextureArraySlot;
uniform samplerBuffer u_PointLightTBO;
uniform samplerBuffer u_DirLightTBO;
uniform samplerBuffer u_SpotLightTBO;
uniform mat4 u_View; // view from the MVP
uniform int u_NumPointLights = 0;
uniform int u_NumDirLights = 0;
uniform int u_NumSpotLights = 0;

uniform float u_BloomThreshold = 1.0;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);

void main() {
	Material material = Material(fs_in.g_Diffuse, fs_in.g_Ambient, fs_in.g_Specular, fs_in.g_Emissive, fs_in.g_Shininess, fs_in.g_TexID);
	
	vec4 res_color = vec4(0.0, 0.0, 0.0, 1.0);

	// normal and viewDir
	// !!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANTE tudo em view space
	vec3 viewDir = normalize(-fs_in.g_FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir = (0,0,0) - FragPosition <=> viewDir = -FragPosition
	vec3 normal = normalize(fs_in.g_Normal);

	
	// apply pointlights
	PointLight pointLight;
	for (int i = 0; i < u_NumPointLights; i++) {
		pointLight.position = texelFetch(u_PointLightTBO, 0 + (i * VEC4_IN_POINTLIGHTS)).xyz;
		pointLight.constant = texelFetch(u_PointLightTBO, 0 + (i * VEC4_IN_POINTLIGHTS)).w;
		pointLight.linear = texelFetch(u_PointLightTBO, 1 + (i * VEC4_IN_POINTLIGHTS)).x;
		pointLight.quadratic = texelFetch(u_PointLightTBO, 1 + (i * VEC4_IN_POINTLIGHTS)).y;
		pointLight.ambient.xy = texelFetch(u_PointLightTBO, 1 + (i * VEC4_IN_POINTLIGHTS)).zw;
		pointLight.ambient.z = texelFetch(u_PointLightTBO, 2 + (i * VEC4_IN_POINTLIGHTS)).x;
		pointLight.diffuse = texelFetch(u_PointLightTBO, 2 + (i * VEC4_IN_POINTLIGHTS)).yzw;
		pointLight.specular = texelFetch(u_PointLightTBO, 3 + (i * VEC4_IN_POINTLIGHTS)).xyz;
		res_color.rgb += CalcPointLight(pointLight, normal, fs_in.g_FragPos, viewDir, material);
	}

	DirLight dirLight;
	for (int i = 0; i < u_NumDirLights; i++) {
		dirLight.direction = texelFetch(u_DirLightTBO, 0 + (i * VEC4_IN_DIRLIGHTS)).xyz;
		dirLight.ambient.x = texelFetch(u_DirLightTBO, 0 + (i * VEC4_IN_DIRLIGHTS)).w;
		dirLight.ambient.yz = texelFetch(u_DirLightTBO, 1 + (i * VEC4_IN_DIRLIGHTS)).xy;
		dirLight.diffuse.xy = texelFetch(u_DirLightTBO, 1 + (i * VEC4_IN_DIRLIGHTS)).zw;
		dirLight.diffuse.z = texelFetch(u_DirLightTBO, 2 + (i * VEC4_IN_DIRLIGHTS)).x;
		dirLight.specular = texelFetch(u_DirLightTBO, 2 + (i * VEC4_IN_DIRLIGHTS)).yzw;
		res_color.rgb += CalcDirLight(dirLight, normal, viewDir, material);
	}

	SpotLight spotLight;
	for (int i = 0; i < u_NumSpotLights; i++) {
		spotLight.position = texelFetch(u_SpotLightTBO, 0 + (i * VEC4_IN_SPOTLIGHTS)).xyz;
		spotLight.direction.x = texelFetch(u_SpotLightTBO, 0 + (i * VEC4_IN_SPOTLIGHTS)).w;
		spotLight.direction.yz = texelFetch(u_SpotLightTBO, 1 + (i * VEC4_IN_SPOTLIGHTS)).xy;
		spotLight.cutOff = texelFetch(u_SpotLightTBO, 1 + (i * VEC4_IN_SPOTLIGHTS)).z;
		spotLight.outerCutOff = texelFetch(u_SpotLightTBO, 1 + (i * VEC4_IN_SPOTLIGHTS)).w;
		spotLight.constant = texelFetch(u_SpotLightTBO, 2 + (i * VEC4_IN_SPOTLIGHTS)).x;
		spotLight.linear = texelFetch(u_SpotLightTBO, 2 + (i * VEC4_IN_SPOTLIGHTS)).y;
		spotLight.quadratic = texelFetch(u_SpotLightTBO, 2 + (i * VEC4_IN_SPOTLIGHTS)).z;
		spotLight.ambient.x = texelFetch(u_SpotLightTBO, 2 + (i * VEC4_IN_SPOTLIGHTS)).w;
		spotLight.ambient.yz = texelFetch(u_SpotLightTBO, 3 + (i * VEC4_IN_SPOTLIGHTS)).xy;
		spotLight.diffuse.xy = texelFetch(u_SpotLightTBO, 3 + (i * VEC4_IN_SPOTLIGHTS)).zw;
		spotLight.diffuse.z = texelFetch(u_SpotLightTBO, 4 + (i * VEC4_IN_SPOTLIGHTS)).x;
		spotLight.specular = texelFetch(u_SpotLightTBO, 4 + (i * VEC4_IN_SPOTLIGHTS)).yzw;
		res_color.rgb += CalcSpotLight(spotLight, normal, fs_in.g_FragPos, viewDir, material);
	}



	// add emissive
	res_color.rgb += material.emissive.rgb;

	// apply texture at the end, merge colors
	color = res_color * texture(u_TextureArraySlot, vec3(fs_in.g_TexCoord.xy, material.texture_id));


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
	// removed the '-' since we need direction TOWARDS the light and already have it, no need to invert the vector
    vec3 lightDir = normalize( (mat3(u_View) * light.direction));
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse.xyz);

    // specular
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.000001), material.shininess); // apparently shininess is usually 0, but pow(0, 0) is undefined according to https://registry.khronos.org/OpenGL-Refpages/gl4/html/pow.xhtml, I spent hours on this fml
    vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.000001), material.shininess);
	
	vec3 specular = light.specular * (spec * material.specular.xyz);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
	vec3 viewSpace_position = vec3(u_View * vec4(light.position, 1.0));
	vec3 fragToLight = viewSpace_position - fragPos;

	// lightDir TOWARDS light, it is more useful
    vec3 lightDir = normalize(fragToLight);
    // diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.000001), material.shininess); // apparently shininess is usually 0, but pow(0, 0) is undefined according to https://registry.khronos.org/OpenGL-Refpages/gl4/html/pow.xhtml, I spent hours on this fml
    vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.000001), material.shininess);
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

	// lightDir TOWARDS the light, it is more useful
    vec3 lightDir = normalize(fragToLight);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.000001), material.shininess); // apparently shininess is usually 0, but pow(0, 0) is undefined according to https://registry.khronos.org/OpenGL-Refpages/gl4/html/pow.xhtml, I spent hours on this fml
    vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.000001), material.shininess);
	// attenuation
    float distance = length(fragToLight);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
									   // - here since we use direction towards the light. could also do - lightdir, whatever
    float theta = dot(lightDir, normalize(-(mat3(u_View) * light.direction)));

	// outra forma de interpolar, mas nao usa outerCutOff
	// float intensity = 0.0;
	// if (theta > light.cutOff) {
	// 	intensity = (1.0 - ((1.0 - theta) / (1.0 - light.cutOff)));
	// }

    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	return ((light.ambient * material.ambient.xyz) /* intensity not applied to ambient */
			+ (light.diffuse * diff * material.diffuse.xyz * intensity)
			+ (light.specular * spec * material.specular.xyz * intensity)) * attenuation;
}
