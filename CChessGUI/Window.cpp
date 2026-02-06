#include "Window.h"

#include <string>
#include <format>
#include <stdexcept>
#include <chrono>
#include <span>
#include <array>
#include <utility>
#include <algorithm>

#include "PieceSprite.h"
#include "Texture.h"
#include "Buffer.h"
#include "Shader.h"
#include "Image.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "GL/glew.h"



//	Static Helpers

//constants
static constexpr int boardSize{ 64 };
static constexpr int rankSize{ 8 };
static constexpr int fileSize{ 8 };



//usings
using BoardTexture = std::array<Texture::Pixel, boardSize>;
using PieceEbo = std::array<Buffer::Triangle, boardSize * 2>;



//functions
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

	user->resize(width, height);
}

static consteval BoardTexture generateBoardTexture()
{
	std::array<Texture::Pixel, boardSize> board{};

	constexpr Texture::Pixel lightSquare{ 235, 236, 207, 255 };
	constexpr Texture::Pixel darkSquare{ 120, 149, 78, 255 };

	for (std::size_t rank{}; rank < rankSize; ++rank)
	{
		for (std::size_t file{}; file < fileSize; ++file)
		{
			const std::size_t index{ rank * fileSize + file };
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

static consteval PieceEbo generatePieceEBO()
{
	PieceEbo ebo{};
	PieceEbo::iterator back{ ebo.begin() };

	for (GLuint i{}; i < boardSize; ++i)
	{
		const GLuint base{ i * 4 };

		*back = Buffer::Triangle(base + 0, base + 1, base + 2);
		++back;

		*back = Buffer::Triangle(base + 0, base + 2, base + 3);
		++back;
	}

	return ebo;
}



// Private Methods

//init
void Window::initGLFW()
{
	if (!glfwInit()) throw std::runtime_error("unable to initialize GLFW");

	m_window = glfwCreateWindow(m_width, m_height, "initializing", NULL, NULL);

	if (!m_window) throw std::runtime_error("unable to create window");

	glfwMakeContextCurrent(m_window);

	if (glewInit() != GLEW_OK) throw std::runtime_error("unable to initialize GLEW");

	glfwSetWindowUserPointer(m_window, this);
	glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
	glfwSetWindowSizeLimits(m_window, minimumWindowWidth, minimumWindowHeight, GLFW_DONT_CARE, GLFW_DONT_CARE); 
	glfwSetWindowSizeCallback(m_window, windowSizeCallback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, m_height, m_height);
}

void Window::initImGui() noexcept
{
	constexpr ImVec4 windowColor{ 0.552f, 0.369f, 0.259f, 1.0f };

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	ImGui::StyleColorsDark();
	ImGuiStyle& style{ ImGui::GetStyle() };
	ImVec4* colors{ style.Colors };
	colors[ImGuiCol_WindowBg] = windowColor;

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}



//render pipelines //TODO: maybe remove redundant binds and uses?
void Window::drawImGui() const noexcept
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(m_height), 0.0f));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(m_width - m_height), static_cast<float>(m_height)));
	ImGui::Begin("settings", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar);

	if (ImGui::Button("Engine Move"))
	{
		m_engineMoveCallback();
	}

	if (ImGui::Button("Back"))
	{
		m_moveBackCallback();
	}
	ImGui::SameLine();
	if (ImGui::Button("Forward"))
	{
		m_moveForwardCallback();
	}

	if (ImGui::Button("Reset"))
	{
		m_resetCallback();
	}
	ImGui::SameLine();
	if (ImGui::Button("Flip"))
	{
		m_flipCallback();
	}

	static bool TEMPORARYchecked{ true };
	if (ImGui::Checkbox("Player Is White", &TEMPORARYchecked))
	{
		m_playerColorCallback(TEMPORARYchecked);
	}
	ImGui::SameLine();
	static bool TEMPORARYplayItself{ false };
	if (ImGui::Checkbox("Engine Play Itself", &TEMPORARYplayItself))
	{
		m_enginePlayItselfCallback(TEMPORARYplayItself);
	}
	

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::drawBoard() const noexcept
{
	m_defaultShader.use();
	m_boardTexture.bind();
	m_viewportBuffer.draw();
}

void Window::drawPieces() const noexcept
{
	m_defaultShader.use();
	m_piecesTexture.bind();
	m_positionBuffer.draw();
}

void Window::drawDragPiece() const noexcept
{
	if (m_dragging)
	{
		auto [x, y] = mousePosition();
		m_dragShader.uniformVec2(m_uMousePosition, (2.0f * x / m_height) - 1.0f, -((2.0f * y / m_height) - 1.0f));

		m_dragShader.use();
		m_piecesTexture.bind();
		m_dragBuffer.draw();
	}
}

void Window::drawRankFile() const noexcept
{
	m_defaultShader.use();
	m_rfTexture.bind();
	m_viewportBuffer.draw();
}



//buffer
void Window::bufferDragPiece(PieceSprite::Piece piece) noexcept
{
	static constexpr std::array<Buffer::Triangle, 2> indexBuffer{
		Buffer::Triangle(0, 1, 2),
		Buffer::Triangle(0, 2, 3)

	};
	const std::array<PieceSprite, 1> sprite{ piece };
	
	m_dragBuffer.buffer(sprite, indexBuffer);
}



