#version 320 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out VS_OUT
{
	vec3 fFragPos;
	vec3 fNormal;
	vec2 fTexCoord;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.fFragPos = vec3(model * vec4(vPos, 1.0f));
	vs_out.fNormal = mat3(transpose(inverse(model))) * vNormal;
	vs_out.fTexCoord = vTexCoord;

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}