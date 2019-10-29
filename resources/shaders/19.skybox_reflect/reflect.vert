#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fFragPos;
out vec3 fNormal;
out vec3 fViewVec;
out vec3 fLightVec;
out mat4 fInvModel;


void main()
{
	fFragPos = vec3(model * vec4(vPos, 1.0f));
	fNormal = mat3(model) * vNormal;
	fViewVec = -fFragPos.xyz;
	fLightVec = vec3(0.0f, -5.0f, -5.0f) - fFragPos.xyz;
	fInvModel = inverse(model);

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}