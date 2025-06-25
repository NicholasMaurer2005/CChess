#include "Move.h"


//if no flags are set than it is a 

constexpr std::uint32_t sourcePieceMask				{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t attackPieceMask				{ 0b00000000000000000000000011110000 };
constexpr std::uint32_t sourceIndexMask				{ 0b00000000000000000011111100000000 };
constexpr std::uint32_t destinationIndexMask		{ 0b00000000000011111100000000000000 };
constexpr std::uint32_t promotePieceMask			{ 0b00000000111100000000000000000000 };
constexpr std::uint32_t enpassantIndexMask			{ 0b01111111000000000000000000000000 };
constexpr std::uint32_t castleFlagMask				{ 0b10000000000000000000000000000000 };
constexpr std::uint32_t castleKingPieceMask			{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t castleRookPieceMask			{ 0b00000000000000000000000011110000 };
constexpr std::uint32_t castleKingSourceMask		{ 0b00000000000000000011111100000000 };
constexpr std::uint32_t castleKingDestinationMask	{ 0b00000000000011111100000000000000 };
constexpr std::uint32_t castleRookSourceMask		{ 0b00000011111100000000000000000000 };
constexpr std::uint32_t castleRookDestinationMask	{ 0b11111100000000000000000000000000 };

constexpr int attackPieceShift{ 4 };
constexpr int sourceIndexShift{ 8 };
constexpr int destinationIndexShift{ 14 };
constexpr int promotePieceShift{ 20 };
constexpr int enpassantIndexShift{ 24 };
constexpr int castleFlagShift{ 31 };
constexpr int castleRookPieceShift{ 4 };
constexpr int castleKingSourceShift{ 8 };
constexpr int castleKingDestinationShift{ 14 };
constexpr int castleRookSourceShift{ 20 };
constexpr int castleRookDestinationShift{ 26 };



//quiet
Move::Move(Piece sourcePiece, int sourceIndex, int destinationIndex) noexcept
	: m_move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift) { }

//attack
Move::Move(Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex) noexcept
	: m_move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift) { }

//quiet promote
Move::Move(Piece sourcePiece, int sourceIndex, int destinationIndex, Piece promotePiece) noexcept
	: m_move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift
		| static_cast<std::uint32_t>(promotePiece) << promotePieceShift) { }

//attack promote
Move::Move(Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex, Piece promotePiece) noexcept
	: m_move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift
		| static_cast<std::uint32_t>(promotePiece) << promotePieceShift) { }

//enpassant
Move::Move(Piece sourcePiece, int sourceIndex, int destinationIndex, Piece attackPiece) noexcept
	: m_move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift) { }

//castle
Move::Move(std::uint32_t castleMove)
	: m_move(castleMove) { }



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