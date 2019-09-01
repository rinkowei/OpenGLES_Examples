#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;
layout(location = 5) in vec3 vColor;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out VS_OUT
{
	vec2 fTexCoord;
	vec3 fFragPos;
	vec4 fFragPosLightSpace;
}vs_out;

void main()
{
	vs_out.fTexCoord = vTexCoord;
	vs_out.fFragPos = vec3(model * vec4(vPos, 1.0f));
	vs_out.fFragPosLightSpace = lightSpaceMatrix * vec4(vs_out.fFragPos, 1.0f);

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}