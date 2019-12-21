#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D inputImage;

void main()
{
    float Pi = 6.28318530718; // Pi*2
    
    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 8.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}
    ivec2 texelSize = textureSize(inputImage, 0);
    vec2 Radius = Size / vec2(texelSize);
    
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fTexcoord;

    int count = 0;
	int kernel = 2;
    float xoffset = 1.0 / float(texelSize.x);
	float yoffset = 1.0 / float(texelSize.y);
    vec4 color = vec4(0.0);
	for (int x = -kernel; x <= kernel; x++)
	{
		for (int y = -kernel; y <= kernel; y++)
		{
			uv.xy += vec2(xoffset * float(x), yoffset * float(y));
			color += texture(inputImage, uv);
			count++;
		}
	}
	color /= float(count);
    
    fragColor =  color;
}