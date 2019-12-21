#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec4 fPosition;

const vec2 exponents = vec2(80.0, 80.0);

void main()
{
	float depth = fPosition.z / fPosition.w ;
	//depth = depth * 0.5 + 0.5;			//Don't forget to move away from unit cube ([-1,1]) to [0,1] coordinate system
	
	float pos = exp(exponents.x * depth);
	float neg = -exp(-exponents.y * depth);
	
	fragColor = vec4(pos, pos * pos, neg, neg * neg);
}