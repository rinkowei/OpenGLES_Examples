#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

layout(shared, binding = 0) uniform mixColor
{
	vec4 additionalColor;
	float mixValue;
};

void main()
{
    fragColor = mix(vec4(0.0f, 1.0f, 0.0f, 1.0f), additionalColor, mixValue);
}