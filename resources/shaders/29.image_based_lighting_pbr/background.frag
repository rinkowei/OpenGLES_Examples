#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;

uniform samplerCube environmentMap;

void main()
{
    vec3 envColor = texture(environmentMap, fFragPos).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
	fragColor = vec4(envColor, 1.0);
}