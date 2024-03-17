#version 410 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_BrightBuffer;
uniform sampler2D u_SceneBuffer;
uniform float u_Gamma;
uniform float u_Exposure;

// void main()
// {
//     vec3 hdrColor = texture(u_HdrBuffer, v_TexCoord).rgb;
//     color = vec4(hdrColor, 1.0);
// }


// Reinhard tone mapping
// void main()
// {             
//     // const float gamma = 2.2;
//     vec3 hdrColor = texture(u_HdrBuffer, v_TexCoord).rgb;
  
//     // reinhard tone mapping
//     vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
//     // gamma correction 
//     mapped = pow(mapped, vec3(1.0 / u_Gamma));
  
//     color = vec4(mapped, 1.0);
// }

// tone mapping with gamma and exposure
void main()
{
    // const float gamma = 2.2;
	// const float exposure = 1.0;
    vec3 hdrColor = texture(u_SceneBuffer, v_TexCoord).rgb;
	vec3 bloomColor = texture(u_BrightBuffer, v_TexCoord).rgb;
  
	hdrColor += bloomColor; // additive blending

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * u_Exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / u_Gamma));
  
    color = vec4(mapped, 1.0);
}
