#version 310 es
layout(location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(vPos, 1.0f);
}