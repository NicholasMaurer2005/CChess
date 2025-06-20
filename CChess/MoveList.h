#pragma once

#include <array>

#include "ChessConstants.hpp"

constexpr std::size_t maxLegalMoves{ 218 };

class cachealign MoveList
{
private:

	std::array<Move, maxLegalMoves> m_moves;
	std::size_t m_back;

public:

	MoveList() noexcept;

	template<Piece piece>
	void pushQuiet(int source, int destination) noexcept
	{
		
	}

	void sort() noexcept;

	std::array<Move, maxLegalMoves>::const_iterator begin() const noexcept;

	std::array<Move, maxLegalMoves>::const_iterator end() const noexcept;
};

