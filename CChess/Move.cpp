#include "Move.h"


constexpr std::uint32_t pieceMask		{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t sourceMask		{ 0b00000000000000000000001111110000 };
constexpr std::uint32_t destinationMask	{ 0b00000000000000001111110000000000 };
constexpr std::uint32_t attackMask		{ 0b00000000000000010000000000000000 };

constexpr int sourceShift{ 6 };
constexpr int destinationShift{ 10 };
constexpr int attackShift{ 16 };

//constructors
Move::Move() noexcept
	: m_move() { }



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

bool Move::attack() const noexcept
{
	return static_cast<bool>((m_move & attackMask) >> attackShift);
}