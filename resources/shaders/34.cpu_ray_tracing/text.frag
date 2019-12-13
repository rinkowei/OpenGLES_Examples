#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D raytracingMap;

void main()
{
	fragColor = texture(raytracingMap, fTexcoord);
}