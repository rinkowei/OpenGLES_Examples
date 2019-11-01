#version 310 es
precision mediump float;

in vec3 fFragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float lightDistance = length(fFragPos - lightPos);

	lightDistance = lightDistance / farPlane;

	gl_FragDepth = lightDistance;
}