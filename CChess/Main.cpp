#include <iostream>
#include <string_view>


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
	Engine engine{ debugFen5, Castle::WhiteBoth };
	//engine.perft(6);
	engine.printMoves(true, 2);
}



//stateCopy.whiteSquares().print();
//m_state.whiteSquares().print();
//std::cout << "\n\n\n";
//
//stateCopy.blackSquares().print();
//m_state.blackSquares().print();
//std::cout << "\n\n\n";
//
//stateCopy.occupancy().print();
//m_state.occupancy().print();
//std::cout << "\n\n\n";
//
//stateCopy.whiteOccupancy().print();
//m_state.whiteOccupancy().print();
//std::cout << "\n\n\n";
//
//stateCopy.blackOccupancy().print();
//m_state.blackOccupancy().print();
//std::cout << "\n\n\n";
//
//for (std::uint32_t i{ whitePieceOffset }; i < pieceCount; ++i)
//{
//	const Piece piece{ static_cast<Piece>(i) };
//	stateCopy.pieceOccupancy(piece).print();
//	m_state.pieceOccupancy(piece).print();
//	std::cout << "\n\n\n";
//}