#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;

uniform float exposure;
uniform samplerCube environmentMap;

vec3 uncharted2Tonemapping(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
    vec3 envColor = textureLod(environmentMap, fFragPos, 0.0).rgb;
    
	envColor = uncharted2Tonemapping(envColor * exposure);
	envColor = envColor * (1.0 / uncharted2Tonemapping(vec3(11.2)));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
	fragColor = vec4(envColor, 1.0);
}