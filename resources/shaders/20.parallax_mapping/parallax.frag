#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;
in vec3 fTangentFragPos;
in vec3 fTangentLightPos;
in vec3 fTangentViewPos;

uniform float numLayers;
uniform float heightScale;
uniform float parallaxBias;

uniform sampler2D diffuseMap_0;
uniform sampler2D normalMap_0;

vec2 parallaxMapping(vec2 uv, vec3 viewDir) 
{
	float height = 1.0 - textureLod(normalMap_0, uv, 0.0).a;
	vec2 p = viewDir.xy * (height * (heightScale * 0.5) + parallaxBias) / viewDir.z;
	return uv - p;  
}

vec2 steepParallaxMapping(vec2 uv, vec3 viewDir) 
{
	float layerDepth = 1.0 / numLayers;
	float currLayerDepth = 0.0;
	vec2 deltaUV = viewDir.xy * heightScale / (viewDir.z * numLayers);
	vec2 currUV = uv;
	float height = 1.0 - textureLod(normalMap_0, currUV, 0.0).a;
	for (int i = 0; i < int(numLayers); i++) {
		currLayerDepth += layerDepth;
		currUV -= deltaUV;
		height = 1.0 - textureLod(normalMap_0, currUV, 0.0).a;
		if (height < currLayerDepth) {
			break;
		}
	}
	return currUV;
}

vec2 parallaxOcclusionMapping(vec2 uv, vec3 viewDir) 
{
	float layerDepth = 1.0 / numLayers;
	float currLayerDepth = 0.0;
	vec2 deltaUV = viewDir.xy * heightScale / (viewDir.z * numLayers);
	vec2 currUV = uv;
	float height = 1.0 - textureLod(normalMap_0, currUV, 0.0).a;
	for (int i = 0; i < int(numLayers); i++) {
		currLayerDepth += layerDepth;
		currUV -= deltaUV;
		height = 1.0 - textureLod(normalMap_0, currUV, 0.0).a;
		if (height < currLayerDepth) {
			break;
		}
	}
	vec2 prevUV = currUV + deltaUV;
	float nextDepth = height - currLayerDepth;
	float prevDepth = 1.0 - textureLod(normalMap_0, prevUV, 0.0).a - currLayerDepth + layerDepth;
	return mix(currUV, prevUV, nextDepth / (nextDepth - prevDepth));
}

void main()
{
    vec3 V = normalize(fTangentViewPos - fTangentFragPos);
	vec2 uv = parallaxOcclusionMapping(fTexcoord, V);
	//vec2 uv = fTexcoord;

	// Discard fragments at texture border
	if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
		discard;
	}

    vec3 N = normalize(textureLod(normalMap_0, uv, 0.0).rgb * 2.0f - 1.0f);
    vec3 L = normalize(fTangentLightPos - fTangentFragPos);
    vec3 H = normalize(L + V);

	vec3 color = texture(diffuseMap_0, uv).rgb;
    vec3 ambient = 0.2f * color;
    vec3 diffuse = max(dot(L, N), 0.0f) * color;
    vec3 specular = vec3(0.15f) * pow(max(dot(N, H), 0.0), 32.0f);

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}