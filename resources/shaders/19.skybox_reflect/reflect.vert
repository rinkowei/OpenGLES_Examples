#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float lodBias;

out VS_OUT
{
	vec3 fFragPos;
	vec3 fNormal;
	vec3 fViewVec;
	vec3 fLightVec;
	mat4 fInvModelView;
}vs_out;


void main()
{
	vs_out.fFragPos = vec3(model * vec4(vPos, 1.0f));
	vs_out.fNormal = mat3(model) * vNormal;
	vs_out.fViewVec = -vs_out.fFragPos.xyz;
	vs_out.fLightVec = vec3(0.0f, -5.0f, -5.0f) - vs_out.fFragPos.xyz;
	vs_out.fInvModelView = inverse(model);

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}