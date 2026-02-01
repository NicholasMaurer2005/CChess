#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "Move.h"

constexpr int plyMovesCount{ 2 };

struct alignas(8) KillerMoves
{
	Move first;
	Move second;
};

class cachealign KillerMoveHistory
{
private:

	struct KillerMoveData
	{
		std::size_t position;
		std::array<Move, plyMovesCount> moves;
	};

	std::array<KillerMoveData, maxSearchDepth> m_moves;

public:

	KillerMoveHistory() noexcept;

	KillerMoves killerMoves(int depth) const noexcept;

	void push(int depth, Move move) noexcept;
};

