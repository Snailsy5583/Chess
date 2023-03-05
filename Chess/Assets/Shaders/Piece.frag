#version 330 core

uniform sampler2D inTex;
uniform vec4 tint = vec4(1, 1, 1, 1);

in vec2 TexCoord;

void main()
{
	gl_FragColor = texture(inTex, TexCoord)*tint;
}