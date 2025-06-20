#pragma once

#include <cstdint>
#include <cstddef>

class Move
{
private:

	std::uint16_t m_move;

public:

	//constructors
	Move() noexcept;



	//getters
	std::size_t source() const noexcept;

	std::size_t destination() const noexcept;

	bool attack() const noexcept;
};

