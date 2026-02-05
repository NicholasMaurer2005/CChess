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

static consteval BoardTexture generateBoardTexture(bool flipped)
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
				board[index] = flipped ? darkSquare : lightSquare;
			}
			else
			{
				board[index] = flipped ? lightSquare : darkSquare;
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

	ImGui::SetNextWindowPos(ImVec2(m_height, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(m_width - m_height, m_height));
	ImGui::Begin("settings", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar);

	/*if (ImGui::Button("Back"))
	{
		m_moveBackCallback();
	}
	ImGui::SameLine();
	if (ImGui::Button("Forward"))
	{
		m_moveForwardCallback();
	}*/

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
		m_dragShader.uniformVec2(m_uMousePosition, x, y);

		m_dragShader.use();
		m_piecesTexture.bind();
		m_dragBuffer.draw();
	}
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
Window::Window(MoveCallback moveCallback, PieceCallback pieceCallback)
	: m_moveCallback(std::move(moveCallback)), m_pieceCallback(std::move(pieceCallback))
{
	initGLFW();

	//must be initialized after glewInit() returns GLEW_OK
	m_viewportBuffer = Buffer::square(2.0f);
	m_positionBuffer.initialize();
	m_dragBuffer = Buffer::square(0.25f);
	m_boardTexture = Texture(generateBoardTexture(false), fileSize, rankSize);
	m_piecesTexture = Texture("pieceTextures.png");
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
	drawPieces();
	drawImGui();
	glfwSwapBuffers(m_window);
}

static BoardTexture whiteBoard{ generateBoardTexture(false) };
static BoardTexture blackBoard{ generateBoardTexture(true) };
void Window::bufferBoard(bool flipped, int source, int destination) const noexcept
{
	const std::span<Texture::Pixel> board{ flipped ? blackBoard : whiteBoard };

	if (source != 64 && destination != 64)
	{
		static constexpr Texture::Pixel destinationColor{ 137, 207, 240, 255 };
		static constexpr Texture::Pixel sourceColor{ 115, 157, 179, 255 };

		Texture::Pixel oldSource{ std::exchange(board[source], sourceColor) };
		Texture::Pixel oldDestination{ std::exchange(board[destination], destinationColor) };

		m_boardTexture.update(board);

		board[source] = oldSource;
		board[destination] = oldDestination;
	}
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
		const std::size_t file{ static_cast<std::size_t>(x / m_height * 8) };
		const std::size_t rank{ static_cast<std::size_t>(8.0f - y / m_height * 8.0f) };
		const std::size_t pieceIndex{ rank * fileSize + file };
		const PieceSprite::Piece piece{ m_pieceCallback(pieceIndex) };

		if (piece != PieceSprite::Piece::NoPiece)
		{
			m_dragStart = static_cast<int>(pieceIndex);
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

		if (m_moveCallback(m_dragStart, square))
		{
			bufferBoard(false, m_dragStart, square);
			glfwSwapBuffers(m_window);
		}
	}
	else
	{
		m_moveCallback(m_dragStart, m_dragStart);
	}
}