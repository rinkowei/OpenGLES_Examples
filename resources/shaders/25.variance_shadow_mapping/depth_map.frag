#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec4 fPosition;

void main()
{
	float depth = fPosition.z / fPosition.w ;
	depth = depth * 0.5 + 0.5;			//Don't forget to move away from unit cube ([-1,1]) to [0,1] coordinate system
	
	float moment1 = depth;
	float moment2 = depth * depth;
	
	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25*(dx*dx+dy*dy) ;
	
	fragColor = vec4(moment1, moment2, 0.0, 0.0 );
}