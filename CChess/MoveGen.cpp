#include "MoveGen.h"

#include <array>
#include <cstdint>

#include "PreGen.h"
#include "ChessConstants.hpp"
#include "Move.h"

static cachealign PreGen preGen;


template<bool white>
static BitBoard pawnDoublesMask(BitBoard pawns) noexcept
{
	constexpr std::uint64_t mask{ white
		? 0b0000000011111111000000000000000000000000000000000000000000000000
		: 0b0000000000000000000000000000000000000000000000001111111100000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnPromotesMask(BitBoard pawns) noexcept
{
	constexpr std::uint64_t mask{ white
		? 0b0000000000000000000000000000000000000000000000001111111100000000
		: 0b0000000011111111000000000000000000000000000000000000000000000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnEnpassantsMask(BitBoard pawns) noexcept
{
	constexpr std::uint64_t mask{ white
		? 0b0000000000000000000000000000000011111111000000000000000000000000
		: 0b0000000000000000000000001111111100000000000000000000000000000000 };

	return BitBoard(pawns.board() & mask);
}

//TODO: maybe check if any bitboards are empty before further logic
template<bool white>
static BitBoard pawnShiftedMoves(BitBoard pawns, std::uint64_t mask) noexcept
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
static void pawnPromotes(BitBoard pawns, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece queen{ white ? Piece::WhiteQueen : Piece::BlackQueen };
	constexpr Piece knight{ white ? Piece::WhiteKnight : Piece::BlackKnight };
	constexpr Piece bishop{ white ? Piece::WhiteBishop : Piece::BlackBishop };
	constexpr Piece rook{ white ? Piece::WhiteRook : Piece::BlackRook };
	constexpr Piece piece{ white ? Piece::WhitePawn : Piece::BlackPawn };

	BitBoard shiftedPawns{ pawnShiftedMoves<white>(pawns, ~state.occupancy().board()) };

	while (shiftedPawns.board())
	{
		const int destinationIndex{ shiftedPawns.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 8 : destinationIndex + 8 };

		moveList.pushQuietPromote<piece, queen>(sourceIndex, destinationIndex);
		moveList.pushQuietPromote<piece, knight>(sourceIndex, destinationIndex);
		moveList.pushQuietPromote<piece, bishop>(sourceIndex, destinationIndex);
		moveList.pushQuietPromote<piece, rook>(sourceIndex, destinationIndex);
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

			moveList.pushAttackPromote<piece, queen>(attackPiece, sourceIndex, attackIndex);
			moveList.pushAttackPromote<piece, knight>(attackPiece, sourceIndex, attackIndex);
			moveList.pushAttackPromote<piece, bishop>(attackPiece, sourceIndex, attackIndex);
			moveList.pushAttackPromote<piece, rook>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void pawnDoubles(BitBoard pawns, MoveList& moveList, const State& state) noexcept //TODO: maybe keep track of when there are no more pawns on that rank?
{
	const std::uint64_t mask{ ~state.occupancy().board() };
	const BitBoard pawnsShiftedOnce{ pawnShiftedMoves<white>(pawns, mask) }; //TODO: maybe exit early?
	BitBoard pawnsShiftedTwice{ pawnShiftedMoves<white>(pawnsShiftedOnce, mask) };

	while (pawnsShiftedTwice.board())
	{
		const int destinationIndex{ pawnsShiftedTwice.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 16 : destinationIndex + 16 };

		moveList.pushQuiet<white ? Piece::WhitePawn : Piece::BlackPawn>(sourceIndex, destinationIndex);
	}
}

template<bool white>
static void pawnEnpassants(BitBoard pawns, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece sourcePiece{ white ? Piece::WhitePawn : Piece::BlackPawn };
	constexpr Piece attackPiece{ white ? Piece::BlackPawn : Piece::WhitePawn };

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

			moveList.pushEnpassant<sourcePiece, attackPiece>(sourceIndex, attackIndex, white ? attackIndex - 8 : attackIndex + 8);
		}
	}
}

template<bool white>
static void pawnNormals(BitBoard pawns, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece piece{ white ? Piece::WhitePawn : Piece::BlackPawn };

	BitBoard shiftedPawns{ pawnShiftedMoves<white>(pawns, ~state.occupancy().board()) };

	//quiets
	while (shiftedPawns.board())
	{
		const int destinationIndex{ shiftedPawns.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 8 : destinationIndex + 8 };

		moveList.pushQuiet<piece>(sourceIndex, destinationIndex);
	}

	//attacks
	while (pawns.board())
	{
		const int sourceIndex{ pawns.popLeastSignificantBit() };
		BitBoard attack{ white 
			? preGen.whitePawnAttack(sourceIndex).board() & state.blackOccupancy().board()
			: preGen.blackPawnAttack(sourceIndex).board() & state.whiteOccupancy().board() };

		while (attack.board())
		{
			const int attackIndex{ attack.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<white>(attackIndex) };

			moveList.pushAttack<piece>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void pawnMoves(BitBoard pawns, MoveList& moveList, const State& state) noexcept
{
	const BitBoard pawnPromoteMoves(pawnPromotesMask<white>(pawns));
	const BitBoard pawnDoubleMoves(pawnDoublesMask<white>(pawns));
	const BitBoard pawnEnpassantMoves(pawnEnpassantsMask<white>(pawns));
	const BitBoard pawnNormalMoves(pawns.board() & ~pawnPromoteMoves.board());

	pawnPromotes<white>(pawnPromoteMoves, moveList, state);
	pawnDoubles<white>(pawnDoubleMoves, moveList, state);
	pawnEnpassants<white>(pawnEnpassantMoves, moveList, state);
	pawnNormals<white>(pawnNormalMoves, moveList, state);
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
			moveList.pushAttack<piece>(attackPiece, sourceIndex, attackIndex);
		}
	}
}


static void kingMoves(BitBoard kings, MoveList& moveList, const State& state) noexcept
{
	constexpr bool white = true;

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