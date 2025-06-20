#pragma once

#include <array>
#include <cstddef>

#include "BitBoard.h"



class cachealign State
{
private:

	BitBoard m_occupancy;
	BitBoard m_whiteOccupancy;
	BitBoard m_blackOccupancy;

	std::array<BitBoard, pieceCount> m_pieceOccupancy;

public:

	State() noexcept;

	BitBoard occupancy() const noexcept;

	BitBoard whiteOccupancy() const noexcept;

	BitBoard blackOccupancy() const noexcept;

	BitBoard pieceOccupancy(Piece piece) const noexcept;

	template<Piece piece>
	BitBoard pieceOccupancyT() const noexcept
	{
		return m_pieceOccupancy[static_cast<std::size_t>(piece)];
	}
};

