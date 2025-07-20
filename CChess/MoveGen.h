#pragma once

#include "State.h"
#include "MoveList.h"

class MoveGen //TODO: make this a namespace and have all functions static?
{

public:

	MoveGen() noexcept;

	MoveList generateMoves(bool white, const State& state) const noexcept;

	CaptureList generateCaptures(bool white, const State& state) const noexcept;

	BitBoard getWhitePawnMoves(std::size_t square) const noexcept;

	BitBoard getBlackPawnMoves(std::size_t square) const noexcept;

	BitBoard getKnightMoves(std::size_t square) const noexcept;

	BitBoard getKingMoves(std::size_t square) const noexcept;

	BitBoard getBishopMoves(std::size_t square, BitBoard occupancy) const noexcept;

	BitBoard getRookMoves(std::size_t square, BitBoard occupancy) const noexcept;
};

