#include "MoveGen.h"

#include "PreGen.h"
#include "ChessConstants.hpp"
#include "Move.h"

static cachealign PreGen preGen;

template<bool white>
static void pawnMoves(BitBoard pawns, MoveList& moveList) noexcept
{
	while (pawns)
	{
		const int index{ pawns.popLeastSignificantBit };

		BitBoard moves{ white ? preGen.whitePawnAttack(index) : preGen.blackPawnAttack(index) };
		
		while (moves)
		{
			const int moveIndex{ moves.popLeastSignificantBit() };
			moveList.pushQuiet(index, moveIndex);
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

}