#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string_view>

#include "PieceSprite.h"

class Window
{

//private properties
private:

	//window
	GLFWwindow* m_window;
	int m_width;
	int m_height;

	//board
	GLuint m_boardShader;
	GLuint m_boardTexture;
	GLuint m_boardBuffer;
	GLuint m_boardVAO;

	//pieces
	GLuint m_pieceShader;
	GLuint m_pieceBuffer;
	GLuint m_pieceTexture;
	GLuint m_pieceVAO;
	GLuint m_pieceEBO;
	GLsizei m_pieceBufferCount;
	int m_maxPieceBufferSize;

	//player move
	int m_sourceSquare;
	bool m_hasSource;
	


//private methods
private:

	void initGLFW() noexcept;

	void initBoardShader() noexcept;

	void initBoardBuffer() noexcept;

	void initBoardTexture() noexcept;

	void initPieceShader() noexcept;

	void initPieceBuffer() noexcept;

	void initPieceTexture() noexcept;
	
	void playerMove(int rank, int file) noexcept;

	void drawBoard() noexcept;


//public methods
public:

	Window(int width, int height) noexcept;

	~Window() noexcept;

	bool open() noexcept;

	void draw() noexcept;

	void buffer(std::string_view board) noexcept;

	void handleClick(double mouseX, double mouseY) noexcept;
};

