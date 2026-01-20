#include "Evaluate.h"
#include "MoveGen.h"

#include <iostream>

static MoveGen moveGen;

static int pieceEvaluation(const State& state) noexcept
{
	constexpr int pawnValue{ 100 };
	constexpr int knightValue{ 300 };
	constexpr int bishopValue{ 300 };
	constexpr int rookValue{ 500 };
	constexpr int queenValue{ 900 };

	const int evaluation{
		static_cast<int>(state.pieceOccupancyT<Piece::WhitePawn>().bitCount()) * pawnValue
		+ static_cast<int>(state.pieceOccupancyT<Piece::WhiteKnight>().bitCount()) * knightValue
		+ static_cast<int>(state.pieceOccupancyT<Piece::WhiteBishop>().bitCount()) * bishopValue
		+ static_cast<int>(state.pieceOccupancyT<Piece::WhiteRook>().bitCount()) * rookValue
		+ static_cast<int>(state.pieceOccupancyT<Piece::WhiteQueen>().bitCount()) * queenValue
		- static_cast<int>(state.pieceOccupancyT<Piece::BlackPawn>().bitCount()) * pawnValue
		- static_cast<int>(state.pieceOccupancyT<Piece::BlackKnight>().bitCount()) * knightValue
		- static_cast<int>(state.pieceOccupancyT<Piece::BlackBishop>().bitCount()) * bishopValue
		- static_cast<int>(state.pieceOccupancyT<Piece::BlackRook>().bitCount()) * rookValue
		- static_cast<int>(state.pieceOccupancyT<Piece::BlackQueen>().bitCount()) * queenValue
	};

	return evaluation;
}

static int spaceEvaluation(const State& state)
{
	const std::size_t whiteQueen{ static_cast<std::size_t>(state.pieceOccupancyT<Piece::WhiteQueen>().leastSignificantBit()) };
	const std::size_t blackQueen{ static_cast<std::size_t>(state.pieceOccupancyT<Piece::BlackQueen>().leastSignificantBit()) };
	std::cout << whitePieceOffset << ' ' << blackQueen << std::endl;

	const BitBoard whiteSquares{ moveGen.getRookMoves(whiteQueen, state.occupancy()).board() | moveGen.getBishopMoves(whiteQueen, state.occupancy()).board() };
	const BitBoard blackSquares{ moveGen.getRookMoves(blackQueen, state.occupancy()).board() | moveGen.getBishopMoves(blackQueen, state.occupancy()).board() };

	return static_cast<int>((state.whiteSquares().bitCount() - whiteSquares.bitCount()) - (state.blackSquares().bitCount() - blackSquares.bitCount()));
}

int evaluate(const State& state) noexcept
{
	return pieceEvaluation(state) + spaceEvaluation(state);
}

