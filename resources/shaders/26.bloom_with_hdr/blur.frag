#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D image;

uniform float blurScale;
uniform float blurStrength;
uniform int horizontal;

void main()
{
	 float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
	 ivec2 texSize = textureSize(image, 0); // gets size of single texel
	 vec2 texOffset = vec2(1.0f / float(texSize.x), 1.0f / float(texSize.y)) * blurScale;
     vec3 result = texture(image, fTexcoord).rgb * weight[0];
     if (horizontal == 1)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(image, fTexcoord + vec2(texOffset.x * float(i), 0.0)).rgb * weight[i] * blurStrength;
            result += texture(image, fTexcoord - vec2(texOffset.x * float(i), 0.0)).rgb * weight[i] * blurStrength;
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(image, fTexcoord + vec2(0.0, texOffset.y * float(i))).rgb * weight[i] * blurStrength;
             result += texture(image, fTexcoord - vec2(0.0, texOffset.y * float(i))).rgb * weight[i] * blurStrength;
         }
     }

	 fragColor = vec4(result, 1.0f);
}