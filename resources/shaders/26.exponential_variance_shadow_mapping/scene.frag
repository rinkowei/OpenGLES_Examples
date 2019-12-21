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

float chebyshev(vec2 moments, float depth)
{
	if (depth <= moments.x)
	{
		return 1.0;
	}

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.005);

	float d = depth - moments.x;
	float pMax = smoothstep(0.2, 1.0, variance / (variance + d * d));
	
	return 1.0 - pMax;
}

float calculateShadow(vec3 projCoords)
{
	//projCoords.z -= 0.00002;
	projCoords = projCoords * 0.5 + 0.5;

	vec4 moments = texture(depthMap, projCoords.xy);

	projCoords = projCoords * 2.0 - 1.0;

	float pos = exp(exponents.x * projCoords.z);
	float neg = -exp(-exponents.y * projCoords.z);

	float posShadow = chebyshev(moments.xy, pos);
	float negShadow = chebyshev(moments.zw, neg);
	float shadow = min(posShadow, negShadow);
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

	float shadow = 0.0;
	ivec2 texelSize = textureSize(depthMap, 0);
	float xoffset = 1.0 / float(texelSize.x);
	float yoffset = 1.0 / float(texelSize.y);
	vec3 shadowCoord = fFragPosLightSpace.xyz / fFragPosLightSpace.w;
	
	int count = 0;
	int kernel = 2;
	for (int x = -kernel; x <= kernel; x++)
	{
		for (int y = -kernel; y <= kernel; y++)
		{
			shadowCoord.xy += vec2(xoffset * float(x), yoffset * float(y));
			shadow += calculateShadow(shadowCoord);
			count++;
		}
	}
	shadow /= float(count);

    fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0);
}