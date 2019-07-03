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

void main()
{
    fragColor = texture(diffuseTexture, fTexCoord);
}