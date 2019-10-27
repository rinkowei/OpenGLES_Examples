#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

uniform vec3 randomColor;

void main()
{
    fragColor = vec4(randomColor, 1.0f);
	float brightness = dot(randomColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0f)
		brightColor = vec4(randomColor.rgb, 1.0f);
	else
		brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}