#version 320 es
layout(location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fUVW;

void main()
{
	fUVW = vPos;
	gl_Position = projection * view * vec4(vPos, 1.0f);
}