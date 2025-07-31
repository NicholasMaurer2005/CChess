#include "Window.h"

#include "boardVertex.hpp"
#include "boardFragment.hpp"
#include "pieceVertex.hpp"
#include "pieceFragment.hpp"

#include <iostream>
#include <array>
#include <chrono>
#include <format>
#include <string>


#define STB_IMAGE_IMPLEMENTATION
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
				board[index] = lightSquare;
			}
			else
			{
				board[index] = darkSquare;
			}
		}
	}

	return board;
}

static consteval std::array<Piece, 256> generateCharToPiece()
{
	std::array<Piece, 256> array{};
	array.fill(Piece::NoPiece);

	array['P'] = Piece::WhitePawn;
	array['N'] = Piece::WhiteKnight;
	array['B'] = Piece::WhiteBishop;
	array['R'] = Piece::WhiteRook;
	array['Q'] = Piece::WhiteQueen;
	array['K'] = Piece::WhiteKing;
	array['p'] = Piece::BlackPawn;
	array['n'] = Piece::BlackKnight;
	array['b'] = Piece::BlackBishop;
	array['r'] = Piece::BlackRook;
	array['q'] = Piece::BlackQueen;
	array['k'] = Piece::BlackKing;

	return array;
}

static consteval std::array<GLuint, 192> generatePieceIndexBuffer()
{
	std::array<GLuint, 192> array{};
	std::size_t back{};

	for (GLuint i{}; i < 32; ++i)
	{
		const GLuint base{ i * 4 };

		array[back] = base;
		++back;

		array[back] = base + 1;
		++back;

		array[back] = base + 2;
		++back;

		array[back] = base + 0;
		++back;

		array[back] = base + 2;
		++back;

		array[back] = base + 3;
		++back;
	}

	return array;
}

static GLuint generateShaderProgram(std::string_view vertexSource, std::string_view fragmentSource) noexcept
{
	//vertex
	GLuint vertex{ glCreateShader(GL_VERTEX_SHADER) };
	const GLchar* vertexData{ vertexSource.data() };
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
	const GLchar* fragmentData{ fragmentSource.data() };
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

	glDeleteShader(vertex);
	glDeleteShader(fragment);

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
	m_window = glfwCreateWindow(m_width, m_height, "proof of concept", NULL, NULL);

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

void Window::initBoardShader() noexcept
{
	m_boardShader = generateShaderProgram(ctl::boardVertex, ctl::boardFragment);
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

	glGenVertexArrays(1, &m_boardVAO);
	glBindVertexArray(m_boardVAO);
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

void Window::initPieceShader() noexcept
{
	m_pieceShader = generateShaderProgram(ctl::pieceVertex, ctl::pieceFragment);
}

void Window::initPieceBuffer() noexcept
{
	glGenBuffers(1, &m_pieceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_pieceBuffer);

	glGenVertexArrays(1, &m_pieceVAO);
	glBindVertexArray(m_pieceVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_pieceEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pieceEBO);
}

void Window::initPieceTexture() noexcept
{
	int width{};
	int height{};
	int channels{};

	unsigned char* data{ stbi_load("pieceTextures.png", &width, &height, &channels, STBI_rgb_alpha) };
	const GLint format{ channels == 4 ? GL_RGBA : GL_RGB };

	glGenTextures(1, &m_pieceTexture);
	glBindTexture(GL_TEXTURE_2D, m_pieceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

//public methods
Window::Window(int width, int height) noexcept
	//window
	: m_window(), m_width(width), m_height(height),

	//board
	m_boardShader(), m_boardTexture(), m_boardBuffer(), m_boardVAO(),

	//pieces
	m_pieceShader(), m_pieceBuffer(), m_pieceTexture(), m_pieceVAO(), m_pieceEBO(), m_pieceBufferCount(), m_maxPieceBufferSize()
{
	initGLFW();

	initBoardShader();
	initBoardBuffer();
	initBoardTexture();

	initPieceShader();
	initPieceBuffer();
	initPieceTexture();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window() noexcept
{
	glDeleteProgram(m_boardShader);
	glDeleteBuffers(1, &m_boardBuffer);
	glDeleteTextures(1, &m_boardTexture);

	glfwTerminate();
}

bool Window::open() noexcept
{
	return !glfwWindowShouldClose(m_window);
}

void Window::draw() noexcept
{
	static std::chrono::steady_clock::time_point lastTime{};
	const auto currentTime{ std::chrono::high_resolution_clock::now() };
	const std::chrono::duration<double> elapsed{ currentTime - lastTime };
	lastTime = currentTime;

	const std::string title{ std::format("CChess - fps: {}", 1.0f / elapsed.count()) };
	glfwSetWindowTitle(m_window, title.data());

	glClear(GL_COLOR_BUFFER_BIT);

	//draw board
	glUseProgram(m_boardShader);
	glBindTexture(GL_TEXTURE_2D, m_boardTexture);
	glBindVertexArray(m_boardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//draw pieces
	glUseProgram(m_pieceShader);
	glBindTexture(GL_TEXTURE_2D, m_pieceTexture);
	glBindVertexArray(m_pieceVAO);
	glDrawElements(GL_TRIANGLES, 6 * m_pieceBufferCount, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);

	glfwPollEvents();
}

void Window::buffer(std::string_view board) noexcept
{
	//TODO: remove magic numbers
	static constexpr std::array<Piece, 256> charToPiece{ generateCharToPiece() };
	static constexpr std::array<GLuint, 192> indexBuffer{ generatePieceIndexBuffer() };

	std::vector<PieceSprite> boardData;
	boardData.reserve(32);

	for (int rank{}; rank < 8; ++rank)
	{
		for (int file{}; file < 8; ++file)
		{
			const Piece piece{ charToPiece[board[rank * 8 + file]] };

			if (piece != Piece::NoPiece)
			{
				boardData.emplace_back(rank, file, piece);
			}
		}
	}

	

	m_pieceBufferCount = boardData.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_pieceBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pieceEBO);

	if (boardData.size() <= m_maxPieceBufferSize)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, boardData.size() * sizeof(PieceSprite), boardData.data());
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, boardData.size() * 6 * sizeof(GLuint), indexBuffer.data());
	}
	else
	{
		m_maxPieceBufferSize = boardData.size();
		glBufferData(GL_ARRAY_BUFFER, boardData.size() * sizeof(PieceSprite), boardData.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, boardData.size() * 6 * sizeof(GLuint), indexBuffer.data(), GL_STATIC_DRAW);
	}
}