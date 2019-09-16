#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

void main()
{
	gl_FragDepth = gl_FragCoord.z;
	fragColor = vec4(gl_FragDepth);
}