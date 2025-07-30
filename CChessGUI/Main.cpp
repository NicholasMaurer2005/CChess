
#include "Window.h"

int main()
{
	Window window{ 1000, 1000 };

	while (window.open())
	{
		window.draw();
	}
}