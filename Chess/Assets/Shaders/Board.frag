#version 330 core

uniform vec4 tint = vec4(1,1,1,0);

uniform bool isWhite;

vec4 WHITE = vec4(234,233,210,255);
vec4 BLACK = vec4(75,115,153,255);

void main()
{
	if (isWhite)
		gl_FragColor = vec4(234,233,210,255);
	else
		gl_FragColor = vec4(75,115,153,255);

	gl_FragColor = mix(WHITE, BLACK, vec4(isWhite));

	gl_FragColor /= 255.f;

	gl_FragColor = mix(gl_FragColor, vec4(tint.r, tint.g, tint.b, 1), tint.a);
}