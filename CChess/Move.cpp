#include "Move.h"

#include <string_view>
#include <array>
#include <iostream>


//print
void Move::print() const noexcept
{
	constexpr std::array<std::string_view, boardSize> squareToRF{
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
	};

	std::cout << squareToRF[sourceIndex()] << squareToRF[destinationIndex()] << '\n';
}



//getters
Piece Move::sourcePiece() const noexcept
{
	return static_cast<Piece>(m_move & sourcePieceMask);
}

Piece Move::attackPiece() const noexcept
{
	return static_cast<Piece>((m_move & attackPieceMask) >> attackPieceShift);
}

int Move::sourceIndex() const noexcept
{
	return static_cast<int>((m_move & sourceIndexMask) >> sourceIndexShift);
}

int Move::destinationIndex() const noexcept
{
	return static_cast<int>((m_move & destinationIndexMask) >> destinationIndexShift);
}

Piece Move::promotePiece() const noexcept
{
	return static_cast<Piece>((m_move & promotePieceMask) >> promotePieceShift);
}

bool Move::doublePawnFlag() const noexcept
{
	return static_cast<bool>(m_move & doublePawnFlagMask);
}

bool Move::enpassantFlag() const noexcept
{
	return static_cast<bool>(m_move & enpassantFlagMask);
}

bool Move::castleFlag() const noexcept
{
	return static_cast<bool>(m_move & castleFlagMask);
}

int Move::enpassantIndex() const noexcept
{
	return static_cast<int>((m_move & enpassantIndexMask) >> enpassantIndexShift);
}

Castle Move::castleType() const noexcept
{
	return static_cast<Castle>(m_move & castleTypeMask);
}
