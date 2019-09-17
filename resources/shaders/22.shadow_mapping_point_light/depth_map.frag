#version 320 es
precision mediump float;

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float lightDistance = length(fragPos.xyz - lightPos);

	lightDistance = lightDistance / farPlane;

	gl_FragDepth = lightDistance;
}