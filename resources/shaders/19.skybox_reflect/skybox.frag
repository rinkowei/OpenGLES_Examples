#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fUVW;

uniform float LodBias;
uniform samplerCube cubemap;

void main()
{
    vec3 color = textureLod(cubemap, fUVW, LodBias).rgb;
    
	fragColor = vec4(color, 1.0);
}