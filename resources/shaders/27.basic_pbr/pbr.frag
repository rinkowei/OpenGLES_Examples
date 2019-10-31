#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fNormal;

void main()
{
	fragColor = vec4(fNormal * 10.0f, 1.0f);
}