#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fFragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 color = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fNormal);
	vec3 lightColor = vec3(0.8f);

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
	
    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}