#version 310 es
layout(location = 0) in vec3 vPos;

out vec3 fFragPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	fFragPos = vec3(model * vec4(vPos, 1.0f));
	gl_Position = lightSpaceMatrix * model * vec4(vPos, 1.0f);
}