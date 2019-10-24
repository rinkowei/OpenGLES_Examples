#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D renderTexture;

void main()
{
    fragColor = vec4(texture(renderTexture, fTexcoord).rgb, 1.0f);
}