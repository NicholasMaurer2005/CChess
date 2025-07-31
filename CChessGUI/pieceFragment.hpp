#pragma once
#include <string_view>
namespace ctl
{
constexpr std::string_view pieceFragment{
"#version 330 core\n"
"\n"
"in vec2 TexCoord;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D pieceTexture;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = texture(pieceTexture, TexCoord);\n"
"	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"	//FragColor = vec4(TexCoord, 0.0, 1.0);\n"
"}\n"
};
}