//mouse position
std::pair<float, float> Window::mousePosition() const noexcept
{
	double x{}, y{};
	glfwGetCursorPos(m_window, &x, &y);

	return std::pair(static_cast<float>(x), static_cast<float>(y));
}



// Public Methods

//constructors
Window::Window(
	MoveCallback moveCallback,
	PieceCallback pieceCallback,
	VoidCallback resetCallback,
	VoidCallback moveForwardCallback,
	VoidCallback moveBackCallback,
	VoidCallback flipCallback,
	VoidCallback engineMoveCallback,
	BooleanCallback playerColorCallback,
	BooleanCallback enginePlayItselfCallback
) :
	m_moveCallback(std::move(moveCallback)),
	m_pieceCallback(std::move(pieceCallback)),
	m_resetCallback(resetCallback),
	m_moveForwardCallback(moveForwardCallback),
	m_moveBackCallback(moveBackCallback),
	m_flipCallback(flipCallback),
	m_engineMoveCallback(engineMoveCallback),
	m_playerColorCallback(playerColorCallback),
	m_enginePlayItselfCallback(enginePlayItselfCallback)
{
	initGLFW();

	//must be initialized after glewInit() returns GLEW_OK
	m_viewportBuffer = Buffer::square(2.0f);
	m_positionBuffer.initialize();
	m_dragBuffer = Buffer::square(0.25f);

	m_boardTexture = Texture(generateBoardTexture(), fileSize, rankSize, Texture::MagFilter::Nearest);
	m_piecesTexture = Texture(Image("pieceTextures.png"), Texture::MagFilter::Linear);
	m_rfTexture = Texture(Image("rfTexture.png", true), Texture::MagFilter::Linear);

	m_defaultShader = Shader("DefaultVertex.glsl", "DefaultFragment.glsl");
	m_dragShader = Shader("DragVertex.glsl", "DefaultFragment.glsl");

	m_uMousePosition = m_dragShader.uniformLocation("mousePosition");

	initImGui();
}



//getters
bool Window::open() const noexcept
{
	return !glfwWindowShouldClose(m_window);
}



//setters
void Window::resize(int width, int height) noexcept
{
	static constexpr float minimumAspectRatio{ static_cast<float>(minimumWindowWidth) / minimumWindowHeight };

	m_width = width;
	m_height = height;
	m_aspectRatio = static_cast<float>(width) / height;

	glViewport(0, 0, height, height);

	//one pixel of wiggle room
	const int minimumWidth{ std::max(static_cast<int>(height * minimumAspectRatio), height) + 1 };

	glfwSetWindowSizeLimits(m_window, minimumWidth, minimumWindowHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glClear(GL_COLOR_BUFFER_BIT);
	drawBoard();
	drawRankFile();
	drawPieces();
	drawImGui();
	glfwSwapBuffers(m_window);
}

static BoardTexture boardTexture{ generateBoardTexture() };
void Window::bufferBoard() const noexcept
{
	m_boardTexture.update(boardTexture);
}

void Window::bufferBoard(int source, int destination) const noexcept
{
	static constexpr Texture::Pixel destinationColor{ 137, 207, 240, 255 };
	static constexpr Texture::Pixel sourceColor{ 115, 157, 179, 255 };

	Texture::Pixel oldSource{ std::exchange(boardTexture[source], sourceColor) };
	Texture::Pixel oldDestination{ std::exchange(boardTexture[destination], destinationColor) };

	m_boardTexture.update(boardTexture);

	boardTexture[source] = oldSource;
	boardTexture[destination] = oldDestination;
}

void Window::bufferPieces(std::span<const PieceSprite> data) noexcept
{
	static constexpr PieceEbo pieceEBO{ generatePieceEBO() };

	m_positionBuffer.buffer(data, std::span(pieceEBO.begin(), data.size() * 2));
}



//window 
void Window::draw() noexcept
{
	//render
	glClear(GL_COLOR_BUFFER_BIT);

	drawBoard();
	drawRankFile();
	drawPieces();
	drawDragPiece();
	drawImGui();

	glfwSwapBuffers(m_window);
	glfwPollEvents();

	//fps and title
	const auto currentTime{ std::chrono::high_resolution_clock::now() };
	const std::chrono::duration<double> elapsed{ currentTime - m_lastTime };
	m_lastTime = currentTime;

	const std::string title{ std::format("CChess - fps: {:.3f}", 1.0 / elapsed.count()) };
	glfwSetWindowTitle(m_window, title.data());
}

void Window::startDragging() noexcept  
{
	auto [x, y] = mousePosition();

	//check if mouse is in board space
	if (x < m_height)
	{
		const int file{ static_cast<int>(x / m_height * 8) };
		const int rank{ static_cast<int>(8.0f - y / m_height * 8.0f) };
		const int square{ rank * fileSize + file };
		const PieceSprite::Piece piece{ m_pieceCallback(square) };

		if (piece != PieceSprite::Piece::NoPiece)
		{
			bufferDragPiece(piece);
			m_dragStart = square;
			m_dragging = true;
		}
	}
}

void Window::stopDragging() noexcept
{
	m_dragging = false;

	auto [x, y] = mousePosition();

	if (x < m_height)
	{
		const int file{ static_cast<int>(x / m_height * 8) };
		const int rank{ static_cast<int>(8 - y / m_height * 8) };
		const int square{ rank * fileSize + file };

		m_moveCallback(m_dragStart, square);
	}
	else
	{
		m_moveCallback(m_dragStart, m_dragStart);
	}
}