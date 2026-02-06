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
	using MoveCallback = std::function<void(int source, int destination)>;
	using PieceCallback = std::function<PieceSprite::Piece(int square)>;
	using VoidCallback = std::function<void()>;
	using PlayerColorCallback = std::function<void(bool playerIsWhite)>;
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
	Buffer m_viewportBuffer;
	Buffer m_positionBuffer;
	Buffer m_dragBuffer;
	Texture m_boardTexture;
	Texture m_piecesTexture;
	Texture m_rfTexture;
	Shader m_defaultShader;
	Shader m_dragShader;
	
	//callbacks
	MoveCallback m_moveCallback;
	PieceCallback m_pieceCallback;
	VoidCallback m_resetCallback;
	VoidCallback m_moveForwardCallback;
	VoidCallback m_moveBackCallback;
	VoidCallback m_flipCallback;
	VoidCallback m_engineMoveCallback;
	PlayerColorCallback m_playerColorCallback;

	//piece drag
	bool m_dragging{};
	GLint m_uMousePosition{};
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

	void drawRankFile() const noexcept;
	


	//buffer
	void bufferDragPiece(PieceSprite::Piece piece) noexcept;



	//mouse position
	std::pair<float, float> mousePosition() const noexcept;



public:

	//	Public Methods

	//constructors
	Window(
		MoveCallback moveCallback, 
		PieceCallback pieceCallback, 
		VoidCallback resetCallback, 
		VoidCallback moveForwardCallback, 
		VoidCallback moveBackCallback, 
		VoidCallback flipCallback,
		VoidCallback engineMoveCallback,
		PlayerColorCallback playerColorCallback
	);



	//getters
	bool open() const noexcept;



	//setters
	void resize(int width, int height) noexcept;

	void bufferBoard() const noexcept;

	void bufferBoard(int source, int destination) const noexcept;

	void bufferPieces(std::span<const PieceSprite> data) noexcept;



	//window
	void draw() noexcept;

	void startDragging() noexcept;

	void stopDragging() noexcept;
};

