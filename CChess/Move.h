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
	Move() noexcept {}

	//quiet
	Move(Piece piece, int source, int destination) noexcept;

	//attack
	Move(Piece piece, int source, int destination, Piece attackPiece) noexcept;

	//quiet promote
	Move(bool white, int source, int destination, Piece promotePiece) noexcept;

	//attack promote
	Move(bool white, int source, int destination, Piece promotePiece, Piece attackPiece) noexcept;

	//enpassant
	Move(bool white, int source, int destination, int enpassant) noexcept;

	//castle
	Move(Castle castle);


	//getters
	Piece piece() const noexcept;

	std::size_t source() const noexcept;

	std::size_t destination() const noexcept;

	Piece attackPiece() const noexcept;

	Piece promotePiece() const noexcept;
};

