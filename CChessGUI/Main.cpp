#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main()
{
	//initialize glfw
	if (!glfwInit())
	{
		std::cerr << "failed to initialize glfw" << std::endl;
	}

	//create window
	GLFWwindow* window{ glfwCreateWindow(1280, 720, "proof of concept", NULL, NULL) };

	if (!window)
	{
		std::cerr << "failed to create window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	//init glew
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "failed to initialize glew" << std::endl;
		glfwTerminate();
	}

	//render loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		//draw
		


		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}