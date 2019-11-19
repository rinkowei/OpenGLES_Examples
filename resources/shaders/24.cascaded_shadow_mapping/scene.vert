#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fTexcoord;
out vec3 fNormal;
out vec3 fPos;
out vec3 fFragPos;
out vec3 fViewSpaceFragPos;

void main()
{
	fTexcoord = vTexcoord;
	fNormal = transpose(inverse(mat3(model))) * vNormal;
	fPos = vPos;
	fFragPos = vec3(model * vec4(vPos, 1.0));
	fViewSpaceFragPos = (view * model * vec4(vPos, 1.0f)).xyz;

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}