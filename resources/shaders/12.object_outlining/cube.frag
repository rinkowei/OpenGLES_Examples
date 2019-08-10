#version 320 es
precision mediump float;
out vec4 fragColor;

in vec2 fTexCoord;

uniform sampler2D cubeTexture;

void main()
{
    fragColor = texture(cubeTexture, fTexCoord);
}