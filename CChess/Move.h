#pragma once

#include <cstdint>
#include <cstddef>

#include "ChessConstants.hpp"



constexpr std::uint32_t sourcePieceMask{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t attackPieceMask{ 0b00000000000000000000000011110000 };
constexpr std::uint32_t sourceIndexMask{ 0b00000000000000000011111100000000 };
constexpr std::uint32_t destinationIndexMask{ 0b00000000000011111100000000000000 };
constexpr std::uint32_t promotePieceMask{ 0b00000000111100000000000000000000 };
constexpr std::uint32_t enpassantIndexMask{ 0b01111111000000000000000000000000 };
constexpr std::uint32_t castleFlagMask{ 0b10000000000000000000000000000000 };
constexpr std::uint32_t castleKingPieceMask{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t castleRookPieceMask{ 0b00000000000000000000000011110000 };
constexpr std::uint32_t castleKingSourceMask{ 0b00000000000000000011111100000000 };
constexpr std::uint32_t castleKingDestinationMask{ 0b00000000000011111100000000000000 };
constexpr std::uint32_t castleRookSourceMask{ 0b00000011111100000000000000000000 };
constexpr std::uint32_t castleRookDestinationMask{ 0b11111100000000000000000000000000 };

constexpr int attackPieceShift{ 4 };
constexpr int sourceIndexShift{ 8 };
constexpr int destinationIndexShift{ 14 };
constexpr int promotePieceShift{ 20 };
constexpr int enpassantIndexShift{ 24 };
constexpr int castleFlagShift{ 31 };
constexpr int castleRookPieceShift{ 4 };
constexpr int castleKingSourceShift{ 8 };
constexpr int castleKingDestinationShift{ 14 };
constexpr int castleRookSourceShift{ 20 };
constexpr int castleRookDestinationShift{ 26 };




//quiet
template<Piece sourcePiece>
static Move makeQuiet(int sourceIndex, int destinationIndex) noexcept
{
	return Move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift);
}

//attack
template<Piece sourcePiece>
static Move makeAttack(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept
{
	return Move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift);
}

//quiet promote
template<Piece sourcePiece, Piece promotePiece>
static Move makeQuietPromote(int sourceIndex, int destinationIndex) noexcept
{
	return Move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift
		| static_cast<std::uint32_t>(promotePiece) << promotePieceShift);
}

//attack promote
template<Piece sourcePiece, Piece promotePiece>
static Move makeAttackPromote(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept
{
	return Move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift
		| static_cast<std::uint32_t>(promotePiece) << promotePieceShift);
}

//enpassant
template<Piece sourcePiece, Piece attackPiece>
static Move makeEnpassant(int sourceIndex, int destinationIndex, int enpassantIndex) noexcept
{
	return Move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(attackPiece) << attackPieceShift
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift
		| static_cast<std::uint32_t>(enpassantIndex) << enpassantIndexShift);
}



class Move
{
private:

	std::uint32_t m_move;

public:

	//constructors
	Move() noexcept {}

	constexpr Move(std::uint32_t move)
		: m_move(move) { }



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

