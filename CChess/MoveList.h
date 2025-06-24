#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "Move.h"

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
		m_moves[m_back] = Move(piece, source, destination);
		++m_back;
	}

	template<Piece piece>
	void pushAttack(int source, int destination, Piece attackPiece)
	{
		m_moves[m_back] = Move(piece, source, destination, attackPiece);
		++m_back;
	}

	void sort() noexcept;

	std::array<Move, maxLegalMoves>::const_iterator begin() const noexcept;

	std::array<Move, maxLegalMoves>::const_iterator end() const noexcept;
};

