#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>

#include "ChessConstants.hpp"
#include "Castle.hpp"



constexpr std::uint32_t sourcePieceMask		{ 0b00000000000000000000000000001111 };
constexpr std::uint32_t attackPieceMask		{ 0b00000000000000000000000011110000 };
constexpr std::uint32_t sourceIndexMask		{ 0b00000000000000000011111100000000 };
constexpr std::uint32_t destinationIndexMask{ 0b00000000000011111100000000000000 };
constexpr std::uint32_t promotePieceMask	{ 0b00000000111100000000000000000000 };
constexpr std::uint32_t doublePawnFlagMask	{ 0b00000001000000000000000000000000 };
constexpr std::uint32_t enpassantFlagMask	{ 0b00000010000000000000000000000000 };
constexpr std::uint32_t castleFlagMask		{ 0b00000100000000000000000000000000 };
constexpr std::uint32_t enpassantIndexMask	{ 0b00111000000000000000000000000000 };
constexpr std::uint32_t castleTypeMask		{ 0b00000000000000000000000000001111 };

constexpr int attackPieceShift{ 4 };
constexpr int sourceIndexShift{ 8 };
constexpr int destinationIndexShift{ 14 };
constexpr int promotePieceShift{ 20 };
constexpr int doublePawnFlagShift{ 24 };
constexpr int enpassantFlagShift{ 25 };
constexpr int castleFlagShift{ 26 };
constexpr int enpassantIndexShift{ 27 };



class Move
{
private:

	std::uint32_t m_move;

public:

	//constructors
	constexpr Move() noexcept {}
	
	constexpr Move(std::uint32_t move) //TODO: move to source file
		: m_move(move) { }



	//print
	void print() const;

	std::string string() const;

		
	//getters
	std::uint32_t move() const noexcept;

	Piece sourcePiece() const noexcept;

	Piece attackPiece() const noexcept;

	int sourceIndex() const noexcept;

	int destinationIndex() const noexcept;

	Piece promotePiece() const noexcept;

	bool doublePawnFlag() const noexcept;

	bool enpassantFlag() const noexcept;

	bool castleFlag() const noexcept;

	int enpassantIndex() const noexcept;

	Castle castleType() const noexcept;
};



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
		| static_cast<std::uint32_t>(enpassantIndex & 0b0111) << enpassantIndexShift
		| static_cast<std::uint32_t>(1U) << enpassantFlagShift);
}

//double pawn push
template<Piece sourcePiece>
static Move makeDoublePawn(int sourceIndex, int destinationIndex) noexcept
{
	return Move(static_cast<std::uint32_t>(sourcePiece)
		| static_cast<std::uint32_t>(sourceIndex) << sourceIndexShift
		| static_cast<std::uint32_t>(destinationIndex) << destinationIndexShift
		| static_cast<std::uint32_t>(1U << doublePawnFlagShift));
}