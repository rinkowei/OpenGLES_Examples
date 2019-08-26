#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;
uniform vec3 viewPos;

out VS_OUT
{
	vec2 fTexCoord;
	vec3 tangentFragPos;
	vec3 tangentLightDir;
	vec3 tangentViewPos;
}vs_out;

void main()
{
	vs_out.fTexCoord = vTexCoord;
	
	vec3 T = normalize(mat3(model) * vTangent);
	vec3 B = normalize(mat3(model) * vBitangent);
	vec3 N = normalize(mat3(model) * vNormal);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.tangentFragPos = TBN * vec3(model * vec4(vPos, 1.0f));
	vs_out.tangentLightDir = TBN * lightDir;
	vs_out.tangentViewPos = TBN * viewPos;

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}