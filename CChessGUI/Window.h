#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>



class Window
{

//private properties
private:

	//window
	GLFWwindow* m_window;
	int m_width;
	int m_height;
	bool m_flipped;

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



//public methods
public:

	//constructors
	Window(int width, int height) noexcept;

	~Window() noexcept;



	//window
	bool open() noexcept;

	void draw() noexcept;

	void resize(int width, int height) noexcept;



	//getters
	int width() const noexcept;

	int height() const noexcept;



	//setters
	void setWindowUser(void* user) noexcept;

	void setMouseButtonCallback(GLFWmousebuttonfun callback) noexcept;

	void setWindowSizeCallback(GLFWwindowsizefun callback) noexcept;



	//buffer
	void bufferBoard(bool flipped) const noexcept;

	void bufferPieces(std::string_view board) noexcept;
};

