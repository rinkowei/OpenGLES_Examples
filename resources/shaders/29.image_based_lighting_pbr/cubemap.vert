#version 310 es
layout(location = 0) in vec3 vPos;

out vec3 fFragPos;

uniform mat4 captureView;
uniform mat4 captureProjection;

void main()
{
	fFragPos = vPos;
	gl_Position = captureProjection * captureView * vec4(vPos, 1.0f);
}