#pragma once

#include "State.h"
#include "MoveList.hpp"

namespace MoveGen
{
	MoveList generateMoves(bool white, const State& state) noexcept;

	CaptureList generateCaptures(bool white, const State& state) noexcept;

	BitBoard whitePawnMoves(std::size_t square) noexcept;

	BitBoard blackPawnMoves(std::size_t square) noexcept;

	BitBoard knightMoves(std::size_t square) noexcept;

	BitBoard kingMoves(std::size_t square) noexcept;

	BitBoard bishopMoves(std::size_t square, BitBoard occupancy) noexcept;

	BitBoard rookMoves(std::size_t square, BitBoard occupancy) noexcept;
};

