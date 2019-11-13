#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

const int NUM_CASCADES = 3;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fFragPos;
in vec4 fLightSpaceFragPos[NUM_CASCADES];
in float fClipSpacePosZ;

struct DirectionalLight {
	vec3 color;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
};

uniform DirectionalLight dirLight;
uniform vec3 viewPos;
uniform float cascadeEndClipSpace[NUM_CASCADES];
uniform sampler2D depthMap0;
uniform sampler2D depthMap1;
uniform sampler2D depthMap2;

float calculateShadow(int cascadeIndex, vec4 lightSpaceFragPos)
{
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.z;
	projCoords = projCoords * 0.5 + 0.5;
	float depth = 0.0;
	if (cascadeIndex == 0)
		depth = texture(depthMap0, projCoords.xy).r;
	else if (cascadeIndex == 1)
		depth = texture(depthMap1, projCoords.xy).r;
	else if (cascadeIndex == 2)
		depth = texture(depthMap2, projCoords.xy).r;
	if (depth < projCoords.z + 0.00001)
	{
		return 0.5;
	}
	else
	{
		return 1.0;
	}
}
void main()
{
	vec3 albedo = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fNormal);
	vec3 lightColor = vec3(0.8f);

	// ambient
	vec3 ambient = dirLight.ambientIntensity * dirLight.color * albedo;

	// diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * dirLight.diffuseIntensity * dirLight.color;

	// specular
    vec3 viewDir = normalize(viewPos - fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * dirLight.color;   
	
	float shadow = 0.0f;
	for (int i = 0; i < NUM_CASCADES; i++)
	{
		if (fClipSpacePosZ <= cascadeEndClipSpace[i])
		{
			shadow = calculateShadow(i, fLightSpaceFragPos[i]);
			break;
		}
	}
	
    //fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0f);
	fragColor = vec4(vec3(texture(depthMap0, fLightSpaceFragPos[0].xy).r), 1.0f);
}