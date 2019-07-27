#version 320 es
precision mediump float;
out vec4 fragColor;

in vec2 fTexCoord;

uniform sampler2D diffuseMap1;

void main()
{
    fragColor = texture(diffuseMap1, fTexCoord);
}