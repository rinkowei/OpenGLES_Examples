#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;


in VS_OUT
{
	vec2 fTexCoord;
	vec3 fFragPos;
	vec4 fFragPosLightSpace;
}fs_in;

//uniform sampler2D diffuseMap_0;
uniform sampler2D depthMap;

float calculateShadow(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = 1.0f;
	ivec2 texelSize = textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * 1.0f / vec2(texelSize.x, texelSize.y)).r; 
            shadow += currentDepth > pcfDepth ? 1.0f : 0.0f;        
        }    
    }

    shadow /= 9.0;

	if (projCoords.z > 1.0f)
	{
		shadow = 0.0f;
	}

	return shadow;
}

void main()
{
	vec4 diffuseColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	float shadow = calculateShadow(fs_in.fFragPosLightSpace);
    fragColor = (1.0f - shadow) * diffuseColor;
}