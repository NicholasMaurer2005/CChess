#include "Move.h"



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
