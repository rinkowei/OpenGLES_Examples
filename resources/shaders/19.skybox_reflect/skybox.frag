#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fUVW;

uniform samplerCube cubemap;

void main()
{
    fragColor = texture(cubemap, fUVW);
}