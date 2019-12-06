#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;
in vec3 fNormal;
in vec3 camDir;
in vec3 camNormal;
in vec4 lightSpaceFragPos;
in float lightDist;

uniform vec4 albedo;
uniform vec4 scatterColor;
uniform float wrap;
uniform float scatterWidth;
uniform float scatterFalloff;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform sampler2D depthMap;

float wrapLighting(float x, float wrap)
{
	return (x + wrap) / (1.0 + wrap);
}

float scatterTint(float x, float scatterWidth)
{
	if (scatterWidth == 0.0)
	{
		return 0.0;
	}

	return smoothstep(0.0, scatterWidth, x) * smoothstep(scatterWidth * 2.0, scatterWidth, x); 
}

void main()
{
	float depthInWC = texture(depthMap, lightSpaceFragPos.xy).r;
	float depthOutWC = lightSpaceFragPos.z;

	float scatterFalloff = exp(-abs(depthOutWC - depthInWC) * scatterFalloff);

	vec3 normal = normalize(fNormal);

	float NdotL = dot(normal, -lightDir);
	float NdotL_wrap = wrapLighting(NdotL, wrap);

	float diffuse = max(NdotL, 0.0);
	float scatter = scatterTint(NdotL_wrap, scatterWidth);

	fragColor = albedo * diffuse + scatterColor * scatter * scatterFalloff;
}