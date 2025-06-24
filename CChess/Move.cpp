#include "Move.h"


constexpr std::uint32_t pieceMask			{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t sourceMask			{ 0b00000000000000000000001111110000 };
constexpr std::uint32_t destinationMask		{ 0b00000000000000001111110000000000 };
constexpr std::uint32_t attackPieceMask		{ 0b00000000000011110000000000000000 };
constexpr std::uint32_t promotePieceMask	{ 0b00000000111100000000000000000000 };
constexpr std::uint32_t enpassantFlagMask	{ 0b00000001000000000000000000000000 };
constexpr std::uint32_t enpassantMask		{ 0b01111110000000000000000000000000 };

constexpr int sourceShift{ 4 };
constexpr int destinationShift{ 10 };
constexpr int attackPieceShift{ 16 };
constexpr int promotePieceShift{ 20 };
constexpr int enpassantFlagShift{ 24 };
constexpr int enpassantShift{ 25 };

//constructors

//quiet
Move::Move(Piece piece, int source, int destination) noexcept
	: m_move(static_cast<std::uint32_t>(piece)
		| static_cast<std::uint32_t>(source) << sourceShift
		| static_cast<std::uint32_t>(destination) << destinationShift) { }

//attack
Move::Move(Piece piece, int source, int destination, Piece attackPiece) noexcept
	: m_move(static_cast<std::uint32_t>(piece)
		| static_cast<std::uint32_t>(source) << sourceShift
		| static_cast<std::uint32_t>(destination) << destinationShift
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift) { }

//quiet promote
Move::Move(bool white, int source, int destination, Piece promotePiece) noexcept
	: m_move(static_cast<std::uint32_t>(white ? Piece::WhitePawn : Piece::BlackPawn)
		| static_cast<std::uint32_t>(source) << sourceShift
		| static_cast<std::uint32_t>(destination) << destinationShift
		| static_cast<std::uint32_t>(promotePiece) << promotePieceShift) { }

//attack promote //TODO: rearange so that there doesnt need to be a condition
Move::Move(bool white, int source, int destination, Piece promotePiece, Piece attackPiece) noexcept
	: m_move(static_cast<std::uint32_t>(white ? Piece::WhitePawn : Piece::BlackPawn)
		| static_cast<std::uint32_t>(source) << sourceShift
		| static_cast<std::uint32_t>(destination) << destinationShift
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(promotePiece) << promotePieceShift) { }

//enpassant
Move::Move(bool white, int source, int destination, int enpassant) noexcept //TODO: maybe make enpassant one shift/mask and make no square an option
	: m_move(static_cast<std::uint32_t>(white ? Piece::WhitePawn : Piece::BlackPawn) 
		| static_cast<std::uint32_t>(source) << sourceShift
		| static_cast<std::uint32_t>(destination) << destinationShift
		| static_cast<std::uint32_t>(white ? Piece::BlackPawn : Piece::WhitePawn) << attackPieceShift
		| static_cast<std::uint32_t>(1 << enpassantFlagShift)
		| static_cast<std::uint32_t>(enpassant << enpassantShift)) { }

//castle
Move::Move(Castle castle)
	: m_move()



//getters
Piece Move::piece() const noexcept
{
	return static_cast<Piece>(m_move & sourceMask);
}

std::size_t Move::source() const noexcept
{
	return static_cast<std::size_t>((m_move & sourceMask) >> sourceShift);
}

std::size_t Move::destination() const noexcept
{
	return static_cast<std::size_t>((m_move & destinationMask) >> destinationShift);
}

Piece Move::attackPiece() const noexcept
{
	return static_cast<Piece>((m_move & attackPieceMask) >> attackPieceShift);
}

Piece Move::promotePiece() const noexcept
{
	return static_cast<Piece>((m_move & promotePieceMask) >> promotePieceShift);
}