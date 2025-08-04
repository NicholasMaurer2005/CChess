#include "Window.h"

#include "PieceSprite.h"
#include "boardVertex.hpp"
#include "boardFragment.hpp"
#include "pieceVertex.hpp"
#include "pieceFragment.hpp"


#include <cstdint>
#include <array>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <string>
#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



//TODO: maybe move to own header file?
struct alignas(4) Pixel
{
	std::uint8_t r, g, b, a;
};



//constants
constexpr int boardSize{ 64 };
constexpr int rankSize{ 8 };
constexpr int fileSize{ 8 };
constexpr int charToPieceTableSize{ 256 };
constexpr int pieceIndexBufferSize{ 192 };
constexpr int maxPieceCount{ 32 };


/* Static Helpers */

static consteval std::array<Pixel, boardSize> generateBoardTexture()
{
	std::array<Pixel, boardSize> board{};

	constexpr Pixel lightSquare{ 235, 236, 207, 255 };
	constexpr Pixel darkSquare{ 120, 149, 78, 255 };

	for (std::size_t rank{}; rank < rankSize; ++rank)
	{
		for (std::size_t file{}; file < fileSize; ++file)
		{
			const std::size_t index{ rank * fileSize + file };
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

static consteval std::array<Piece, charToPieceTableSize> generateCharToPiece()
{
	std::array<Piece, charToPieceTableSize> array{};
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

static consteval std::array<GLuint, pieceIndexBufferSize> generatePieceIndexBuffer()
{
	std::array<GLuint, pieceIndexBufferSize> array{};
	std::size_t back{};

	for (GLuint i{}; i < maxPieceCount; ++i)
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

static std::array<Pixel, boardSize> boardTextureMove(int source, int destination) noexcept
{
	static constexpr std::array<Pixel, boardSize> defaultBoard{ generateBoardTexture() };
	static constexpr Pixel moveColor{ 31, 102, 255, 255 };

	std::array<Pixel, boardSize> board{ defaultBoard };
	board[source] = moveColor;
	board[destination] = moveColor;

	return board;
}




/* Private Methods */

//init GLFW
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



//init board
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



//init pieces
void Window::initPieceShader() noexcept
{
	m_piecesShader = generateShaderProgram(ctl::pieceVertex, ctl::pieceFragment);
}

void Window::initPieceBuffer() noexcept
{
	glGenBuffers(1, &m_piecesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_piecesBuffer);

	glGenVertexArrays(1, &m_piecesVAO);
	glBindVertexArray(m_piecesVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_piecesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_piecesEBO);
}

void Window::initPieceTexture() noexcept
{
	int width{};
	int height{};
	int channels{};

	unsigned char* data{ stbi_load("pieceTextures.png", &width, &height, &channels, STBI_rgb_alpha)};

	const GLint format{ channels == 4 ? GL_RGBA : GL_RGB };

	glGenTextures(1, &m_piecesTexture);
	glBindTexture(GL_TEXTURE_2D, m_piecesTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}




/* Public Methods */

//constructors
Window::Window(int width, int height) noexcept
	//window
	: m_window(), m_width(width), m_height(height), m_flipped(),
	//board
	m_boardShader(), m_boardTexture(), m_boardBuffer(), m_boardVAO(),
	//pieces
	m_piecesShader(), m_piecesBuffer(), m_piecesTexture(), m_piecesVAO(), m_piecesEBO(), m_piecesBufferCount(), m_maxPiecesBufferSize()
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

	engine_set_search_milliseconds(500);

	if (!m_flipped)
	{
		engine_search_and_move();
		drawPieces();
		updateBoardTexture();
	}
}

Window::~Window() noexcept
{
	//board cleanup
	glDeleteProgram(m_boardShader);
	glDeleteTextures(1, &m_boardTexture);
	glDeleteBuffers(1, &m_boardBuffer);
	glDeleteVertexArrays(1, &m_boardVAO);

	//pieces cleanup
	glDeleteProgram(m_piecesShader);
	glDeleteTextures(1, &m_piecesTexture);
	glDeleteBuffers(1, &m_piecesBuffer);
	glDeleteVertexArrays(1, &m_piecesVAO);
	glDeleteBuffers(1, &m_piecesEBO);

	//GLFW cleanup
	glfwDestroyWindow(m_window);
	glfwTerminate();
}



//window
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
	glUseProgram(m_piecesShader);
	glBindTexture(GL_TEXTURE_2D, m_piecesTexture);
	glBindVertexArray(m_piecesVAO);
	glDrawElements(GL_TRIANGLES, 6 * m_piecesBufferCount, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Window::resize(int width, int height) noexcept
{
	m_width = width;
	m_height = height;

	glViewport(0, 0, width, height);
}



//getters
int Window::width() const noexcept
{
	return m_width;
}

int Window::height() const noexcept
{
	return m_height;
}



//setters
void Window::setWindowUser(void* user) noexcept
{
	glfwSetWindowUserPointer(m_window, user);
}

void  Window::setMouseButtonCallback(GLFWmousebuttonfun callback) noexcept
{
	glfwSetMouseButtonCallback(m_window, callback);
}

void  Window::setWindowSizeCallback(GLFWwindowsizefun callback) noexcept
{
	glfwSetWindowSizeCallback(m_window, callback);
}



//buffer
void Window::bufferBoard(bool flipped) const noexcept
{
	//TODO: impliment
}

void Window::bufferPieces(std::string_view board) noexcept
{
	static constexpr std::array<Piece, charToPieceTableSize> charToPiece{ generateCharToPiece() };
	static constexpr std::array<GLuint, pieceIndexBufferSize> indexBuffer{ generatePieceIndexBuffer() };

	std::vector<PieceSprite> boardData;
	boardData.reserve(maxPieceCount);

	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const Piece piece{ charToPiece[board[rank * fileSize + file]] };

			if (piece != Piece::NoPiece)
			{
				boardData.emplace_back(m_flipped ? 7 - rank : rank, m_flipped ? 7 - file : file, piece);
			}
		}
	}

	m_piecesBufferCount = boardData.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_piecesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_piecesEBO);

	//TODO: remove magic numbers
	if (boardData.size() <= m_maxPiecesBufferSize)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, boardData.size() * sizeof(PieceSprite), boardData.data());
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, boardData.size() * 6 * sizeof(GLuint), indexBuffer.data());
	}
	else
	{
		m_maxPiecesBufferSize = boardData.size();
		glBufferData(GL_ARRAY_BUFFER, boardData.size() * sizeof(PieceSprite), boardData.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, boardData.size() * 6 * sizeof(GLuint), indexBuffer.data(), GL_STATIC_DRAW);
	}
}