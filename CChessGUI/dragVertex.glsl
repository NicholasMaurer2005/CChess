#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform float dragX;
uniform float dragY;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPosition + vec2(dragX, dragY), 0.0, 1.0);
	TexCoord = aTexCoord;
}