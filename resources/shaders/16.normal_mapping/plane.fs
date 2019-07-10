#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexCoord;

uniform sampler2D normalMap;

void main()
{
    vec3 normal = texture(normalMap, fTexCoord).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    fragColor = vec4(normal, 1.0f);
}