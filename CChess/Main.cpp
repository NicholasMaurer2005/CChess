#include <iostream>
#include <string_view>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces
//TODO: for build -> performace guided optimization

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
constexpr std::string_view debugFen{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R" };
constexpr std::string_view debugFen2{ "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8" };

int main()
{
	Engine engine{ debugFen };
	engine.perft(6);
	//engine.printMoves(true);
}
