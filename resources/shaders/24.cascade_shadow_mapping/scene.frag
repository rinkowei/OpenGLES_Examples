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
uniform sampler2D depthMap[NUM_CASCADES];

float calculateShadow(int cascadeIndex, vec4 lightSpaceFragPos, vec3 normal, vec3 lightDir)
{
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;

	float bias = 0.005f * tan(acos(clamp(dot(normal, lightDir), 0.0f, 1.0f)));
	bias = clamp(bias, 0.0f, 0.0005f);

	ivec2 texSize = textureSize(depthMap[cascadeIndex], 0).xy;
	float scale = 1.0;
	float dx = scale * 1.0 / float(texSize.x);
	float dy = scale * 1.0 / float(texSize.y);

	float shadow = 0.0;
	int count = 0;
	int range = 1;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			float depth = texture(depthMap[cascadeIndex], vec2(projCoords.x + dx * float(x), projCoords.y + dy * float(y))).r;
			if (lightSpaceFragPos.w > 0.0 && depth < projCoords.z - bias)
			{
				shadow += 0.5;
			}
			else
			{
				shadow = 0.0;
			}
			count++;
		}
	}

	return shadow / float(count);
}
void main()
{
	vec3 albedo = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fNormal);

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
			shadow = calculateShadow(i, fLightSpaceFragPos[i], normal, lightDir);
			break;
		}
	}
    fragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0f);
}