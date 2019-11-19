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

float filterPCF(vec4 shadowCoord, int cascadeIndex)
{
	ivec2 texelSize = textureSize(cascadedDepthMap, 0).xy;
	float scale = 0.75;
	float dx = scale * 1.0 / float(texelSize.x);
	float dy = scale * 1.0 / float(texelSize.y);

	float shadow = 0.0;
	int count = 0;
	int range = 1;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadow += calculateShadow(shadowCoord, vec2(dx * float(x), dy * float(y)), cascadeIndex);
			count++;
		}
	}
	return shadow / float(count);
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
	shadow = filterPCF(shadowCoord / shadowCoord.w, cascadeIndex);

	vec3 normal = normalize(fNormal);
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 viewDir = normalize(viewPos - fFragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	vec3 ambient = 0.2 * albedo * dirLight.color;

	vec3 diffuse = max(dot(normal, lightDir), 0.0) * dirLight.color * albedo;

    vec3 specular = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f) * dirLight.color; 

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
	fragColor = vec4(vec3(ambient + shadow * (diffuse + specular)), 1.0);
	//fragColor = vec4(vec3(texture(cascadedDepthMap, vec3(shadowCoord.xy, 2)).r), 1.0);
}