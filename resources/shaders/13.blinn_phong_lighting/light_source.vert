#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fTexCoord = vTexCoord;
	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}