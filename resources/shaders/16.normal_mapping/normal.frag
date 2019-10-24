#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fTangentFragPos;
in vec3 fTangentLightPos;
in vec3 fTangentViewPos;

uniform sampler2D diffuseMap_0;
uniform sampler2D normalMap_0;

void main()
{
    vec3 V = normalize(fTangentViewPos - fTangentFragPos);
    vec3 N = normalize(texture(normalMap_0, fTexcoord).rgb * 2.0f - 1.0f);
    vec3 L = normalize(-fTangentLightPos - fTangentFragPos);
    vec3 H = normalize(L + V);

    vec3 ambient = 0.2f * texture(diffuseMap_0, fTexcoord).rgb;
    vec3 diffuse = max(dot(L, N), 0.0f) * texture(diffuseMap_0, fTexcoord).rgb;
    vec3 specular = vec3(0.15f) * pow(max(dot(N, H), 0.0), 32.0f);

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}