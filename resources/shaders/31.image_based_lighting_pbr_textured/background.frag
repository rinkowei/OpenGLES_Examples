#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;

uniform float exposure;
uniform samplerCube environmentMap;

vec3 acesToneMapping(vec3 color, float adaptedNum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adaptedNum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main()
{
    vec3 envColor = textureLod(environmentMap, fFragPos, 0.0).rgb;
    
	envColor = acesToneMapping(envColor, exposure);

    envColor = pow(envColor, vec3(1.0/2.2)); 
    
	fragColor = vec4(envColor, 1.0);
}