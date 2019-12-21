#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform float exposure;

void main()
{
	const float gamma = 2.2;
    vec3 hdrColor = texture(scene, fTexcoord).rgb;      
    vec3 bloomColor = texture(bloomBlur, fTexcoord).rgb;

    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
	//fragColor = vec4(texture(bloomBlur, fTexcoord).rgb, 1.0f);
}