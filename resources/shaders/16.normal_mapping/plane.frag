#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in VS_OUT
{
	vec2 fTexCoord;
	vec3 tangentFragPos;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
}fs_in;

uniform sampler2D diffuseMap_0;
uniform sampler2D normalMap_0;

void main()
{
    vec3 V = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
    vec3 N = normalize(texture(normalMap_0, fs_in.fTexCoord).rgb * 2.0f - 1.0f);
    vec3 L = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
    vec3 H = normalize(L + V);

    vec3 ambient = 0.2f * texture(diffuseMap_0, fs_in.fTexCoord).rgb;
    vec3 diffuse = max(dot(L, N), 0.0f) * texture(diffuseMap_0, fs_in.fTexCoord).rgb;
    vec3 specular = vec3(0.15f) * pow(max(dot(N, H), 0.0), 32.0f);

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
	//fragColor = texture(normalMap_0, fs_in.fTexCoord);
}