#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 texCoord;

uniform vec2 renderOffset = vec2(0, 0);

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(pos.x+renderOffset.x, pos.y+renderOffset.y, pos.z, 1);
	TexCoord = texCoord;
}