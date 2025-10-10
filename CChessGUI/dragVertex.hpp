#pragma once
#include <string_view>
namespace ctl
{
constexpr std::string_view dragVertex{
"#version 330 core\n"
"\n"
"layout (location = 0) in vec2 aPosition;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"\n"
"uniform float dragX;\n"
"uniform float dragY;\n"
"\n"
"out vec2 TexCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPosition + vec2(dragX, dragY), 0.0, 1.0);\n"
"	TexCoord = aTexCoord;\n"
"}\n"
};
}
