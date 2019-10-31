#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

void main()
{
	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}