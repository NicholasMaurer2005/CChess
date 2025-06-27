#include <iostream>


#include "Engine.h"

//TODO: renaming and namespaces

int main()
{
	Engine engine{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
	engine.perft(4);
}