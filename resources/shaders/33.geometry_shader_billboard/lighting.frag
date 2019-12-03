#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;
in vec2 fTexcoord;
in vec3 fNormal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuseMap_0;

void main()
{
    vec3 N = normalize(fNormal);
	vec3 L = normalize(lightPos - fFragPos);
	vec3 V = normalize(viewPos - fFragPos);
	vec3 H = normalize(L + V);

	vec3 color = texture(diffuseMap_0, fTexcoord).rgb;
	vec3 ambient = 0.2 * color;
	vec3 diffuse = max(dot(N, L), 0.0) * color;
	vec3 specular = pow(max(dot(N, H), 0.0), 16.0) * color;

	fragColor = vec4(ambient + diffuse + specular, 1.0);
}