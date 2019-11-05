#version 310 es
layout(location = 0) in vec3 vPos;

out vec3 fFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fFragPos = vPos;
	vec4 clipPos = projection * mat4(mat3(view)) * vec4(vPos, 1.0f);
	gl_Position = clipPos.xyww;
}