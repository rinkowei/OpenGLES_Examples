#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in VS_OUT
{
	vec2 fTexcoord;
	vec3 fNormal;
	vec3 fFragPos;
	vec4 fFragPosLightSpace;
}fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D depthMap;

float calculateShadow(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	vec3 normal = normalize(fs_in.fNormal);
	vec3 lightDir = normalize(lightPos - fs_in.fFragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005f);

	float shadow = 0.0f;
	ivec2 texelSize = textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) / vec2(texelSize.x, texelSize.y)).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;        
        }    
    }

    shadow /= 9.0f;

	if (projCoords.z > 1.0f)
	{
		shadow = 0.0f;
	}

	return shadow;
}

void main()
{
	vec3 color = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fs_in.fNormal);
	vec3 lightColor = vec3(0.5f);

	// ambient
	vec3 ambient = 0.3f * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - fs_in.fFragPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - fs_in.fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;    

	float shadow = calculateShadow(fs_in.fFragPosLightSpace);
    fragColor = vec4(ambient + (1.0f - shadow) * (diffuse + specular), 1.0f);
}