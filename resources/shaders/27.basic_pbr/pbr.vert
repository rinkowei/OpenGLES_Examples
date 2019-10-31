#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

out vec2 fTexcoord;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fTexcoord = vTexcoord;
	fNormal = mat3(model) * vNormal;
	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}