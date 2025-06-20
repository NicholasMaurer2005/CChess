#include "Move.h"


constexpr std::uint16_t sourceMask		{ 0b0000000000111111 };
constexpr std::uint16_t destinationMask	{ 0b0000111111000000 };
constexpr std::uint16_t attackMask		{ 0b0001000000000000 };

constexpr int destinationShift{ 6 };
constexpr int attackShift{ 12 };

//constructors
Move::Move()
	: m_move() { }



//getters
std::size_t Move::source() const
{
	return static_cast<std::size_t>(m_move & sourceMask);
}

std::size_t Move::destination() const
{
	return static_cast<std::size_t>((m_move & destinationMask) >> destinationShift);
}

bool Move::attack() const
{
	return static_cast<bool>((m_move & attackMask) >> attackShift);
}