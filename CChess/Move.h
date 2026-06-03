#pragma once

#include <cstdint>
#include <string>

#include "Castle.hpp"
#include "ChessConstants.hpp"



class Move
{
private:

	//	Private Definitions

	static constexpr std::uint32_t sourcePieceMask{ 0b00000000000000000000000000001111 };
	static constexpr std::uint32_t attackPieceMask{ 0b00000000000000000000000011110000 };
	static constexpr std::uint32_t sourceIndexMask{ 0b00000000000000000011111100000000 };
	static constexpr std::uint32_t destinationIndexMask{ 0b00000000000011111100000000000000 };
	static constexpr std::uint32_t promotePieceMask{ 0b00000000111100000000000000000000 };
	static constexpr std::uint32_t doublePawnFlagMask{ 0b00000001000000000000000000000000 };
	static constexpr std::uint32_t enpassantFlagMask{ 0b00000010000000000000000000000000 };
	static constexpr std::uint32_t castleFlagMask{ 0b00000100000000000000000000000000 };
	static constexpr std::uint32_t enpassantIndexMask{ 0b00111000000000000000000000000000 };
	static constexpr std::uint32_t castleTypeMask{ 0b00000000000000000000000000001111 };

	static constexpr int attackPieceShift{ 4 };
	static constexpr int sourceIndexShift{ 8 };
	static constexpr int destinationIndexShift{ 14 };
	static constexpr int promotePieceShift{ 20 };
	static constexpr int doublePawnFlagShift{ 24 };
	static constexpr int enpassantFlagShift{ 25 };
	static constexpr int castleFlagShift{ 26 };
	static constexpr int enpassantIndexShift{ 27 };



private:

	//	Private Members

	std::uint32_t m_move;



public:

	//	Static Methods
	// 
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



public:

	//	Public Methods

	//constructors
	constexpr Move() noexcept {} // m_move is supposed to be uninitialized if the default constructor is called
	
	constexpr Move(std::uint32_t move) //TODO: move to source file
		: m_move(move) { }


		
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



	//helpers
	void print() const;

	std::string string() const;
};