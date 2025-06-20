#pragma once

#include <array>
#include <cstddef>

#include "BitBoard.h"

enum class Piece : std::size_t
{
	WhitePawn = 0,
	WhiteKnight = 1,
	WhiteBishop = 2,
	WhiteRook = 3,
	WhiteQueen = 4,
	WhiteKing = 5,
	BlackPawn = 6,
	BlackKnight = 7,
	BlackBishop = 8,
	BlackRook = 9,
	BlackQueen = 10,
	BlackKing = 11,
};

constexpr int pieceOccupancySize{ 12 };

class alignas(64) State
{
private:

	BitBoard m_occupancy;
	BitBoard m_whiteOccupancy;
	BitBoard m_blackOccupancy;

	std::array<BitBoard, pieceOccupancySize> m_pieceOccupancy;

public:

	State();

	BitBoard occupancy() const;

	BitBoard whiteOccupancy() const;

	BitBoard blackOccupancy() const;

	BitBoard pieceOccupancy(Piece piece) const;

	template<Piece piece>
	BitBoard pieceOccupancyT() const
	{
		return m_pieceOccupancy[static_cast<std::size_t>(piece)];
	}
};

