#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fFragPos;
out vec3 fNormal;
out mat4 fInvModelView;

void main()
{
	fFragPos = vec3(model * vec4(vPos, 1.0));
	fNormal = mat3(transpose(inverse(model))) * vNormal;
	fInvModelView = inverse(model);

	vec3 lightPos = vec3(0.0, -5.0, 5.0);
	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}