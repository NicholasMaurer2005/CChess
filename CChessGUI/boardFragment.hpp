#pragma once
#include <string_view>
namespace ctl
{
constexpr std::string_view boardFragment{
"#version 330 core\n"
"\n"
"in vec2 TexCoord;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D boardTexture;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = texture(boardTexture, TexCoord);\n"
"}\n"
};
}
