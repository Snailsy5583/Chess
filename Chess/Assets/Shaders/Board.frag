#version 330 core

uniform bool isWhite;

void main()
{
	if (isWhite)
		gl_FragColor = vec4(234,233,210,255);
	else
		gl_FragColor = vec4(75,115,153,255);

	gl_FragColor /= 255.f;
}