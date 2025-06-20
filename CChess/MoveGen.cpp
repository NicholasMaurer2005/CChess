#include "MoveGen.h"

#include "PreGen.h"
#include "ChessConstants.hpp"
#include "Move.h"

static cachealign PreGen preGen;


static bool whitePawnDouble(int index)
{
	constexpr BitBoard doubleMask{ 0b0000000011111111000000000000000000000000000000000000000000000000 };
	return doubleMask.test(index);
}

static bool blackPawnDouble(int index)
{
	constexpr BitBoard doubleMask{ 0b0000000000000000000000000000000000000000000000001111111100000000 };
	return doubleMask.test(index);
}

template<bool white>
static void pawnMoves(BitBoard pawns, MoveList& moveList, State& state) noexcept
{
	constexpr int single{ white ? 8 : -8 };
	constexpr int pawnDouble{ white ? 16 : -16 };
	constexpr Piece piece{ white ? Piece::WhitePawn : Piece::BlackPawn };

	while (pawns)
	{
		const int source{ pawns.popLeastSignificantBit() };

		const BitBoard attacks{ white ? preGen.whitePawnAttack(source) : preGen.blackPawnAttack(source) };
		BitBoard moves{ attacks & (white ? state.blackOccupancy() : state.whiteOccupancy) };
		
		while (moves)
		{
			const int destination{ moves.popLeastSignificantBit() };
			moveList.pushQuiet<piece>(source, destination);
		}

		const int singleDestination{ source + single };
		const int doubleDestination{ source + pawnDouble };

		if (singleDestination < boardSize && !state.occupancy().test(singleDestination))
		{
			moveList.pushQuiet<piece>(source, singleDestination);
		}
		
		if ((white ? whitePawnDouble(source) : blackPawnDouble(source)) && !state.occupancy().test(doubleDestination))
		{
			moveList.pushQuiet<piece>(source, doubleDestination);
		}
	}
}

static void knightMoves(BitBoard knights, MoveList& moveList) noexcept
{

}

static void kingMoves(BitBoard kings, MoveList& moveList) noexcept
{

}

static void bishopMoves(const State& state, MoveList& moveList) noexcept
{

}

static void rookMoves(const State& state, MoveList& moveList) noexcept
{

}

static void queenMoves(const State& state, MoveList& moveList) noexcept
{

}



//public
MoveGen::MoveGen() noexcept {}



MoveList MoveGen::generateMoves(const State& state) const noexcept
{
	return MoveList();
}