#include "Window.h"

#include "PieceSprite.h"
#include "boardVertex.hpp"
#include "boardFragment.hpp"
#include "pieceVertex.hpp"
#include "pieceFragment.hpp"
#include "dragVertex.hpp"
#include "dragFragment.hpp"

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <format>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"



/* Static Helpers */

//helper structs
struct alignas(4) Pixel
{
	std::uint8_t r, g, b, a;
};

struct Vertex
{
	float x, y, u, v;
};

struct alignas(64) Square
{
	Vertex v1, v2, v3, v4;
};



//constants
static constexpr int rankSize{ 8 };
static constexpr int fileSize{ 8 };
static constexpr int charToPieceTableSize{ 256 };
static constexpr int pieceIndexBufferSize{ 192 };
static constexpr int maxPieceCount{ 32 };
static constexpr std::size_t vertexPositionOffset{ 0 };
static constexpr std::size_t vertexTextureOffset{ 2 * sizeof(float) };



//constexpr generators
template<bool white>
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
				board[index] = white ? lightSquare : darkSquare;
			}
			else
			{
				board[index] = white ? darkSquare : lightSquare;
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



//constexpr helpers
static constexpr Square squareBuffer{
	//bottem left
	-1.0f, -1.0f, 0.0f, 0.0f,
	//bottem right
	1.0f, -1.0f, 1.0f, 0.0f,
	//top right
	1.0f, 1.0f, 1.0f, 1.0f,
	//top left
	-1.0f, 1.0f, 0.0f, 1.0f
};

static constexpr std::array<GLuint, 6> squareEBO{
	0, 1, 2,
	0, 2, 3
};

static constexpr std::array<Piece, charToPieceTableSize> charToPiece{ generateCharToPiece() };



//static functions
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

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) noexcept
{
	Window* user{ reinterpret_cast<Window*>(glfwGetWindowUserPointer(window)) };

	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (action == GLFW_PRESS)
		{
			user->startDragging();
		}
		else if (action == GLFW_RELEASE)
		{
			user->stopDragging();
		}
	}
}

static void windowSizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	Window* user{ reinterpret_cast<Window*>(glfwGetWindowUserPointer(window)) };

	user->setWidth(width);
	user->setHeight(height);
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
	m_window = glfwCreateWindow(m_width, m_height, "initializing", NULL, NULL);

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

	glfwSetWindowUserPointer(m_window, this);
	glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
}



//init board
void Window::initBoardShader() noexcept
{
	m_boardShader = generateShaderProgram(ctl::boardVertex, ctl::boardFragment);
}

void Window::initBoardBuffer() noexcept
{
	glGenBuffers(1, &m_boardBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_boardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), &squareBuffer, GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_boardVAO);
	glBindVertexArray(m_boardVAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(vertexPositionOffset));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(vertexTextureOffset));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glGenBuffers(1, &m_boardEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_boardEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareEBO), &squareEBO, GL_STATIC_DRAW);
}

void Window::initBoardTexture() noexcept
{
	static constexpr std::array<Pixel, boardSize> boardTexture{ generateBoardTexture<true>() };

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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(vertexPositionOffset));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(vertexTextureOffset));
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



//init drag
void Window::initDragShader() noexcept
{
	m_dragShader = generateShaderProgram(ctl::dragVertex, ctl::dragFragment);
	m_uDragX = glGetUniformLocation(m_dragShader, "dragX");
	m_uDragY = glGetUniformLocation(m_dragShader, "dragY");
}

void Window::initDragBuffer() noexcept
{
	glGenBuffers(1, &m_dragBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_dragBuffer);

	glGenVertexArrays(1, &m_dragVAO);
	glBindVertexArray(m_dragVAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(vertexPositionOffset));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(vertexTextureOffset));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_dragEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dragEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareEBO), &squareEBO, GL_STATIC_DRAW);
}



//init ImGui
void Window::initImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}



