#include <CChess.h>
#include <string>

#include "Window.h"



int main()
{
	engine_create();
	const char* position{ engine_get_char_position() };
	Window window{ 1920, 1080 };
	window.buffer(position);
	delete[] position;


	while (window.open())
	{
		window.draw();

		int source{};
		int destination{};
		engine_search(&source, &destination);
		engine_move(source, destination);

		const char* position{ engine_get_char_position() };
		window.buffer(position);
		delete[] position;
	}
}