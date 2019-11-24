#version 310 es
layout(location = 0) in vec3 vPos;

out vec3 fFragPos;

uniform mat4 captureView;
uniform mat4 captureProj;

void main()
{
	fFragPos = vPos;
	gl_Position = captureProj * captureView * vec4(vPos, 1.0f);
}