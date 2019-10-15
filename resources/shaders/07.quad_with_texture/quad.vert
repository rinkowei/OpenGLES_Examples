#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;

out vec2 fTexCoord;

void main()
{
	fTexCoord = vTexCoord;
	gl_Position = vec4(vPos, 1.0f);
}