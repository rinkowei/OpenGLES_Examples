#version 320 es
precision mediump float;
out vec4 fragColor;

in vec2 fTexCoord;

uniform sampler2D cubeTexture;

void main()
{
    fragColor = mix(texture(cubeTexture, fTexCoord), vec4(0.7f, 0.5f, 0.6f, 1.0f), 0.3f);
}