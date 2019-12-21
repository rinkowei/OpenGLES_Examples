#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D inputImage;

const float sigma   = 20.0;             // Gaussian sigma
const int   support = int(sigma * 3.0); // int(sigma * 3.0) truncation
void main()
{
	vec2 iResolution = vec2(2048.0);
    vec2 loc   = vec2(fTexcoord.x, 1.0 - fTexcoord.y);
    vec2 dir   = vec2( 0.0, 1.0 / iResolution.y ); // horiz=(1.0, 0.0), vert=(0.0, 1.0)
	vec4 acc   = vec4( 0.0 );                      // accumulator
	float norm = 0.0;
	for (int i = -support; i <= support; i++) {
		float coeff = exp(-0.5 * float(i) * float(i) / (sigma * sigma));
		acc += (texture(inputImage, loc + float(i) * dir)) * coeff;
		norm += coeff;
	}
	acc *= 1.0/norm;                               // normalize for unity gain

    // Output to screen
    fragColor = texture(inputImage, fTexcoord);
}