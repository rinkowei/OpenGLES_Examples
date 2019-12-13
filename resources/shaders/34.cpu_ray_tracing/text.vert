#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;

out vec2 fTexcoord;

void main()
{
	fTexcoord = vTexcoord;
    gl_Position = vec4(vPos, 1.0);
}