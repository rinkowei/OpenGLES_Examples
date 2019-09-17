#version 320 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in VS_OUT
{
	vec2 fTexcoord;
	vec3 fNormal;
	vec3 fFragPos;
	vec4 fFragPosLightSpace;
}fs_in;

uniform vec3 lightPos;
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

float random(vec3 seed, int i)
{
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float calculateShadow(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	vec3 normal = normalize(fs_in.fNormal);
	vec3 lightDir = normalize(lightPos - fs_in.fFragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0f;
	ivec2 texelSize = textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) / vec2(texelSize.x, texelSize.y)).r; 
            shadow += currentDepth > pcfDepth ? 1.0f : 0.0f;        
        }    
    }

    shadow /= 9.0f;

	if (projCoords.z > 1.0f)
	{
		shadow = 0.0f;
	}

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

	//float shadow = calculateShadow(fs_in.fFragPosLightSpace);
	float shadow = 1.0f;
	float bias = 0.005f * tan(acos(clamp(dot(fs_in.fNormal, lightDir), 0.0f, 1.0f)));
	bias = clamp(bias, 0.0f, 0.01f);

	for (int i=0;i<4;i++){
		// use either :
		//  - Always the same samples.
		//    Gives a fixed pattern in the shadow, but no noise
		int index = i;
		//  - A random sample, based on the pixel's screen location. 
		//    No banding, but the shadow moves with the camera, which looks weird.
		// int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
		//  - A random sample, based on the pixel's position in world space.
		//    The position is rounded to the millimeter to avoid too much aliasing
		// int index = int(16.0f*random(floor(fs_in.fFragPos.xyz*1000.0), i))%16;
		
		// being fully in the shadow will eat up 4*0.2 = 0.8
		// 0.2 potentially remain, which is quite dark.
		shadow -= 0.7f * (1.0 - texture( depthMap, vec3(fs_in.fFragPosLightSpace.xy + poissonDisk[index] / 700.0, (fs_in.fFragPosLightSpace.z-bias) / fs_in.fFragPosLightSpace.w).xy).r);
	}

	shadow = clamp(shadow, 0.1f, 1.0f);
    fragColor = vec4(ambient + shadow * (diffuse + specular), 1.0f);
}