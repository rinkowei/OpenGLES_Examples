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
uniform vec3 albedo;
uniform float roughness;
uniform float metallic;
uniform float ao;

uniform float exposure;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform Light lights[6];

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness)
{	
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float geometrySchlickGGX(vec3 N, vec3 V, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float NdotV = max(dot(N, V), 0.0);
	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float ggx1 = geometrySchlickGGX(N, L, roughness);
	float ggx2 = geometrySchlickGGX(N, V, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlickFast(vec3 F0, vec3 V, vec3 H)
{
	return F0 + (1.0 - F0) * exp2((-5.55473 * dot(V, H) - 6.98316) * dot(V, H));
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 N = normalize(fNormal);
	vec3 V = normalize(viewPos - fFragPos);
	vec3 R = reflect(-V, N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	for (int i = 0; i < lights.length(); i++)
	{
		vec3 L = normalize(lights[i].position - fFragPos);
		vec3 H = normalize(V + L);
		float distance = length(lights[i].position - fFragPos);
		float attenuation = 1.0 / pow(distance, 2.0);
		vec3 radiance = lights[i].color * attenuation;

		float D = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlickFast(F0, V, H);

		vec3 nominator = D * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = nominator / max(denominator, 0.001);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - metallic;

		Lo += (kD * albedo / PI + specular) * radiance * max(dot(N, L), 0.0);
	}

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	//ambient = 0.03 * albedo;
	vec3 color = ambient + Lo;

	color = vec3(1.0) - exp(-color * exposure);
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1.0);
}