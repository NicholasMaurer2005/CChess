#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <array>
#include <cstddef>

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

	//pieces
	GLuint m_pieceShader;
	GLuint m_pieceBuffer;
	GLuint m_pieceTexture;
	std::array<PieceSprite, 32> m_pieces;


//private methods
private:

	void initGLFW() noexcept;

	void initBoardShader() noexcept;

	void initBoardBuffer() noexcept;

	void initBoardTexture() noexcept;

	void initPieceShader() noexcept;

	void initPieceBuffer() noexcept;

	void initPieceTextures() noexcept;
	


//public methods
public:

	Window(int width, int height) noexcept;

	~Window() noexcept;

	bool open() noexcept;

	void draw() noexcept;
};

