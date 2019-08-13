#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

out VS_OUT
{
	vec3 fFragPos;
	vec2 fTexCoord;
	vec3 fNormal;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.fFragPos = vec3(model * vec4(vPos, 1.0f));
	vs_out.fTexCoord = vTexCoord;
	vs_out.fNormal = mat3(transpose(inverse(model))) * vNormal;

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}