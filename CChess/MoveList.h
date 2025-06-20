#pragma once

#include <array>

#include "Move.h"

constexpr std::size_t maxLegalMoves{ 218 };

class alignas(64) MoveList
{
private:

	std::array<Move, maxLegalMoves> m_moves;
	std::size_t m_back;

public:

	MoveList();

	void push(Move move);

	void sort();

	std::array<Move, maxLegalMoves>::const_iterator begin() const;

	std::array<Move, maxLegalMoves>::const_iterator end() const;
};

