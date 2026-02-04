#pragma once

#include <array>
#include <chrono>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <span>
#include <utility>

#include "PieceSprite.h"
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"



class Window
{
private:

	// Private Definitions

	//constants
	static constexpr int minimumSettingsWidth{ 200 };
	static constexpr int minimumWindowWidth{ 500 + minimumSettingsWidth };
	static constexpr int minimumWindowHeight{ 500 };



	//usings
	using MoveCallback = std::function<bool(int source, int destination)>;
	using PieceCallback = std::function<PieceSprite::Piece(std::size_t index)>;
	using clock = std::chrono::high_resolution_clock;



private:

	//	Private Members
	
	//window
	GLFWwindow* m_window{};
	int m_width{ minimumWindowWidth };
	int m_height{ minimumWindowHeight };
	float m_aspectRatio{ static_cast<float>(minimumSettingsWidth) / minimumWindowHeight };
	clock::time_point m_lastTime;

	//pipelines
	Buffer m_viewportBuffer{ Buffer::square(2.0f) };
	Buffer m_positionBuffer;
	Buffer m_dragBuffer{ Buffer::square(0.25f) };
	Texture m_boardTexture;
	Texture m_piecesTexture{ "pieceTextures.png" };
	Shader m_defaultShader{ "DefaultVertex.glsl", "DefaultFragment.glsl" };
	Shader m_dragShader{ "DragVertex.glsl", "DefaultFragment.glsl" };
	
	//callbacks
	MoveCallback m_moveCallback;
	PieceCallback m_pieceCallback;

	//piece drag
	bool m_dragging{};
	GLint m_uMousePosition{ m_dragShader.uniformLocation("mousePosition") };
	int m_dragStart{};


	
private:

	//	Private Methods

	//init
	void initGLFW();

	void initImGui() noexcept;



	//render pipelines
	void drawImGui() const noexcept;

	void drawBoard() const noexcept;

	void drawPieces() const noexcept;

	void drawDragPiece() const noexcept;

	

	//mouse position
	std::pair<float, float> mousePosition() const noexcept;



public:

	//	Public Methods

	//constructors
	Window(MoveCallback moveCallback, PieceCallback pieceCallback);



	//getters
	bool open() const noexcept;



	//setters
	void resize(int width, int height) noexcept;

	void bufferBoard(bool flipped, int source, int destination) const noexcept;

	void bufferPieces(std::span<const PieceSprite> data) noexcept;



	//window
	void draw() noexcept;

	void startDragging() noexcept;

	void stopDragging() noexcept;
};

