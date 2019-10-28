#version 310 es
layout(location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fUVW;

void main()
{
	// vertex position as texcoord
	fUVW = vPos;
	fUVW.x *= -1.0f;
	// remove translation from the view matrix
	vec4 pos = projection * mat4(mat3(view)) * vec4(vPos, 1.0f);
	gl_Position = pos.xyww;
}