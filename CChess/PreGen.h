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
private:

	//magic numbers
	std::array<BitBoard, boardSize>  m_bishopRelevantBits; //TODO: use stockfish method.
	std::array<BitBoard, boardSize>  m_rookRelevantBits;
	std::array<std::uint64_t, boardSize> m_bishopMagics;
	std::array<std::uint64_t, boardSize> m_rookMagics;
	std::array<int, boardSize> m_bishopBitCount; //TODO: replace with shifts
	std::array<int, boardSize> m_rookBitCount; 

	//attack tables
	std::array<BitBoard, boardSize> m_whitePawnAttacks;
	std::array<BitBoard, boardSize> m_blackPawnAttacks;
	std::array<BitBoard, boardSize> m_knightAttacks; //TODO: rename from Attacks to Moves?
	std::array<BitBoard, boardSize> m_kingAttacks;
	std::array<BitBoard, boardSize * maxBishopAttacks> m_bishopAttacks;
	std::array<BitBoard, boardSize * maxRookAttacks> m_rookAttacks;


	
	//magic numbers
	void generateBishopRelevantBits() noexcept;

	void generateRookRelevantBits() noexcept;

	void generateBishopMagics() noexcept;

	void generateRookMagics() noexcept;



	//tables
	void generatePawnAttacks() noexcept;

	void generateKightAttacks() noexcept;

	void generateKingAttacks() noexcept;

	void generateBishopAttacks() noexcept;

	void generateRookAttacks() noexcept;



public:

	//constructor
	PreGen() noexcept;



	//getters
	BitBoard whitePawnAttack(std::size_t index) const noexcept;

	BitBoard blackPawnAttack(std::size_t index) const noexcept;

	BitBoard knightAttack(std::size_t index) const noexcept;

	BitBoard kingAttack(std::size_t index) const noexcept;

	BitBoard bishopAttack(std::size_t index, BitBoard occupancy) const noexcept;

	BitBoard rookAttack(std::size_t index, BitBoard occupancy) const noexcept;
};

