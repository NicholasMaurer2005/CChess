#include "Window.h"

#include "vertex.hpp"
#include "fragment.hpp"

#include <iostream>
#include <stb_image.h>

struct alignas(4) Pixel
{
	std::uint8_t r, g, b, a;
};



//static helpers
static consteval std::array<Pixel, 64> generateBoardTexture()
{
	std::array<Pixel, 64> board{};

	constexpr Pixel lightSquare{ 235, 236, 207, 255 };
	constexpr Pixel darkSquare{ 120, 149, 78, 255 };

	for (std::size_t rank{}; rank < 8; ++rank)
	{
		for (std::size_t file{}; file < 8; ++file)
		{
			const std::size_t index{ rank * 8 + file };
			const std::size_t square{ rank + file };

			if (square % 2)
			{
				board[index] = darkSquare;
			}
			else
			{
				board[index] = lightSquare;
			}
		}
	}

	return board;
}

static GLuint generateShaderProgram() noexcept
{
	//vertex
	GLuint vertex{ glCreateShader(GL_VERTEX_SHADER) };
	const GLchar* vertexData{ ctl::vertex.data() };
	glShaderSource(vertex, 1, &vertexData, NULL);
	glCompileShader(vertex);

	GLint vertexCompileStatus{};
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &vertexCompileStatus);

	if (vertexCompileStatus != GL_TRUE)
	{
		std::cerr << "unable to compile vertex shader" << std::endl;
		return 0;
	}

	//fragment
	GLuint fragment{ glCreateShader(GL_FRAGMENT_SHADER) };
	const GLchar* fragmentData{ ctl::fragment.data() };
	glShaderSource(fragment, 1, &fragmentData, NULL);
	glCompileShader(fragment);

	GLint fragmentCompileStatus{};
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &fragmentCompileStatus);

	if (fragmentCompileStatus != GL_TRUE)
	{
		std::cerr << "unable to compile fragment shader" << std::endl;
		return 0;
	}

	//shader
	GLuint shader{ glCreateProgram() };
	glAttachShader(shader, vertex);
	glAttachShader(shader, fragment);
	glLinkProgram(shader);

	return shader;
}



//private methods
void Window::initGLFW() noexcept
{
	//initialize glfw
	if (!glfwInit())
	{
		std::cerr << "failed to initialize glfw" << std::endl;
	}

	//create window
	m_window = glfwCreateWindow(1280, 720, "proof of concept", NULL, NULL);

	if (!m_window)
	{
		std::cerr << "failed to create window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(m_window);

	//init glew
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "failed to initialize glew" << std::endl;
		glfwTerminate();
	}
}

void Window::initShader() noexcept
{
	m_shader = generateShaderProgram();
	glUseProgram(m_shader);
}

void Window::initBoardBuffer() noexcept
{
	static constexpr float boardBufferData[]{
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};

	glGenBuffers(1, &m_boardBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_boardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boardBufferData), &boardBufferData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Window::initBoardTexture() noexcept
{
	static constexpr std::array <Pixel, 64> boardTexture{ generateBoardTexture() };

	glGenTextures(1, &m_boardTexture);
	glBindTexture(GL_TEXTURE_2D, m_boardTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, boardTexture.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Window::initPieceTextures() noexcept
{

}



//public methods
Window::Window() noexcept
	: m_window(), m_shader(), m_boardTexture(), m_boardBuffer()
{
	initGLFW();

	initShader();

	initBoardBuffer();

	initBoardTexture();

	initPieceTextures();
}

Window::~Window() noexcept
{
	glfwTerminate();
}

bool Window::open() noexcept
{
	return !glfwWindowShouldClose(m_window);
}

void Window::draw() noexcept
{
	glClear(GL_COLOR_BUFFER_BIT);

	//draw
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glfwSwapBuffers(m_window);

	glfwPollEvents();
}