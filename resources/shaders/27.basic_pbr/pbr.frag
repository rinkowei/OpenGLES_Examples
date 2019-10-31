#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;
in vec2 fTexcoord;
in vec3 fNormal;

struct Light {
	vec3 color;
	vec3 position;
};

uniform vec3 viewPos;
uniform float roughness;
uniform float metallic;

uniform Light lights[4];

const float PI = 3.14159265359;

const vec3 albedo = vec3(1.0f, 0.765557f, 0.336057f);

float GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2) / (PI * denom * denom);
}

float SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

vec3 FresnelSchlick(float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04), albedo, metallic); 
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
	return F;    
}

vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness, vec3 lightColor)
{
	vec3 H = normalize(V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0)
	{
		float D = GGX(dotNH, roughness);
		float G = SchlicksmithGGX(dotNL, dotNV, roughness);
		vec3 F = FresnelSchlick(dotNV, metallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

void main()
{
	vec3 N = normalize(fNormal);
	vec3 V = normalize(viewPos - fFragPos);

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < lights.length(); i++)
	{
		vec3 L = normalize(lights[i].position - fFragPos);
		Lo += BRDF(L, V, N, metallic, roughness, lights[i].color);
	}

	vec3 color = albedo * 0.02;
	color += Lo;

	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1.0f);
}