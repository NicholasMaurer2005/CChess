#include "State.h"


State::State()
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy() { }

BitBoard State::occupancy() const
{
	return m_occupancy;
}

BitBoard State::whiteOccupancy() const
{
	return m_whiteOccupancy;
}

BitBoard State::blackOccupancy() const
{
	return m_blackOccupancy;
}

BitBoard State::pieceOccupancy(Piece piece) const
{
	return m_pieceOccupancy[static_cast<std::size_t>(piece)];
}