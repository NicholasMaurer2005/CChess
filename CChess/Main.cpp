#include <iostream>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces

int main()
{
	Engine engine{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
	engine.perft(4);

	/*static PreGen gen;
	State state{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
	gen.rookAttack(24, state.occupancy()).print();*/
}