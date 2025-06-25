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

std::size_t Move::sourceIndex() const noexcept
{
	return static_cast<std::size_t>((m_move & sourceIndexMask) >> sourceIndexShift);
}

std::size_t Move::destinationIndex() const noexcept
{
	return static_cast<std::size_t>((m_move & destinationIndexMask) >> destinationIndexShift);
}

Piece Move::promotePiece() const noexcept
{
	return static_cast<Piece>((m_move & promotePieceMask) >> promotePieceShift);
}

std::size_t Move::enpassantIndex() const noexcept
{
	const std::size_t index{ static_cast<std::size_t>((m_move & enpassantIndexMask) >> enpassantIndexShift) };
	return index ? index - 1 : 0;
}

bool Move::castleFlag() const noexcept
{
	return static_cast<bool>((m_move & castleFlagMask) >> castleFlagShift);
}

Piece Move::castleKingPiece() const noexcept
{
	return static_cast<Piece>(m_move & castleKingPieceMask);
}

Piece Move::castleRookPiece() const noexcept
{
	return static_cast<Piece>((m_move & castleRookPieceMask) >> castleRookPieceShift);
}

std::size_t Move::castleKingSource() const noexcept
{
	return static_cast<std::size_t>((m_move & castleKingSourceMask) >> castleKingSourceShift);
}

std::size_t Move::castleKingDestination() const noexcept
{
	return static_cast<std::size_t>((m_move & castleKingDestinationMask) >> castleKingDestinationShift);
}

std::size_t Move::castleRookSource() const noexcept
{
	return static_cast<std::size_t>((m_move & castleRookSourceMask) >> castleRookSourceShift);
}

std::size_t Move::castleRookDestination() const noexcept
{
	return static_cast<std::size_t>((m_move & castleRookDestinationMask) >> castleRookDestinationShift);
}