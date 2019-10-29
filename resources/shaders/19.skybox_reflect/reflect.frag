#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

uniform samplerCube skybox;

in vec3 fFragPos;
in vec3 fNormal;
in vec3 fViewVec;
in vec3 fLightVec;
in mat4 fInvModel;


void main()
{
	vec3 cI = normalize(fFragPos);
	vec3 cR = reflect(cI, normalize(fNormal));

	cR = vec3(fInvModel * vec4(cR, 0.0f));
	cR.yz *= -1.0f;

	vec4 color = texture(skybox, cR);

	vec3 N = normalize(fNormal);
	vec3 L = normalize(fLightVec);
	vec3 V = normalize(fViewVec);
	vec3 R = reflect(-L, N);
	vec3 ambient = vec3(0.5f) * color.rgb;
	vec3 diffuse = max(dot(N, L), 0.0f) * vec3(1.0f);
	vec3 specular = pow(max(dot(R, V), 0.0f), 16.0f) * vec3(0.5f);

    fragColor = vec4(ambient + diffuse * color.rgb + specular, 1.0f);
}