#include "Shader.h"

#include <fstream>
#include <stdexcept>
#include <format>
#include <string>
#include <utility>
#include <GL/glew.h>



//	Static Helpers
static GLuint createProgram(std::string_view vertex, std::string_view fragment)
{
	GLuint vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
	GLuint fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };

	{
		std::ifstream in{ vertex.data(), std::ios::binary | std::ios::ate };
		if (!in) throw std::runtime_error(std::format("unable to open '{}'", vertex));

		std::streampos size{ in.tellg() };
		in.seekg(0);

		std::string data;
		data.resize(size);
		in.read(data.data(), size);

		const char* ptr{ data.c_str() };
		glShaderSource(vertexShader, 1, &ptr, nullptr);
		glCompileShader(vertexShader);

		GLint value{};
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &value);
		if (value == GL_FALSE) throw std::runtime_error(std::format("unable to compile vertex shader from source '{}'", vertex));
	}

	{
		std::ifstream in{ fragment.data(), std::ios::binary | std::ios::ate };
		if (!in) throw std::runtime_error(std::format("unable to open '{}'", vertex));

		std::streampos size{ in.tellg() };
		in.seekg(0);

		std::string data;
		data.resize(size);
		in.read(data.data(), size);

		const char* ptr{ data.c_str() };
		glShaderSource(fragmentShader, 1, &ptr, nullptr);
		glCompileShader(fragmentShader);

		GLint value{};
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &value);
		if (value == GL_FALSE) throw std::runtime_error(std::format("unable to compile fragment shader from source '{}'", fragment));
	}

	const GLuint program{ glCreateProgram() };
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	GLint value{};
	glGetProgramiv(program, GL_LINK_STATUS, &value);
	if (value == GL_FALSE) throw std::runtime_error("unable to link program");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}



//	Public Methods

//constructors
Shader::Shader(std::string_view vertex, std::string_view fragment)
	: m_program(createProgram(vertex, fragment)) {
}

Shader::Shader(Shader&& other) noexcept
	: m_program(std::exchange(other.m_program, 0)) {
}

Shader& Shader::operator= (Shader&& other) noexcept
{
	glDeleteProgram(m_program);
	m_program = std::exchange(other.m_program, 0);

	return *this;
}

Shader::~Shader()
{
	glDeleteProgram(m_program);
}



//getters
GLint Shader::uniformLocation(std::string_view uniform) const
{
	const GLint location{ glGetUniformLocation(m_program, uniform.data()) };
	if (location == -1) throw std::runtime_error(std::format("could not find uniform: {}", uniform));

	return location;
}



//setters
void Shader::use() const noexcept
{
	glUseProgram(m_program);
}
