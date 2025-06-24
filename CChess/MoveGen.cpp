#include "MoveGen.h"

#include <array>
#include <cstdint>

#include "PreGen.h"
#include "ChessConstants.hpp"
#include "Move.h"

static cachealign PreGen preGen;


template<bool white>
static BitBoard pawnDoublesMask(BitBoard pawns)
{
	constexpr std::uint64_t mask{ white
		? 0b0000000011111111000000000000000000000000000000000000000000000000
		: 0b0000000000000000000000000000000000000000000000001111111100000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnPromotesMask(BitBoard pawns)
{
	constexpr std::uint64_t mask{ white
		? 0b0000000000000000000000000000000000000000000000001111111100000000
		: 0b0000000011111111000000000000000000000000000000000000000000000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnEnpassantsMask(BitBoard pawns)
{
	constexpr std::uint64_t mask{ white
		? 0b0000000000000000000000000000000011111111000000000000000000000000
		: 0b0000000000000000000000001111111100000000000000000000000000000000 };

	return BitBoard(pawns.board() & mask);
}

//TODO: maybe check if any bitboards are empty before further logic
template<bool white>
static BitBoard pawnShiftedMoves(BitBoard pawns, std::uint64_t mask)
{
	if constexpr (white)
	{
		const std::uint64_t shiftedPawns{ pawns.board() >> 8};
		return BitBoard(shiftedPawns & mask);
	}
	else
	{
		const std::uint64_t shiftedPawns{ pawns.board() << 8 };
		return BitBoard(shiftedPawns & mask);
	}
}

template<bool white>
static void pawnPromotes(BitBoard pawns, MoveList& moveList, const State& state)
{
	constexpr Piece queen{ white ? Piece::WhiteQueen : Piece::BlackQueen };
	constexpr Piece knight{ white ? Piece::WhiteKnight : Piece::BlackKnight };
	constexpr Piece bishop{ white ? Piece::WhiteBishop : Piece::BlackBishop };
	constexpr Piece rook{ white ? Piece::WhiteRook : Piece::BlackRook };

	BitBoard pawnQuiets{ pawnShiftedMoves<white>(pawns, ~state.occupancy().board()) };

	while (pawnQuiets.board())
	{
		const int destinationIndex{ pawnQuiets.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 8 : destinationIndex + 8 };

		moveList.pushQuietPromote<white>(sourceIndex, destinationIndex, queen);
		moveList.pushQuietPromote<white>(sourceIndex, destinationIndex, knight);
		moveList.pushQuietPromote<white>(sourceIndex, destinationIndex, bishop);
		moveList.pushQuietPromote<white>(sourceIndex, destinationIndex, rook);
	}

	while (pawns.board())
	{
		const int sourceIndex{ pawns.popLeastSignificantBit() };

		BitBoard pawnAttacks{ white
			? (preGen.whitePawnAttack(sourceIndex).board() & state.blackOccupancy().board())
			: (preGen.blackPawnAttack(sourceIndex).board() & state.whiteOccupancy().board()) };

		while (pawnAttacks.board())
		{
			const int attackIndex{ pawnAttacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			moveList.pushAttackPromote<white>(sourceIndex, attackIndex, queen, attackPiece);
			moveList.pushAttackPromote<white>(sourceIndex, attackIndex, knight, attackPiece);
			moveList.pushAttackPromote<white>(sourceIndex, attackIndex, bishop, attackPiece);
			moveList.pushAttackPromote<white>(sourceIndex, attackIndex, rook, attackPiece);
		}
	}
}

template<bool white>
static void pawnDoubles(BitBoard pawns, MoveList& moveList, const State& state)//TODO: maybe keep track of when there are no more pawns on that rank
{
	const std::uint64_t mask{ ~state.occupancy().board() };
	const BitBoard pawnsShiftedOnce{ pawnShiftedMoves<white>(pawns, mask) };
	BitBoard pawnsShiftedTwice{ pawnShiftedMoves<white>(pawnsShiftedOnce, mask) };

	while (pawnsShiftedTwice.board())
	{
		const int destinationIndex{ pawnsShiftedTwice.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 16 : destinationIndex + 16 };

		moveList.pushQuiet<white ? Piece::WhitePawn : Piece::BlackPawn>(sourceIndex, destinationIndex);
	}
}

template<bool white>
static void pawnEnpassants(BitBoard pawns, MoveList& moveList, const State& state)
{
	const BitBoard enpassantSquare{ white ? state.blackEnpassantSquare() : state.whiteEnpassantSquare() };

	if (!enpassantSquare.board()) return;

	while (pawns.board())
	{
		const int sourceIndex{ pawns.popLeastSignificantBit() };
		BitBoard attacks{ white
			? preGen.whitePawnAttack(sourceIndex).board() & enpassantSquare.board()
			: preGen.blackPawnAttack(sourceIndex).board() & enpassantSquare.board() };
		
		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			moveList.pushEnpassant<white>(sourceIndex, attackIndex, white ? attackIndex - 8 : attackIndex + 8);
		}
	}
}

//template<bool white>
static void pawnMoves(BitBoard pawns, MoveList& moveList, const State& state)
{
	constexpr bool white = true;

	const BitBoard pawnPromoteMoves(pawnPromotesMask<white>(pawns));
	const BitBoard pawnDoubleMoves(pawnDoublesMask<white>(pawns));
	const BitBoard pawnEnpassantMoves(pawnEnpassantsMask<white>(pawns));
	const BitBoard pawnNormalMoves(pawns.board() & ~pawnPromoteMoves.board());

	pawnPromotes<white>(pawnPromoteMoves, moveList, state);
	pawnDoubles<white>(pawnDoubleMoves, moveList, state);
}

template<bool white>
static void knightMoves(BitBoard knights, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece piece{ white ? Piece::WhiteKnight : Piece::BlackKnight };

	while (knights.board())
	{
		const int sourceIndex{ knights.popLeastSignificantBit() };
		const std::uint64_t knightAttacks{ preGen.knightAttack(sourceIndex).board() };

		BitBoard quiets{ knightAttacks & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board()) };
		BitBoard attacks{ knightAttacks & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int quietIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<piece>(sourceIndex, quietIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
			moveList.pushAttack<piece>(sourceIndex, attackIndex, attackPiece);
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