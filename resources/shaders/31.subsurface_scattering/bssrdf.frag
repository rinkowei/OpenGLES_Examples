#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec3 fFragPos;
in vec3 fNormal;
in vec3 camDir;
in vec3 camNormal;
in vec4 lightSpaceFragPos;
in float lightDist;

//uniform vec3 lightDir;
uniform sampler2D depthMap;

void main()
{
	float bias = 0.0005f;

	float hitShadowDist = texture(depthMap, lightSpaceFragPos.xy).r;
	float scatDistance = lightDist - hitShadowDist;
	float scatPower = exp(-scatDistance * 10.0f);

	fragColor = vec4(scatPower);
}