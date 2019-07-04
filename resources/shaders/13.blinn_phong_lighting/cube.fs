#version 320 es
precision mediump float;
out vec4 fragColor;

in vec3 fFragPos;
in vec3 fNormal;
in vec2 fTexCoord;

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

uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform float shininess;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(normal, lightDir), 0.0f);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

	vec3 ambient = light.ambient * vec3(texture(diffuseMap, fTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, fTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(specularMap, fTexCoord));

	return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0f);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * vec3(texture(diffuseMap, fTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, fTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(specularMap, fTexCoord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0f);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

	vec3 ambient = light.ambient * vec3(texture(diffuseMap, fTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, fTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(specularMap, fTexCoord));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

void main()
{
	vec3 viewDir = normalize(viewPos - fFragPos);
	vec3 normal = normalize(fNormal);

	//vec3 result = calculateDirectionalLight(dirLight, normal, viewDir);
	//result += calculatePointLight(pointLight, normal, fFragPos, viewDir);
	//result += calculateSpotLight(spotLight, normal, fFragPos, viewDir);

    fragColor = vec4(texture(diffuseMap, fTexCoord).rgb, 1.0f);
}