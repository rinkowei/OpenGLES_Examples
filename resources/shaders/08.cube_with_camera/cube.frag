#version 320 es
precision mediump float;
out vec4 fragColor;

in vec2 fTexCoord;

uniform sampler2D diffuseMap_0;

void main()
{
    fragColor = mix(texture(diffuseMap_0, fTexCoord), vec4(0.7f, 0.5f, 0.6f, 1.0f), 0.3f);
}