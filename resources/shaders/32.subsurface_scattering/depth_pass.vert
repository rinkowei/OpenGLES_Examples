#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 lightView;
uniform mat4 lightProj;

void main()
{
	gl_Position = lightProj * lightView * model * vec4(vPos, 1.0f);
}