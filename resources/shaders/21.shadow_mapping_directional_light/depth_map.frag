#version 320 es
precision mediump float;

void main()
{
	gl_FragDepth = gl_FragCoord.z;
}