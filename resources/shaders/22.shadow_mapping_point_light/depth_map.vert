#version 310 es
layout(location = 0) in vec3 vPos;

out vec3 fFragPos;

uniform mat4 model;
uniform mat4 lightMatrix;

void main()
{
	fFragPos = vec3(model * vec4(vPos, 1.0f));
	gl_Position = lightMatrix * model * vec4(vPos, 1.0f);
}