#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in VS_OUT
{
	vec3 fFragPos;
	vec2 fTexcoord;
	vec3 fNormal;
}fs_in;

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};

uniform vec3 viewPos;
uniform float shininess;

uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform sampler2D diffuseMap_0;
uniform sampler2D specularMap_0;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	// diff irradiance
	float diff = max(dot(normal, lightDir), 0.0f);

	// spec irradiance
    vec3 halfwayVec = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayVec), 0.0f), shininess);

	vec3 ambient = light.ambient * vec3(texture(diffuseMap_0, fs_in.fTexcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap_0, fs_in.fTexcoord));
	vec3 specular = light.specular * spec * vec3(texture(specularMap_0, fs_in.fTexcoord));

	// combine results
	return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// diff irradiance
	float diff = max(dot(normal, lightDir), 0.0f);

	// spec irradiance
    vec3 halfwayVec = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayVec), 0.0f), shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * vec3(texture(diffuseMap_0, fs_in.fTexcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap_0, fs_in.fTexcoord));
	vec3 specular = light.specular * spec * vec3(texture(specularMap_0, fs_in.fTexcoord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// combine results
	return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// diff irradiance
	float diff = max(dot(normal, lightDir), 0.0f);

	// spec irradiance
    vec3 halfwayVec = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayVec), 0.0f), shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

	vec3 ambient = light.ambient * vec3(texture(diffuseMap_0, fs_in.fTexcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap_0, fs_in.fTexcoord));
	vec3 specular = light.specular * spec * vec3(texture(specularMap_0, fs_in.fTexcoord));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    // combine results
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.fFragPos);
	vec3 normal = normalize(fs_in.fNormal);

	// combine all the light sources
	vec3 result = calculateDirectionalLight(dirLight, normal, viewDir);
	result += calculatePointLight(pointLight, normal, fs_in.fFragPos, viewDir);
	result += calculateSpotLight(spotLight, normal, fs_in.fFragPos, viewDir);   

    fragColor = vec4(result, 1.0f);
}