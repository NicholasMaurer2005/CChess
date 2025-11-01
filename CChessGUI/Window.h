#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>
#include <array>


//constants
constexpr int boardSize{ 64 };

using MoveCallback = void(*)(int source, int destination) noexcept;



class Window
{

//private properties
private:

	//window
	GLFWwindow* m_window;
	int m_width;
	int m_height;
	std::array<char, boardSize> m_position;

	//board
	GLuint m_boardShader;
	GLuint m_boardTexture;
	GLuint m_boardBuffer;
	GLuint m_boardVAO;

	//pieces
	GLuint m_piecesShader;
	GLuint m_piecesTexture;
	GLuint m_piecesBuffer;
	GLuint m_piecesVAO;
	GLuint m_piecesEBO;

	GLsizei m_piecesBufferCount;
	int m_maxPiecesBufferSize;
	
	//move
	MoveCallback m_moveCallback;
	GLuint m_dragShader;
	GLuint m_dragBuffer;
	GLuint m_dragVAO;

	bool m_dragging;
	GLuint m_uDragX;
	GLuint m_uDragY;



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



	//buffer drag
	void bufferDragPiece() noexcept;



//public methods
public:



	//constructors
	Window(int width, int height) noexcept;

	~Window() noexcept;



	//window
	bool open() noexcept;

	void draw() noexcept;

	void resize(int width, int height) noexcept;

	void startDragging() noexcept;

	void stopDragging() noexcept;



	//getters
	int width() const noexcept;

	int height() const noexcept;



	//setters
	void setWindowUser(void* user) noexcept;

	void setMoveCallback(MoveCallback callback) noexcept;

	void setWidth(int width) noexcept;

	void setHeight(int height) noexcept;



	//buffer
	void bufferBoard(bool flipped, int source, int destination) const noexcept;

	void bufferPieces(bool flipped, std::string_view board) noexcept;
};

