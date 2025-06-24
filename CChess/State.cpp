#include "State.h"



//constructors
State::State() noexcept
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy() { }



//getters
BitBoard State::occupancy() const noexcept
{
	return m_occupancy;
}

BitBoard State::whiteOccupancy() const noexcept
{
	return m_whiteOccupancy;
}

BitBoard State::blackOccupancy() const noexcept
{
	return m_blackOccupancy;
}

BitBoard State::pieceOccupancy(Piece piece) const noexcept
{
	return m_pieceOccupancy[static_cast<std::size_t>(piece)];
}

BitBoard State::whiteEnpassantSquare() const noexcept
{
	return m_whiteEnpassantSquare;
}

BitBoard State::blackEnpassantSquare() const noexcept
{
	return m_blackEnpassantSquare;
}