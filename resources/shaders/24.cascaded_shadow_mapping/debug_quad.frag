#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform highp sampler2DArray cascadedDepthMap;

void main() 
{
	fragColor = vec4(vec3(texture(cascadedDepthMap, vec3(fTexcoord, 0)).r), 1.0);
}