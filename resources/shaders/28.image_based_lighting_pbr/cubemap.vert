#version 310 es
layout(location = 0) in vec3 vPos;

out vec3 fFragPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	fFragPos = vPos;
	gl_Position = projection * view * vec4(vPos, 1.0f);
}