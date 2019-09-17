#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in VS_OUT
{
	vec2 fTexcoord;
	vec3 fNormal;
	vec3 fFragPos;
}fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float farPlane;

uniform samplerCube depthMap;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float calculateShadow(vec3 fragPos)
{
	vec3 fragToLight = fragPos - lightPos;

	float currentDepth = length(fragToLight);

	float shadow = 0.0f;
	float bias = 0.005f;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0f + (viewDistance / farPlane)) / 200.0f;
	for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0f;
    }

	shadow /= float(samples);

	return shadow;
}

void main()
{
	vec3 color = vec3(0.8f, 0.8f, 0.8f);
	vec3 normal = normalize(fs_in.fNormal);
	vec3 lightColor = vec3(0.5f);

	// ambient
	vec3 ambient = 0.3f * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - fs_in.fFragPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
    vec3 viewDir = normalize(viewPos - fs_in.fFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;    

	float shadow = calculateShadow(fs_in.fFragPos);
    fragColor = vec4(ambient + (1.0f - shadow) * (diffuse + specular), 1.0f);
}