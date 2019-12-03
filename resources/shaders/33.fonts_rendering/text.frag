#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 Texcoord;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, Texcoord).r);
    fragColor = vec4(textColor, 1.0) * sampled;
}