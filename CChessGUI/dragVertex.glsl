#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 dragPosition;

out vec2 texCoord;



void main()
{
	gl_Position = vec4(aPosition + dragPosition, 0.0, 1.0);
	texCoord = aTexCoord;
}