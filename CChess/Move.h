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
	Move(Piece sourcePiece, int sourceIndex, int destinationIndex) noexcept;

	//attack
	Move(Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex) noexcept;

	//quiet promote
	Move(Piece sourcePiece, int sourceIndex, int destinationIndex, Piece promotePiece) noexcept;

	//attack promote
	Move(Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex, Piece promotePiece) noexcept;

	//enpassant
	Move(Piece sourcePiece, int sourceIndex, int destinationIndex, Piece attackPiece) noexcept;

	//castle
	Move(std::uint32_t castleMove);



	//getters
	Piece sourcePiece() const noexcept;

	Piece attackPiece() const noexcept;

	std::size_t sourceIndex() const noexcept;

	std::size_t destinationIndex() const noexcept;

	Piece promotePiece() const noexcept;

	std::size_t enpassantIndex() const noexcept;

	bool castleFlag() const noexcept;

	Piece castleKingPiece() const noexcept;

	Piece castleRookPiece() const noexcept;

	std::size_t castleKingSource() const noexcept;

	std::size_t castleKingDestination() const noexcept;

	std::size_t castleRookSource() const noexcept;

	std::size_t castleRookDestination() const noexcept;
};

