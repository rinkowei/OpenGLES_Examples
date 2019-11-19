#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

const int MAX_SPLITS = 4;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fPos;
in vec3 fFragPos;
in vec3 fViewSpaceFragPos;

struct DirectionalLight {
	vec3 color;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
};

uniform float cascadedSplits[MAX_SPLITS];
uniform mat4 lightSpaceMatrices[MAX_SPLITS];
uniform DirectionalLight dirLight;
uniform vec3 viewPos;
uniform highp sampler2DArray cascadedDepthMap;
uniform mat4 biasMatrix;

float calculateShadow(vec4 shadowCoord, vec2 offset, int cascadeIndex)
{
	float shadow = 1.0;
	float bias = 0.005;

	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float dist = texture(cascadedDepthMap, vec3(shadowCoord.xy + offset, cascadeIndex)).r;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z - bias)
		{
			shadow = 0.1;
		}
	}
	return shadow;
}
void main()
{
	vec3 albedo = vec3(0.8, 0.8, 0.8);
	
	int cascadeIndex = 0;
	for (int i = 0; i < MAX_SPLITS - 1; i++)
	{
		if (fViewSpaceFragPos.z < cascadedSplits[i])
		{
			cascadeIndex = i + 1;
		}
	}
	cascadeIndex = 0;
	vec4 shadowCoord = (biasMatrix * lightSpaceMatrices[cascadeIndex]) * vec4(fFragPos, 1.0);

	float shadow = 0.0;
	shadow = calculateShadow(shadowCoord / shadowCoord.w, vec2(0.0, 0.0), cascadeIndex);

	vec3 normal = normalize(fNormal);
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 H = normalize(lightDir + fViewSpaceFragPos);

	vec3 ambient = 0.3 * albedo * dirLight.ambientIntensity * dirLight.color;

	vec3 diffuse = max(dot(normal, lightDir), 0.0) * dirLight.color * dirLight.diffuseIntensity * albedo;

	vec3 cascadeColor = vec3(0.0, 0.0, 0.0);

	switch (cascadeIndex)
	{
		case 0 :
			cascadeColor = vec3(0.1f, 0.0f, 0.0f);
			break;
		case 1 :
			cascadeColor = vec3(0.0f, 0.1f, 0.0f);
			break;
		case 2:
			cascadeColor = vec3(0.0f, 0.0f, 0.1f);
			break;
		case 3:
			cascadeColor = vec3(0.0f, 0.1f, 0.1f);
			break;
	}
	fragColor = vec4(vec3(ambient + shadow * diffuse) + cascadeColor, 1.0);
	//fragColor = vec4(vec3(texture(cascadedDepthMap, vec3(shadowCoord.xy, 2)).r), 1.0);
}