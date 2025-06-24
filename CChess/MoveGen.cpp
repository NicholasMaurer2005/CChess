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

//template<bool white>
static void pawnMoves(BitBoard pawns, MoveList& moveList, State& state) noexcept
{
	constexpr bool white = true;

	constexpr int single{ white ? 8 : -8 };
	constexpr int pawnDouble{ white ? 16 : -16 };
	constexpr Piece piece{ white ? Piece::WhitePawn : Piece::BlackPawn };

	while (pawns.board())
	{
		const int source{ pawns.popLeastSignificantBit() };

		BitBoard attacks{ white ? (preGen.whitePawnAttack(source).board() & state.blackOccupancy().board()) : (preGen.blackPawnAttack(source).board() & state.whiteOccupancy().board()) };

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			moveList.pushAttack<piece>(source, attackIndex, attackPiece);
		}

		const int singleDestination{ source + single };
		const int doubleDestination{ source + pawnDouble };

		if (!state.occupancy().test(singleDestination)) //TODO: make pawn quiet lookup table?
		{
			moveList.pushQuiet<piece>(source, singleDestination);

			if ((white ? whitePawnDouble(source) : blackPawnDouble(source)) && !state.occupancy().test(doubleDestination))
			{
				moveList.pushQuiet<piece>(source, doubleDestination);
			}
		}
	}
}

template<bool white>
static void knightMoves(BitBoard knights, MoveList& moveList, State& state) noexcept
{
	constexpr Piece piece{ white ? Piece::WhiteKnight : Piece::BlackKnight };

	while (knights.board())
	{
		const int sourceIndex{ knights.popLeastSignificantBit() };
		const std::uint64_t knightAttacks{ preGen.knightAttack(sourceIndex).board() };

		BitBoard attacks{ knightAttacks & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };
		BitBoard quiets{ knightAttacks & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board()) };

		while (attacks.board())
		{
			const int moveIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(moveIndex) };
			moveList.pushAttack<piece>(sourceIndex, moveIndex, attackPiece);
		}

		while (quiets.board())
		{
			const int moveIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<piece>(sourceIndex, moveIndex);
		}
	}
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