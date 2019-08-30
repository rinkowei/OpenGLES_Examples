#version 320 es
layout(location = 0) in vec3 vPos;
//layout(location = 1) in vec2 vTexCoord;
//layout(location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}