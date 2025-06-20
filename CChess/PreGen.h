#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "BitBoard.h"

#include <iostream>

constexpr int maxBishopAttacks{ 512 };
constexpr int maxRookAttacks{ 4096 };

class PreGen
{
private:

	//magic numbers
	alignas(64) std::array<BitBoard, boardSize>  m_bishopRelevantBits;
	alignas(64) std::array<BitBoard, boardSize>  m_rookRelevantBits;
	alignas(64) std::array<std::uint64_t, boardSize> m_bishopMagics;
	alignas(64) std::array<std::uint64_t, boardSize> m_rookMagics;
	alignas(64) std::array<int, boardSize> m_bishopBitCount;
	alignas(64) std::array<int, boardSize> m_rookBitCount;

	//attack tables
	alignas(64) std::array<BitBoard, boardSize> m_whitePawnAttacks;
	alignas(64) std::array<BitBoard, boardSize> m_blackPawnAttacks;
	alignas(64) std::array<BitBoard, boardSize> m_knightAttacks;
	alignas(64) std::array<BitBoard, boardSize> m_kingAttacks;
	alignas(64) std::array<BitBoard, boardSize * maxBishopAttacks> m_bishopAttacks;
	alignas(64) std::array<BitBoard, boardSize * maxRookAttacks> m_rookAttacks;


	

	//magic numbers
	void generateBishopRelevantBits();

	void generateRookRelevantBits();

	void generateBishopMagics();

	void generateRookMagics();



	//tables
	void generatePawnAttacks();

	void generateKightAttacks();

	void generateKingAttacks();

	void generateBishopAttacks();

	void generateRookAttacks();



	//getters
	BitBoard whitePawnAttack(std::size_t index) const;

	BitBoard blackPawnAttack(std::size_t index) const;

	BitBoard knightAttack(std::size_t index) const;

	BitBoard bishopAttack(std::size_t index, BitBoard occupancy) const;

	BitBoard rookAttack(std::size_t index, BitBoard occupancy) const;

public:

	//constructor
	PreGen();
};

