#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fFragPos;
in vec4 fFragPosLightSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D depthMap;

const vec2 exponents = vec2(80.0, 80.0);

float linearStep(float minVal, float maxVal, float val)
{
	return clamp((val - minVal) / (maxVal - minVal), 0.0, 1.0);
}

float reduceLightBleed(float pMax, float amount)
{
	return linearStep(amount, 1.0, pMax);
}

float chebyshev(vec2 moments, float depth)
{
	if (depth <= moments.x)
	{
		return 1.0;
	}

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.005);

	float d = depth - moments.x;
	float pMax = variance / (variance + d * d);
	
	return reduceLightBleed(1.0 - pMax, 0.2);
}

float calculateShadow(vec4 shadowCoord)
{
	vec3 projCoords = shadowCoord.xyz / shadowCoord.w;

	//projCoords.z -= 0.005;
	projCoords = projCoords * 0.5 + 0.5;

	float shadow = 0.0;

	vec4 moments = texture(depthMap, projCoords.xy);

	projCoords = projCoords * 2.0 - 1.0;

	float pos = exp(exponents.x * projCoords.z);
	float neg = -exp(-exponents.y * projCoords.z);

	float posShadow = chebyshev(moments.xy, pos);
	float negShadow = chebyshev(moments.zw, neg);
	shadow = min(posShadow, negShadow);
	return shadow;
}

void main()
{
	vec3 albedo = vec3(0.8, 0.8, 0.8);
	vec3 normal = normalize(fNormal);
	vec3 lightColor = vec3(0.6);

	// ambient
	vec3 ambient = 0.2 * albedo;

	// diffuse
	vec3 lightDir = normalize(lightPos - fFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * albedo * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0);
    vec3 specular = spec * lightColor;    

	vec3 shadowCoord = fFragPosLightSpace.xyz / fFragPosLightSpace.w;
	//float shadow = chebyshev(texture(depthMap, shadowCoord.xy).rg, shadowCoord.z);
	float shadow = calculateShadow(fFragPosLightSpace);
    fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0);
}