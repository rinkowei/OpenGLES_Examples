#version 310 es
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vOffset;

out vec4 fColor;

void main()
{
	fColor = vColor;
	gl_Position = vec4(vPos.x + vOffset.x, vPos.y + vOffset.y, vPos.z, 1.0f);
}