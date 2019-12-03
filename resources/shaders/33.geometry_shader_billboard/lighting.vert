#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;

out vec3 fFragPos;
out vec2 fTexcoord;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fFragPos = vec3(model * vec4(vPos, 1.0));
	fTexcoord = vTexcoord;
	fNormal = mat3(transpose(inverse(model))) * vNormal;

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}