#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fFragPos;
in vec4 fFragPosLightSpace;


uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D depthMap;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float random(vec3 seed, int i)
{
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float rgba_to_float(vec4 rgba)
{
    return dot(rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0));
}

void main()
{
	vec3 color = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fNormal);
	vec3 lightColor = vec3(0.6f);

	// ambient
	vec3 ambient = 0.3f * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - fFragPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;    

	float shadow = 1.0f;
	float bias = 0.005f * tan(acos(clamp(dot(fNormal, lightDir), 0.0f, 1.0f)));
	bias = clamp(bias, 0.0f, 0.01f);

	for (int i = 0; i < 4; i++)
	{
		float depth = rgba_to_float(texture(depthMap, vec2(fFragPosLightSpace.xy + poissonDisk[i] / 700.0f)));
		float closestDepth = fFragPosLightSpace.z * 2.0 - 1.0;
		if (depth < closestDepth - bias)
		{
			shadow -= 0.2f;
		}
	}

	shadow = clamp(shadow, 0.0f, 1.0f);
    fragColor = vec4(ambient + (1.0f - shadow) * (diffuse + specular), 1.0f);
}