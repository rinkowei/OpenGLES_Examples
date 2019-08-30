#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

uniform vec3 viewPos;
uniform samplerCube skybox;

in VS_OUT
{
	vec3 fFragPos;
	vec3 fNormal;
	vec3 fViewVec;
	vec3 fLightVec;
	mat4 fInvModelView;
}fs_in;

void main()
{
	vec3 cI = normalize(fs_in.fFragPos);
	vec3 cR = reflect(cI, normalize(fs_in.fNormal));

	cR = vec3(fs_in.fInvModelView * vec4(cR, 0.0f));
	cR.yz *= -1.0f;

	vec4 color = texture(skybox, cR);

	vec3 N = normalize(fs_in.fNormal);
	vec3 L = normalize(fs_in.fLightVec);
	vec3 V = normalize(fs_in.fViewVec);
	vec3 R = reflect(-L, N);
	vec3 ambient = vec3(0.5f) * color.rgb;
	vec3 diffuse = max(dot(N, L), 0.0f) * vec3(1.0f);
	vec3 specular = pow(max(dot(R, V), 0.0f), 16.0f) * vec3(0.5f);

    fragColor = vec4(ambient + diffuse * color.rgb + specular, 1.0f);
}