#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

uniform float LodBias;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform samplerCube skybox;

in vec3 fFragPos;
in vec3 fNormal;
in mat4 fInvModelView;

void main()
{
	vec3 cI = normalize(fFragPos);
	vec3 cR = reflect(cI, normalize(fNormal));

	cR = vec3(fInvModelView * vec4(cR, 0.0));
	cR.yz *= -1.0;

	vec4 color = textureLod(skybox, cR, LodBias);

	vec3 N = normalize(fNormal);
	vec3 L = normalize(lightPos - fFragPos);
	vec3 V = normalize(viewPos - fFragPos);
	vec3 R = reflect(-L, N);
	vec3 ambient = 0.5 * color.rgb;
	vec3 diffuse = max(dot(N, L), 0.0) * color.rgb;
	vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * color.rgb;

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}