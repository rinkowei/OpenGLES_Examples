#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform mat4 biasMatrix;

out VS_OUT
{
	vec2 fTexcoord;
	vec3 fNormal;
	vec3 fFragPos;
	vec4 fFragPosLightSpace;
}vs_out;

void main()
{
	vs_out.fTexcoord = vTexcoord;
	vs_out.fNormal = transpose(inverse(mat3(model))) * vNormal;
	vs_out.fFragPos = vec3(model * vec4(vPos, 1.0f));
	vs_out.fFragPosLightSpace = (biasMatrix * lightSpaceMatrix * model) * vec4(vPos, 1.0f);

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}