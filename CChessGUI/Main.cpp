#include <CChess.h>
#include <string>
#include <memory>

#include "Window.h"



int main()
{
	engine_create();
	std::unique_ptr<const char> position{ engine_get_char_position() };
	Window window{ 1200, 1200 };
	window.buffer(position.get());


	while (window.open())
	{
		window.draw();

		int source{};
		int destination{};
	}
}