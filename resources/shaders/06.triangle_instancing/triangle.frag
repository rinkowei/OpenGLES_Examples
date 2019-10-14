#version 310 es
precision mediump float;
in vec4 fColor;

out vec4 fragColor;

void main()
{
    fragColor = fColor;
}