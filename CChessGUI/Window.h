#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <array>
#include <cstddef>



class Window
{

//private properties
private:

	GLFWwindow* m_window;
	GLuint m_shader;
	GLuint m_boardTexture;
	GLuint m_boardBuffer;



//private methods
private:

	void initGLFW() noexcept;

	void initShader() noexcept;

	void initBoardBuffer() noexcept;

	void initBoardTexture() noexcept;

	void initPieceTextures() noexcept;
	


//public methods
public:

	Window() noexcept;

	~Window() noexcept;

	bool open() noexcept;

	void draw() noexcept;
};

