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

using MoveCallback = std::function<void(int source, int destination)>;


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
	MoveCallback m_moveCallback;

	//board
	GLuint m_boardShader;
	GLuint m_boardTexture;
	GLuint m_boardBuffer;
	GLuint m_boardVAO;
	GLuint m_boardEBO;

	//pieces
	GLuint m_piecesShader;
	GLuint m_piecesTexture;
	GLuint m_piecesBuffer;
	GLuint m_piecesVAO;
	GLuint m_piecesEBO;
	GLsizei m_piecesBufferCount;
	int m_maxPiecesBufferSize;

	//dragging
	GLuint m_dragShader;
	GLuint m_dragBuffer;
	GLuint m_dragVAO;
	GLuint m_dragEBO;
	GLuint m_uDragX;
	GLuint m_uDragY;
	bool m_dragging;
	int m_dragStart;


	
//private methods
private:

	//init GLFW
	void initGLFW() noexcept;



	//init board
	void initBoardShader() noexcept;

	void initBoardBuffer() noexcept;

	void initBoardTexture() noexcept;



	//init pieces
	void initPieceShader() noexcept;

	void initPieceBuffer() noexcept;

	void initPieceTexture() noexcept;



	//init drag
	void initDragShader() noexcept;

	void initDragBuffer() noexcept;



	//init ImGui
	void initImGui() noexcept;



	//buffer drag
	void bufferDragPiece(std::size_t pieceIndex) noexcept;



	//render pipelines
	void drawBoard() const noexcept;

	void drawPieces() const noexcept;

	void drawDrag() const noexcept;

	void drawImGui() const noexcept;



//public methods
public:



	//constructors
	Window(MoveCallback moveCallback) noexcept;

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

