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
#include "MenuManager.h"



class Window
{
private:

	// Private Definitions

	//constants
	static constexpr int minimumSettingsWidth{ 300 };
	static constexpr int minimumWindowWidth{ 500 + minimumSettingsWidth };
	static constexpr int minimumWindowHeight{ 500 };



	//usings
	using clock = std::chrono::high_resolution_clock;
	using PieceCallback = std::function<PieceSprite::Piece(int)>;
	using MoveCallback = std::function<void(int, int)>;


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
	
	//piece drag
	bool m_dragging{};
	GLint m_uMousePosition{};
	int m_dragStart{};

	//callbacks
	PieceCallback m_pieceCallback;
	MoveCallback m_moveCallback;


	MenuManager* m_menuManager;


	
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
	Window(MenuManager& menuManager, PieceCallback pieceCallback, MoveCallback moveCallback);



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

