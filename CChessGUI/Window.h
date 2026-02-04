#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>
#include <array>
#include <chrono>
#include <span>
#include <functional>



//constants
constexpr int boardSize{ 64 };

using MoveCallback = std::function<bool(int source, int destination)>;



class Window
{

//private properties
private:

	//window
	GLFWwindow* m_window;
	int m_width;
	int m_height;
	float m_aspectRatio;
	alignas(64) std::array<char, boardSize> m_position;
	std::chrono::high_resolution_clock::time_point m_lastTime;


	
//private methods
private:

	//init GLFW
	void initGLFW() noexcept;



	//init ImGui
	void initImGui() noexcept;



	//buffer drag
	bool bufferDragPiece(std::size_t pieceIndex) noexcept;



	//render pipelines
	void drawImGui() const noexcept;



//public methods
public:



	//constructors
	Window(MoveCallback moveCallback, std::function<void()> moveBackCallback, std::function<void()> moveForwardCallback) noexcept;

	~Window() noexcept;



	//window
	bool open() noexcept;

	void draw() noexcept;

	void resize(int width, int height) noexcept;

	void startDragging() noexcept;

	void stopDragging() noexcept;



	//buffer
	void bufferBoard(bool flipped, int source, int destination) const noexcept;

	void bufferPieces(bool flipped, std::span<const char> board) noexcept;
};

