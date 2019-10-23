#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fTexcoord;

void main()
{
	fTexcoord = vTexcoord;
	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}