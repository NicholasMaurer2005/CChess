#pragma once

#include <cstdint>
#include <cstddef>

#include "ChessConstants.hpp"

class Move
{
private:

	std::uint32_t m_move;

public:

	//constructors
	Move() noexcept;



	//getters
	Piece piece() const noexcept;

	std::size_t source() const noexcept;

	std::size_t destination() const noexcept;

	bool attack() const noexcept;
};

