#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

uniform vec3 randomColor;

void main()
{
    fragColor = vec4(randomColor, 1.0f);
}