#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexcoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

const int NUM_CASCADES = 3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightMatrix[NUM_CASCADES];

out vec2 fTexcoord;
out vec3 fNormal;
out vec3 fFragPos;
out vec4 fLightSpaceFragPos[NUM_CASCADES];
out float fClipSpacePosZ;

void main()
{
	fTexcoord = vTexcoord;
	fNormal = transpose(inverse(mat3(model))) * vNormal;
	fFragPos = vec3(model * vec4(vPos, 1.0f));

	for (int i = 0; i < NUM_CASCADES; i++)
	{
		fLightSpaceFragPos[i] = lightMatrix[i] * model * vec4(vPos, 1.0f);
	}

	gl_Position = projection * view * model * vec4(vPos, 1.0f);
	fClipSpacePosZ = gl_Position.z;
}