#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D pieceTexture;

void main()
{
	FragColor = texture(pieceTexture, TexCoord);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}