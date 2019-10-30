#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

vec4 float_to_rgba( float v )
{
	vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
	enc      = fract(enc);
	enc     -= enc.yzww * vec4(1.0/255.0,1.0/255.0,1.0/255.0,0.0);
	return enc;
}

void main()
{
	fragColor = float_to_rgba(gl_FragCoord.z);
}