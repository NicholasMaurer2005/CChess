
#include "Window.h"

int main()
{
	Window window;

	while (window.open())
	{
		window.draw();
	}
}