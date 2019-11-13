#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fNormal;
in vec3 fFragPos;
in vec4 fFragPosLightSpace;

uniform vec3 lightDir;
uniform vec3 viewPos;

uniform sampler2D depthMap;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

void main()
{
	vec3 color = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fNormal);
	vec3 lightColor = vec3(0.8f);

	// ambient
	vec3 ambient = 0.2f * color;

	// diffuse
	vec3 lightDir = normalize(-lightDir);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;    

	float shadow = 1.0f;
	float bias = 0.005f * tan(acos(clamp(dot(fNormal, lightDir), 0.0f, 1.0f)));
	bias = clamp(bias, 0.0f, 0.0005f);

	for (int i = 0; i < 16; i++)
	{
		if (texture(depthMap, vec2(fFragPosLightSpace.xy + poissonDisk[i] / 700.0f)).r < fFragPosLightSpace.z - bias)
		{
			shadow -= 0.08;
		}
	}
    
    if(fFragPosLightSpace.z > 1.0)
        shadow = 0.0;

	shadow = clamp(shadow, 0.0f, 1.0f);
    fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0f);
}