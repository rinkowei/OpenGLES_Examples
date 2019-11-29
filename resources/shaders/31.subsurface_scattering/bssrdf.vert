#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;

out vec3 fFragPos;
out vec3 fNormal;
out vec3 camDir;
out vec3 camNormal;
out vec4 lightSpaceFragPos;
out float lightDist;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 biasMatrix;

uniform mat4 lightView;
uniform mat4 lightProj;

void main()
{
	fFragPos = vec3(model * vec4(vPos, 1.0f));
	fNormal = mat3(model) * vNormal;
	camDir = -vec3(view * vec4(vPos, 1.0f));
	camNormal = normalize(mat3(view) * vNormal);
	lightSpaceFragPos = biasMatrix * lightProj * lightView * vec4(vPos, 1.0f);
	lightDist = length(lightView * vec4(vPos, 1.0f));

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
}