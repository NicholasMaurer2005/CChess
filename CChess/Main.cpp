#include <iostream>
#include <string_view>
#include <chrono>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces
//TODO: for build -> performace guided optimization

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };

int main()
{
	Engine engine{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", Castle::All};

	engine.perft(6);
	//engine.play();
}
