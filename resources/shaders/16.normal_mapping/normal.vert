#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec2 fTexcoord;
out vec3 fTangentFragPos;
out vec3 fTangentLightPos;
out vec3 fTangentViewPos;

void main()
{
	fTexcoord = vTexcoord;
	
	vec3 T = normalize(mat3(model) * vTangent);
	vec3 B = normalize(mat3(model) * vBitangent);
	vec3 N = normalize(mat3(model) * vNormal);
	mat3 TBN = transpose(mat3(T, B, N));

	fTangentFragPos = TBN * vec3(model * vec4(vPos, 1.0f));
	fTangentLightPos = TBN * lightPos;
	fTangentViewPos = TBN * viewPos;

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}