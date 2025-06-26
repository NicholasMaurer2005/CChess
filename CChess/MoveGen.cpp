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
	constexpr Piece pawn{ white ? Piece::WhitePawn : Piece::BlackPawn };

	BitBoard shiftedPawns{ pawnShiftedMoves<white>(pawns, ~state.occupancy().board()) };

	while (shiftedPawns.board())
	{
		const int destinationIndex{ shiftedPawns.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 8 : destinationIndex + 8 };

		moveList.pushQuietPromote<pawn, queen>(sourceIndex, destinationIndex);
		moveList.pushQuietPromote<pawn, knight>(sourceIndex, destinationIndex);
		moveList.pushQuietPromote<pawn, bishop>(sourceIndex, destinationIndex);
		moveList.pushQuietPromote<pawn, rook>(sourceIndex, destinationIndex);
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

			moveList.pushAttackPromote<pawn, queen>(attackPiece, sourceIndex, attackIndex);
			moveList.pushAttackPromote<pawn, knight>(attackPiece, sourceIndex, attackIndex);
			moveList.pushAttackPromote<pawn, bishop>(attackPiece, sourceIndex, attackIndex);
			moveList.pushAttackPromote<pawn, rook>(attackPiece, sourceIndex, attackIndex);
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

		moveList.pushDoublePawn<white ? Piece::WhitePawn : Piece::BlackPawn>(sourceIndex, destinationIndex);
	}
}

