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

float chebyshevUpperBound(vec3 shadowCoord)
{
	vec2 moments = texture(depthMap, shadowCoord.xy).rg;
	if (shadowCoord.z <= moments.x)
	{
		return 0.0;
	}

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.02);

	float d = shadowCoord.z - moments.x;
	float pMax = variance / (variance + d * d);

	return pMax;
}

void main()
{
	vec3 color = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fNormal);
	vec3 lightColor = vec3(0.5f);

	// ambient
	vec3 ambient = 0.3f * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - fFragPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;    

	vec3 shadowCoord = fFragPosLightSpace.xyz / fFragPosLightSpace.w;
	float shadow = chebyshevUpperBound(shadowCoord);
	
    fragColor = vec4(ambient + (1.0f - shadow) * (diffuse + specular), 1.0f);
}