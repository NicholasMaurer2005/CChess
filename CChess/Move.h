#pragma once

#include <cstdint>
#include <cstddef>

class Move
{
private:

	std::uint16_t m_move;

public:

	//constructors
	Move();



	//getters
	std::size_t source() const;

	std::size_t destination() const;

	bool attack() const;
};