template<bool white>
static void pawnEnpassants(BitBoard pawns, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece sourcePiece{ white ? Piece::WhitePawn : Piece::BlackPawn };
	constexpr Piece attackPiece{ white ? Piece::BlackPawn : Piece::WhitePawn };

	const BitBoard enpassantSquare{ white ? state.blackEnpassantSquare() : state.whiteEnpassantSquare() };

	if (!enpassantSquare.board()) [[likely]] return; //TODO: test if [[likely]] is faster

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
	constexpr Piece pawn{ white ? Piece::WhitePawn : Piece::BlackPawn };

	BitBoard shiftedPawns{ pawnShiftedMoves<white>(pawns, ~state.occupancy().board()) };

	//quiets
	while (shiftedPawns.board())
	{
		const int destinationIndex{ shiftedPawns.popLeastSignificantBit() };
		const int sourceIndex{ white ? destinationIndex - 8 : destinationIndex + 8 };

		moveList.pushQuiet<pawn>(sourceIndex, destinationIndex);
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

			moveList.pushAttack<pawn>(attackPiece, sourceIndex, attackIndex);
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
	constexpr Piece knight{ white ? Piece::WhiteKnight : Piece::BlackKnight };

	while (knights.board())
	{
		const int sourceIndex{ knights.popLeastSignificantBit() };
		const std::uint64_t knightMoves{ preGen.knightAttack(sourceIndex).board() };

		BitBoard quiets{ knightMoves & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board()) };
		BitBoard attacks{ knightMoves & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int quietIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<knight>(sourceIndex, quietIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
			moveList.pushAttack<knight>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void kingCastles(MoveList& moveList, const State& state) noexcept
{
	if constexpr (white)
	{
		if (state.castleWhiteKingSide())
		{
			constexpr std::uint64_t castleMask{ 0b0000011000000000000000000000000000000000000000000000000000000000 };

			if (!(state.occupancy().board() & castleMask))
			{
				moveList.pushCastle<Castle::WhiteKingSide>();
			}
		}

		if (state.castleWhiteQueenSide())
		{
			constexpr std::uint64_t castleMask{ 0b0111000000000000000000000000000000000000000000000000000000000000 };

			if (!(state.occupancy().board() & castleMask))
			{
				moveList.pushCastle<Castle::WhiteQueenSide>();
			}
		}
	}
	else
	{
		if (state.castleBlackKingSide())
		{
			constexpr std::uint64_t castleMask{ 0b0000000000000000000000000000000000000000000000000000000000000110 };

			if (!(state.occupancy().board() & castleMask))
			{
				moveList.pushCastle<Castle::BlackKingSide>();
			}
		}

		if (state.castleBlackQueenSide())
		{
			constexpr std::uint64_t castleMask{ 0b0000000000000000000000000000000000000000000000000000000001110000 };

			if (!(state.occupancy().board() & castleMask))
			{
				moveList.pushCastle<Castle::BlackQueenSide>();
			}
		}
	}
}

template<bool white>
static void kingMoves(BitBoard kings, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece king{ white ? Piece::WhiteKing : Piece::BlackKing };

	const int sourceIndex{ kings.leastSignificantBit() };
	const BitBoard kingMoves{ preGen.knightAttack(sourceIndex) };

	BitBoard quiets{ kingMoves.board() & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board())};
	BitBoard attacks{ kingMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board())};

	while (quiets.board())
	{
		const int destinationIndex{ quiets.popLeastSignificantBit() };
		moveList.pushQuiet<king>(sourceIndex, destinationIndex);
	}

	while (attacks.board())
	{
		const int attackIndex{ attacks.popLeastSignificantBit() };
		const Piece attackPiece{ state.findPiece<white>(attackIndex) };
		moveList.pushAttack<king>(attackPiece, sourceIndex, attackIndex);
	}

	if (!state.kingInCheck())
	{
		kingCastles<white>(moveList, state);
	}
}

//template<bool white>
static void bishopMoves(BitBoard bishops, MoveList& moveList, const State& state) noexcept
{
	constexpr bool white{};

	constexpr Piece bishop{ white ? Piece::WhiteBishop : Piece::BlackBishop };

	while (bishops.board())
	{
		const int sourceIndex{ bishops.popLeastSignificantBit() };
		const BitBoard bishopMoves{ preGen.bishopAttack(sourceIndex, state.occupancy()) };

		BitBoard quiets{ bishopMoves.board() & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board()) };
		BitBoard attacks{ bishopMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int destinationIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<bishop>(sourceIndex, destinationIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<white>(attackIndex) };
			moveList.pushAttack<bishop>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

static void rookMoves(BitBoard rooks, MoveList& moveList, const State& state) noexcept
{
	constexpr bool white{};

	constexpr Piece rook{ white ? Piece::WhiteRook : Piece::BlackRook };

	while (rooks.board())
	{
		const int sourceIndex{ rooks.popLeastSignificantBit() };
		const BitBoard rookMoves{ preGen.rookAttack(sourceIndex, state.occupancy()) };

		BitBoard quiets{ rookMoves.board() & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board()) };
		BitBoard attacks{ rookMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int destinationIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<rook>(sourceIndex, destinationIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<white>(attackIndex) };
			moveList.pushAttack<rook>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

static void queenMoves(BitBoard queens, MoveList& moveList, const State& state) noexcept //TODO: maybe make no loop for queen? always one queen per side
{
	constexpr bool white{};

	constexpr Piece queen{ white ? Piece::WhiteQueen : Piece::BlackQueen };

	while (queens.board())
	{
		const int sourceIndex{ queens.popLeastSignificantBit() };
		const BitBoard queenMoves{ preGen.bishopAttack(sourceIndex, state.occupancy()).board() | preGen.rookAttack(sourceIndex, state.occupancy()).board() };

		BitBoard quiets{ queenMoves.board() & (white ? ~state.whiteOccupancy().board() : ~state.blackOccupancy().board()) };
		BitBoard attacks{ queenMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int destinationIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<queen>(sourceIndex, destinationIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<white>(attackIndex) };
			moveList.pushAttack<queen>(attackPiece, sourceIndex, attackIndex);
		}
	}
}



//public
MoveGen::MoveGen() noexcept {}

MoveList MoveGen::generateMoves(bool white, const State& state) const noexcept
{
	MoveList moveList;
	
	if (white) //TODO: maybe remove if statement and make template?
	{ //TODO: maybe add if statements to test for occupancy before doing any logic?
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhitePawn>(), moveList, state);
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhiteKnight>(), moveList, state);
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhiteKing>(), moveList, state);
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhiteBishop>(), moveList, state);
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhiteRook>(), moveList, state);
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhiteQueen>(), moveList, state);
	}
	else
	{
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackPawn>(), moveList, state);
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackKnight>(), moveList, state);
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackKing>(), moveList, state);
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackBishop>(), moveList, state);
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackRook>(), moveList, state);
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackQueen>(), moveList, state);
	}
}