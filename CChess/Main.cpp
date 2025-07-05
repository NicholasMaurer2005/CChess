#include <iostream>
#include <string_view>
#include <chrono>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces
//TODO: for build -> performace guided optimization

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
constexpr std::string_view debugFen2{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R" };
constexpr std::string_view debugFen3{ "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8" };
constexpr std::string_view debugFen4{ "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1" };
constexpr std::string_view debugFen5{ "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R" };
constexpr std::string_view debugFen6{ "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1" };

int main()
{
	Engine engine{ startFen, Castle::All };

	const auto start{ std::chrono::high_resolution_clock::now() };

	engine.perft(6);

	const auto end{ std::chrono::high_resolution_clock::now() };
	const std::chrono::duration<double> elapsed{ end - start };
	std::cout << elapsed.count() << std::endl;

	//engine.printMoves(true, 2);
}
