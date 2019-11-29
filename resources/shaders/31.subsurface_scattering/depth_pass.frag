#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in float distance;

void main()
{
	fragColor = vec4(distance);
}