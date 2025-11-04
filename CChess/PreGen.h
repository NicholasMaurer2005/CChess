#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "BitBoard.h"

#include <iostream>

constexpr int maxBishopAttacks{ 512 };
constexpr int maxRookAttacks{ 4096 };

//TODO: replace magic bitcount with magic shift and store it contiguously with attack lists for better cache layout

class cachealign PreGen
{
	/* Class Members*/

private:

	struct alignas(16) MagicData
	{
		BitBoard relevantBits;
		int magicShift;
	};

	//magic numbers //TODO: use stockfish method.
	std::array<MagicData, boardSize> m_bishopMagicData;
	std::array<MagicData, boardSize> m_rookMagicData;
	std::array<std::uint64_t, boardSize> m_bishopMagics;
	std::array<std::uint64_t, boardSize> m_rookMagics;

	//attack tables
	std::array<BitBoard, boardSize> m_whitePawnAttacks;
	std::array<BitBoard, boardSize> m_blackPawnAttacks;
	std::array<BitBoard, boardSize> m_knightMoves; 
	std::array<BitBoard, boardSize> m_kingMoves;
	std::array<BitBoard, boardSize * maxBishopAttacks> m_bishopMoves;
	std::array<BitBoard, boardSize * maxRookAttacks> m_rookMoves;



	/* Private Methods */

private:

	
	//magic numbers
	void generateBishopRelevantBits() noexcept;

	void generateRookRelevantBits() noexcept;

	void generateBishopMagics() noexcept;

	void generateRookMagics() noexcept;



	//tables
	void generatePawnAttacks() noexcept;

	void generateKightMoves() noexcept;

	void generateKingMoves() noexcept;

	void generateBishopMoves() noexcept;

	void generateRookMoves() noexcept;



public:

	//constructor
	PreGen() noexcept;



	//getters
	BitBoard whitePawnAttack(std::size_t index) const noexcept;

	BitBoard blackPawnAttack(std::size_t index) const noexcept;

	BitBoard knightMove(std::size_t index) const noexcept;

	BitBoard kingMove(std::size_t index) const noexcept;

	BitBoard bishopMove(std::size_t index, BitBoard occupancy) const noexcept;

	BitBoard rookMove(std::size_t index, BitBoard occupancy) const noexcept;
};

