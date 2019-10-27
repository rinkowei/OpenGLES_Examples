#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

uniform vec3 randomColor;

void main()
{
	vec3 color = vec3(1.0f) - exp(-randomColor * 1.0f);
	//color = pow(color, vec3(1.0f / 2.2f));
    fragColor = vec4(color, 1.0f);
}