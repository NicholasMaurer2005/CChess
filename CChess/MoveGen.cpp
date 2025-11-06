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
		? 0b0000000000000000000000000000000000000000000000001111111100000000
		: 0b0000000011111111000000000000000000000000000000000000000000000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnPromotesMask(BitBoard pawns) noexcept
{
	constexpr std::uint64_t mask{ white
		? 0b0000000011111111000000000000000000000000000000000000000000000000
		: 0b0000000000000000000000000000000000000000000000001111111100000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnEnpassantsMask(BitBoard pawns) noexcept
{
	constexpr std::uint64_t mask{ white
		? 0b0000000000000000000000001111111100000000000000000000000000000000
		: 0b0000000000000000000000000000000011111111000000000000000000000000 };

	return BitBoard(pawns.board() & mask);
}

template<bool white>
static BitBoard pawnShiftedMoves(BitBoard pawns, std::uint64_t mask) noexcept
{
	if constexpr (white)
	{
		const std::uint64_t shiftedPawns{ pawns.board() << 8};
		return BitBoard(shiftedPawns & mask);
	}
	else
	{
		const std::uint64_t shiftedPawns{ pawns.board() >> 8 };
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
static void pawnDoubles(BitBoard pawns, MoveList& moveList, const State& state) noexcept 
{
	const std::uint64_t mask{ ~state.occupancy().board() };
	const BitBoard pawnsShiftedOnce{ pawnShiftedMoves<white>(pawns, mask) }; 
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

	if (!state.enpassantSquare().board()) return;

	while (pawns.board())
	{
		const int sourceIndex{ pawns.popLeastSignificantBit() };
		BitBoard attacks{ white
			? preGen.whitePawnAttack(sourceIndex).board() & state.enpassantSquare().board()
			: preGen.blackPawnAttack(sourceIndex).board() & state.enpassantSquare().board() };
		
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
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

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
		const std::uint64_t knightMoves{ preGen.knightMove(sourceIndex).board() };

		BitBoard quiets{ knightMoves & ~state.occupancy().board() };
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
			constexpr std::uint64_t castleOccupancyMask = (1ULL << 5) | (1ULL << 6); // f1, g1
			constexpr std::uint64_t castleAttackMask = (1ULL << 4) | (1ULL << 5) | (1ULL << 6); // e1, f1, g1

			if (!(state.occupancy().board() & castleOccupancyMask) && !(state.blackSquares().board() & castleAttackMask))
			{
				moveList.pushCastle<Castle::WhiteKingSide>();
			}
		}

		if (state.castleWhiteQueenSide())
		{
			constexpr std::uint64_t castleOccupancyMask = (1ULL << 1) | (1ULL << 2) | (1ULL << 3); // b1, c1, d1
			constexpr std::uint64_t castleAttackMask = (1ULL << 2) | (1ULL << 3) | (1ULL << 4); // c1, d1, e1

			if (!(state.occupancy().board() & castleOccupancyMask) && !(state.blackSquares().board() & castleAttackMask))
			{
				moveList.pushCastle<Castle::WhiteQueenSide>();
			}
		}
	}
	else
	{
		if (state.castleBlackKingSide())
		{
			constexpr std::uint64_t castleOccupancyMask = (1ULL << 61) | (1ULL << 62); // f8, g8
			constexpr std::uint64_t castleAttackMask = (1ULL << 60) | (1ULL << 61) | (1ULL << 62); // e8, f8, g8

			if (!(state.occupancy().board() & castleOccupancyMask) && !(state.whiteSquares().board() & castleAttackMask))
			{
				moveList.pushCastle<Castle::BlackKingSide>();
			}
		}

		if (state.castleBlackQueenSide())
		{
			constexpr std::uint64_t castleOccupancyMask = (1ULL << 57) | (1ULL << 58) | (1ULL << 59); // b8, c8, d8
			constexpr std::uint64_t castleAttackMask = (1ULL << 58) | (1ULL << 59) | (1ULL << 60); // c8, d8, e8

			if (!(state.occupancy().board() & castleOccupancyMask) && !(state.whiteSquares().board() & castleAttackMask))
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
	const BitBoard kingMoves{ preGen.kingMove(sourceIndex) };

	BitBoard quiets{ kingMoves.board() & ~state.occupancy().board() };
	BitBoard attacks{ kingMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board())};

	while (quiets.board())
	{
		const int destinationIndex{ quiets.popLeastSignificantBit() };
		moveList.pushQuiet<king>(sourceIndex, destinationIndex);
	}

	while (attacks.board())
	{
		const int attackIndex{ attacks.popLeastSignificantBit() };
		const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
		moveList.pushAttack<king>(attackPiece, sourceIndex, attackIndex);
	}

	kingCastles<white>(moveList, state);
}

template<bool white>
static void bishopMoves(BitBoard bishops, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece bishop{ white ? Piece::WhiteBishop : Piece::BlackBishop };

	while (bishops.board())
	{
		const int sourceIndex{ bishops.popLeastSignificantBit() };
		const BitBoard bishopMoves{ preGen.bishopMove(sourceIndex, state.occupancy()) };

		BitBoard quiets{ bishopMoves.board() & ~state.occupancy().board() };
		BitBoard attacks{ bishopMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int destinationIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<bishop>(sourceIndex, destinationIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
			moveList.pushAttack<bishop>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void rookMoves(BitBoard rooks, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece rook{ white ? Piece::WhiteRook : Piece::BlackRook };

	while (rooks.board())
	{
		const int sourceIndex{ rooks.popLeastSignificantBit() };
		const BitBoard rookMoves{ preGen.rookMove(sourceIndex, state.occupancy()) };

		BitBoard quiets{ rookMoves.board() & ~state.occupancy().board() };
		BitBoard attacks{ rookMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int destinationIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<rook>(sourceIndex, destinationIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			moveList.pushAttack<rook>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void queenMoves(BitBoard queens, MoveList& moveList, const State& state) noexcept
{
	constexpr Piece queen{ white ? Piece::WhiteQueen : Piece::BlackQueen };

	while (queens.board())
	{
		const int sourceIndex{ queens.popLeastSignificantBit() };
		const BitBoard queenMoves{ preGen.bishopMove(sourceIndex, state.occupancy()).board() | preGen.rookMove(sourceIndex, state.occupancy()).board() };

		BitBoard quiets{ queenMoves.board() & ~state.occupancy().board() };
		BitBoard attacks{ queenMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (quiets.board())
		{
			const int destinationIndex{ quiets.popLeastSignificantBit() };
			moveList.pushQuiet<queen>(sourceIndex, destinationIndex);
		}

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			moveList.pushAttack<queen>(attackPiece, sourceIndex, attackIndex);
		}
	}
}



//CAPTURE MOVES

template<bool white>
static void pawnPromoteCaptures(BitBoard pawns, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece queen{ white ? Piece::WhiteQueen : Piece::BlackQueen };
	constexpr Piece knight{ white ? Piece::WhiteKnight : Piece::BlackKnight };
	constexpr Piece bishop{ white ? Piece::WhiteBishop : Piece::BlackBishop };
	constexpr Piece rook{ white ? Piece::WhiteRook : Piece::BlackRook };
	constexpr Piece pawn{ white ? Piece::WhitePawn : Piece::BlackPawn };

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

			captureList.pushAttackPromote<pawn, queen>(attackPiece, sourceIndex, attackIndex);
			captureList.pushAttackPromote<pawn, knight>(attackPiece, sourceIndex, attackIndex);
			captureList.pushAttackPromote<pawn, bishop>(attackPiece, sourceIndex, attackIndex);
			captureList.pushAttackPromote<pawn, rook>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void pawnNormalCaptures(BitBoard pawns, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece pawn{ white ? Piece::WhitePawn : Piece::BlackPawn };

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
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			captureList.pushAttack<pawn>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void pawnEnpassants(BitBoard pawns, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece sourcePiece{ white ? Piece::WhitePawn : Piece::BlackPawn };
	constexpr Piece attackPiece{ white ? Piece::BlackPawn : Piece::WhitePawn };

	if (!state.enpassantSquare().board()) return;

	while (pawns.board())
	{
		const int sourceIndex{ pawns.popLeastSignificantBit() };
		BitBoard attacks{ white
			? preGen.whitePawnAttack(sourceIndex).board() & state.enpassantSquare().board()
			: preGen.blackPawnAttack(sourceIndex).board() & state.enpassantSquare().board() };

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };

			captureList.pushEnpassant<sourcePiece, attackPiece>(sourceIndex, attackIndex, white ? attackIndex - 8 : attackIndex + 8);
		}
	}
}

template<bool white>
static void pawnCaptures(BitBoard pawns, CaptureList& captureList, const State& state) noexcept
{
	const BitBoard pawnPromoteMoves(pawnPromotesMask<white>(pawns));
	const BitBoard pawnEnpassantMoves(pawnEnpassantsMask<white>(pawns));
	const BitBoard pawnNormalMoves(pawns.board() & ~pawnPromoteMoves.board());

	pawnPromoteCaptures<white>(pawnPromoteMoves, captureList, state);
	pawnEnpassants<white>(pawnEnpassantMoves, captureList, state);
	pawnNormalCaptures<white>(pawnNormalMoves, captureList, state);
}

template<bool white>
static void knightCaptures(BitBoard knights, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece knight{ white ? Piece::WhiteKnight : Piece::BlackKnight };

	while (knights.board())
	{
		const int sourceIndex{ knights.popLeastSignificantBit() };
		const std::uint64_t knightMoves{ preGen.knightMove(sourceIndex).board() };
		BitBoard attacks{ knightMoves & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
			captureList.pushAttack<knight>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void kingCaptures(BitBoard kings, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece king{ white ? Piece::WhiteKing : Piece::BlackKing };

	const int sourceIndex{ kings.leastSignificantBit() };
	const BitBoard kingMoves{ preGen.kingMove(sourceIndex) };
	BitBoard attacks{ kingMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

	while (attacks.board())
	{
		const int attackIndex{ attacks.popLeastSignificantBit() };
		const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
		captureList.pushAttack<king>(attackPiece, sourceIndex, attackIndex);
	}
}

template<bool white>
static void bishopCaptures(BitBoard bishops, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece bishop{ white ? Piece::WhiteBishop : Piece::BlackBishop };

	while (bishops.board())
	{
		const int sourceIndex{ bishops.popLeastSignificantBit() };
		const BitBoard bishopMoves{ preGen.bishopMove(sourceIndex, state.occupancy()) };
		BitBoard attacks{ bishopMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };
			captureList.pushAttack<bishop>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void rookCaptures(BitBoard rooks, CaptureList& captureList, const State& state) noexcept
{
	constexpr Piece rook{ white ? Piece::WhiteRook : Piece::BlackRook };

	while (rooks.board())
	{
		const int sourceIndex{ rooks.popLeastSignificantBit() };
		const BitBoard rookMoves{ preGen.rookMove(sourceIndex, state.occupancy()) };
		BitBoard attacks{ rookMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			captureList.pushAttack<rook>(attackPiece, sourceIndex, attackIndex);
		}
	}
}

template<bool white>
static void queenCaptures(BitBoard queens, CaptureList& captureList, const State& state) noexcept 
{
	constexpr Piece queen{ white ? Piece::WhiteQueen : Piece::BlackQueen };

	while (queens.board())
	{
		const int sourceIndex{ queens.popLeastSignificantBit() };
		const BitBoard queenMoves{ preGen.bishopMove(sourceIndex, state.occupancy()).board() | preGen.rookMove(sourceIndex, state.occupancy()).board() };
		BitBoard attacks{ queenMoves.board() & (white ? state.blackOccupancy().board() : state.whiteOccupancy().board()) };

		while (attacks.board())
		{
			const int attackIndex{ attacks.popLeastSignificantBit() };
			const Piece attackPiece{ state.findPiece<!white>(attackIndex) };

			captureList.pushAttack<queen>(attackPiece, sourceIndex, attackIndex);
		}
	}
}



CaptureList MoveGen::generateCaptures(bool white, const State& state) const noexcept
{
	CaptureList captureList;

	if (white) 
	{ //TODO: maybe add if statements to test for occupancy before doing any logic?
		pawnCaptures<true>(state.pieceOccupancyT<Piece::WhitePawn>(), captureList, state);
		knightCaptures<true>(state.pieceOccupancyT<Piece::WhiteKnight>(), captureList, state);
		bishopCaptures<true>(state.pieceOccupancyT<Piece::WhiteBishop>(), captureList, state);
		rookCaptures<true>(state.pieceOccupancyT<Piece::WhiteRook>(), captureList, state);
		queenCaptures<true>(state.pieceOccupancyT<Piece::WhiteQueen>(), captureList, state);
		kingCaptures<true>(state.pieceOccupancyT<Piece::WhiteKing>(), captureList, state);
	}
	else
	{
		pawnCaptures<false>(state.pieceOccupancyT<Piece::BlackPawn>(), captureList, state);
		knightCaptures<false>(state.pieceOccupancyT<Piece::BlackKnight>(), captureList, state);
		bishopCaptures<false>(state.pieceOccupancyT<Piece::BlackBishop>(), captureList, state);
		rookCaptures<false>(state.pieceOccupancyT<Piece::BlackRook>(), captureList, state);
		queenCaptures<false>(state.pieceOccupancyT<Piece::BlackQueen>(), captureList, state);
		kingCaptures<false>(state.pieceOccupancyT<Piece::BlackKing>(), captureList, state);
	}

	return captureList;
}



//public
MoveGen::MoveGen() noexcept {}

MoveList MoveGen::generateMoves(bool white, const State& state) const noexcept
{
	MoveList moveList;
	
	if (white)
	{ //TODO: maybe add if statements to test for occupancy before doing any logic?
		pawnMoves<true>(state.pieceOccupancyT<Piece::WhitePawn>(), moveList, state);
		knightMoves<true>(state.pieceOccupancyT<Piece::WhiteKnight>(), moveList, state);
		bishopMoves<true>(state.pieceOccupancyT<Piece::WhiteBishop>(), moveList, state);
		rookMoves<true>(state.pieceOccupancyT<Piece::WhiteRook>(), moveList, state);
		queenMoves<true>(state.pieceOccupancyT<Piece::WhiteQueen>(), moveList, state);
		kingMoves<true>(state.pieceOccupancyT<Piece::WhiteKing>(), moveList, state);
	}
	else
	{
		pawnMoves<false>(state.pieceOccupancyT<Piece::BlackPawn>(), moveList, state);
		knightMoves<false>(state.pieceOccupancyT<Piece::BlackKnight>(), moveList, state);
		bishopMoves<false>(state.pieceOccupancyT<Piece::BlackBishop>(), moveList, state);
		rookMoves<false>(state.pieceOccupancyT<Piece::BlackRook>(), moveList, state);
		queenMoves<false>(state.pieceOccupancyT<Piece::BlackQueen>(), moveList, state);
		kingMoves<false>(state.pieceOccupancyT<Piece::BlackKing>(), moveList, state);
	}

	return moveList;
}


BitBoard MoveGen::getWhitePawnMoves(std::size_t square) const noexcept
{
	return preGen.whitePawnAttack(square);
}

BitBoard MoveGen::getBlackPawnMoves(std::size_t square) const noexcept
{
	return preGen.blackPawnAttack(square);
}

BitBoard MoveGen::getKnightMoves(std::size_t square) const noexcept
{
	return preGen.knightMove(square);
}

BitBoard MoveGen::getKingMoves(std::size_t square) const noexcept
{
	return preGen.kingMove(square);
}

BitBoard MoveGen::getBishopMoves(std::size_t square, BitBoard occupancy) const noexcept
{
	return preGen.bishopMove(square, occupancy);
}

BitBoard MoveGen::getRookMoves(std::size_t square, BitBoard occupancy) const noexcept
{
	return preGen.rookMove(square, occupancy);
}