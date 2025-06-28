#include <iostream>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces

int main()
{
	Engine engine{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R" };
	//engine.perft(1);
	engine.printMoves(true);

	/*static PreGen gen;
	State state{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
	gen.rookAttack(24, state.occupancy()).print();*/
}



