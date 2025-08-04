#include "Evaluate.h"



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
	return static_cast<int>(state.whiteSquares().bitCount() - state.blackSquares().bitCount());
}

int evaluate(const State& state) noexcept
{
	return pieceEvaluation(state) + spaceEvaluation(state);
}