//buffer drag
void Window::bufferDragPiece(std::size_t pieceIndex) noexcept
{
	const PieceSprite sprite{ charToPiece[m_position[pieceIndex]] };

	m_position[pieceIndex] = ' ';
	bufferPieces(false, m_position);

	glBindBuffer(GL_ARRAY_BUFFER, m_dragBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PieceSprite), &sprite, GL_STATIC_DRAW);
}



//render pipelines
void Window::drawBoard() const noexcept
{
	glUseProgram(m_boardShader);
	glBindTexture(GL_TEXTURE_2D, m_boardTexture);
	glBindVertexArray(m_boardVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Window::drawPieces() const noexcept
{
	glUseProgram(m_piecesShader);
	glBindTexture(GL_TEXTURE_2D, m_piecesTexture);
	glBindVertexArray(m_piecesVAO);
	glDrawElements(GL_TRIANGLES, 6 * m_piecesBufferCount, GL_UNSIGNED_INT, 0);
}

void Window::drawDrag() const noexcept
{
	if (m_dragging)
	{
		glUseProgram(m_dragShader);
		glBindTexture(GL_TEXTURE_2D, m_piecesTexture);
		glBindVertexArray(m_dragVAO);

		double x{};
		double y{};
		glfwGetCursorPos(m_window, &x, &y);

		const float normalizedX{ 2.0f * static_cast<float>(x) / m_width - 1.0f };
		const float normalizedY{ -(2.0f * static_cast<float>(y) / m_height - 1.0f) };

		glUniform1f(m_uDragX, normalizedX);
		glUniform1f(m_uDragY, normalizedY);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

void Window::drawImGui() const noexcept
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("proof of concept");
	ImGui::Text("I am terrible at chess");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



/* Public Methods */

//constructors
Window::Window(int width, int height, MoveCallback moveCallback) noexcept
	//window
	: m_window(), m_width(width), m_height(height), m_position(), m_lastTime(std::chrono::high_resolution_clock::now()), m_moveCallback(moveCallback),
	//board
	m_boardShader(), m_boardTexture(), m_boardBuffer(), m_boardVAO(), m_boardEBO(),
	//pieces
	m_piecesShader(), m_piecesTexture(), m_piecesBuffer(), m_piecesVAO(), m_piecesEBO(), m_piecesBufferCount(), m_maxPiecesBufferSize(),
	//dragging
	m_dragShader(), m_dragBuffer(), m_dragVAO(), m_dragEBO(), m_uDragX(), m_uDragY(), m_dragging(), m_dragStart()
{
	initGLFW();

	initBoardShader();
	initBoardBuffer();
	initBoardTexture();

	initPieceShader();
	initPieceBuffer();
	initPieceTexture();

	initDragShader();
	initDragBuffer();

	initImGui();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window() noexcept
{
	//board cleanup
	glDeleteProgram(m_boardShader);
	glDeleteTextures(1, &m_boardTexture);
	glDeleteBuffers(1, &m_boardBuffer);
	glDeleteVertexArrays(1, &m_boardVAO);
	glDeleteBuffers(1, &m_boardEBO);

	//pieces cleanup
	glDeleteProgram(m_piecesShader);
	glDeleteTextures(1, &m_piecesTexture);
	glDeleteBuffers(1, &m_piecesBuffer);
	glDeleteVertexArrays(1, &m_piecesVAO);
	glDeleteBuffers(1, &m_piecesEBO);

	//dragging cleanup
	glDeleteProgram(m_dragShader);
	glDeleteBuffers(1, &m_dragBuffer);
	glDeleteVertexArrays(1, &m_dragVAO);
	glDeleteBuffers(1, &m_dragEBO);

	//ImGui cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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
	//fps
	const auto currentTime{ std::chrono::high_resolution_clock::now() };
	const std::chrono::duration<double> elapsed{ currentTime - m_lastTime };
	m_lastTime = currentTime;

	const std::string title{ std::format("CChess - fps: {}", 1.0 / elapsed.count()) };
	glfwSetWindowTitle(m_window, title.data());

	//render
	glClear(GL_COLOR_BUFFER_BIT);

	drawBoard();
	drawPieces();
	drawDrag();
	drawImGui();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Window::resize(int width, int height) noexcept
{
	m_width = width;
	m_height = height;

	glViewport(0, 0, width, height);
}

void Window::startDragging() noexcept
{
	double x{};
	double y{};
	glfwGetCursorPos(m_window, &x, &y);

	const std::size_t file{ static_cast<std::size_t>(x / m_width * 8) };
	const std::size_t rank{ static_cast<std::size_t>(8 - y / m_height * 8) };
	const std::size_t pieceIndex{ rank * fileSize + file };

	bufferDragPiece(pieceIndex);
	m_dragStart = static_cast<int>(pieceIndex);
	m_dragging = true;
}

void Window::stopDragging() noexcept
{
	double x{};
	double y{};
	glfwGetCursorPos(m_window, &x, &y);

	const int file{ static_cast<int>(x / m_width * 8) };
	const int rank{ static_cast<int>(8 - y / m_height * 8) };
	const int pieceIndex{ rank * fileSize + file };

	m_dragging = false;
	m_moveCallback(m_dragStart, pieceIndex);
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
void Window::setWidth(int width) noexcept
{
	m_width = width;
}

void Window::setHeight(int height) noexcept
{
	m_height = height;
}



//buffer

//in global space to remove initialization guard
static std::array<Pixel, boardSize> whiteBoard{ generateBoardTexture<true>() };
static std::array<Pixel, boardSize> blackBoard{ generateBoardTexture<false>() };

void Window::bufferBoard(bool flipped, int source, int destination) const noexcept
{
	static constexpr Pixel darkMoveColor{ 137, 207, 240, 255 };
	static constexpr Pixel lightMoveColor{ 115, 157, 179, 255 };

	const bool sourceLight{ (source / rankSize + source % fileSize) % 2 == 0 };
	const bool destinationLight{ (destination / rankSize + destination % fileSize) % 2 == 0 };

	if (flipped)
	{
		const Pixel originalSourceColor{ blackBoard[source] };
		const Pixel originalDestinationColor{ blackBoard[destination] };

		blackBoard[source] = sourceLight ? lightMoveColor : darkMoveColor;
		blackBoard[destination] = destinationLight ? lightMoveColor : darkMoveColor;

		glBindTexture(GL_TEXTURE_2D, m_boardTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fileSize, rankSize, GL_RGBA, GL_UNSIGNED_BYTE, blackBoard.data());

		blackBoard[source] = originalSourceColor;
		blackBoard[destination] = originalDestinationColor;
	}
	else
	{ 
		const Pixel originalSourceColor{ whiteBoard[source] };
		const Pixel originalDestinationColor{ whiteBoard[destination] };

		whiteBoard[source] = sourceLight ? lightMoveColor : darkMoveColor;
		whiteBoard[destination] = destinationLight ? lightMoveColor : darkMoveColor;

		glBindTexture(GL_TEXTURE_2D, m_boardTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fileSize, rankSize, GL_RGBA, GL_UNSIGNED_BYTE, whiteBoard.data());

		whiteBoard[source] = originalSourceColor;
		whiteBoard[destination] = originalDestinationColor;
	}
}

void Window::bufferPieces(bool flipped, std::span<const char> board) noexcept
{
	static constexpr std::array<GLuint, pieceIndexBufferSize> indexBuffer{ generatePieceIndexBuffer() };

	std::ranges::copy(board, m_position.begin());

	std::vector<PieceSprite> boardData;
	boardData.reserve(maxPieceCount);

	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const Piece piece{ charToPiece[board[rank * fileSize + file]] };

			if (piece != Piece::NoPiece)
			{
				boardData.emplace_back(flipped ? 7 - rank : rank, flipped ? 7 - file : file, piece);
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