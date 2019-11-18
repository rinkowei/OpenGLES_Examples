#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

const int NUM_CASCADES = 3;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fFragPos;
in vec4 fViewSpaceFragPos;
in mat4 fViewMatrix;

struct DirectionalLight {
	vec3 color;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
};

uniform vec2 windowSize;
uniform vec4 cascadedSplits;
uniform int numOfCascades;
uniform mat4 lightMatrices[4];
uniform DirectionalLight dirLight;
uniform vec3 viewPos;
uniform highp sampler2DArray cascadedDepthMap;

float calculateShadow(vec3 lightDir, vec3 normal, float depthViewSpace, vec3 viewPosition)
{
	float positiveViewSpaceZ = depthViewSpace;
	int cascadeIndex = 0;

	for (int i = 0; i < numOfCascades - 1; ++i)
	{
		if (positiveViewSpaceZ < cascadedSplits[i])
		{
			cascadeIndex = i + 1;
		}
	}
	float angleBias = 0.006f;

	mat4 lightMatrix = lightMatrices[cascadeIndex];

	vec4 Texcoord = vec4(gl_FragCoord.x / windowSize.x, gl_FragCoord.y / windowSize.y, cascadeIndex, 1.0);

	vec4 fragModelViewSpace = vec4(viewPosition, 1.0);

	vec4 fragModelSpace = inverse(fViewMatrix) * fragModelViewSpace;

	vec4 fragLightSpace = lightMatrix * fragModelSpace;

	vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;

	float currentDepth = projCoords.z;

	projCoords.z = float(cascadeIndex);

	float bias = max(angleBias * (1.0 - dot(normal, lightDir)), 0.0008);

	float shadow = 0.0;

	ivec2 texelSize = textureSize(cascadedDepthMap, 0).xy;

	vec4 shadowCoord = vec4(0.0);
	float pcfDepth = texture(cascadedDepthMap, projCoords).r;

	shadow += currentDepth > pcfDepth - bias ? 1.0 : 0.0;

	return shadow;
}
void main()
{
	vec3 albedo = vec3(0.8, 0.8, 0.8);
	vec3 ambient = dirLight.ambientIntensity * dirLight.color * albedo;

	vec3 diffuse = max(dot(fNormal, -dirLight.direction), 0.0) * dirLight.color * dirLight.diffuseIntensity * albedo;

	vec3 viewDir = normalize(viewPos - fFragPos);
	vec3 halfwayDir = normalize(-dirLight.direction + viewDir);  
    float spec = pow(max(dot(fNormal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * dirLight.color * albedo;   
	
	float shadow = calculateShadow(dirLight.direction, fNormal, fViewSpaceFragPos.z, fViewSpaceFragPos.xyz);
    fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0);
	//fragColor = vec4(shadow, shadow, shadow, 1.0);
	//fragColor = vec4(vec3(texture(cascadedDepthMap, vec3(fViewSpaceFragPos.xy, 2)).r), 1.0);
}