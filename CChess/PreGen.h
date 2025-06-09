#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "BitBoard.h"

constexpr int maxBishopAttacks{ 512 };
constexpr int maxRookAttacks{ 4096 };

class PreGen
{
private:

	//attack tables
	alignas(64) std::array<BitBoard, boardSize> m_whitePawnAttacks;
	alignas(64) std::array<BitBoard, boardSize> m_blackPawnAttacks;
	alignas(64) std::array<BitBoard, boardSize> m_knightAttacks;
	alignas(64) std::array<BitBoard, boardSize> m_kingAttacks;
	alignas(64) std::array<BitBoard, boardSize * maxBishopAttacks> m_bishopAttacks;
	alignas(64) std::array<BitBoard, boardSize * maxRookAttacks> m_rookAttacks;

	//magic numbers
	alignas(64) std::array<BitBoard, boardSize>  m_bishopRelevantBits;
	alignas(64) std::array<BitBoard, boardSize>  m_rookRelevantBits;
	alignas(64) std::array<BitBoard, boardSize> m_bishopMagics;
	alignas(64) std::array<BitBoard, boardSize> m_rookMagics;

public:

	void print()
	{
		for (BitBoard board : m_kingAttacks)
		{
			board.print();
		}
	}

	//constructor
	PreGen();



	//private
	void generatePawnAttacks();

	void generateKightAttacks();

	void generateKingAttacks();

	void generateBishopRelevantBits();

	void generateRookRelevantBits();



	//getters
	const std::array<BitBoard, boardSize>& whitePawnAttacks() const;

	const std::array<BitBoard, boardSize>& blackPawnAttacks() const;

	const std::array<BitBoard, boardSize>& knightAttacks() const;

	const std::array<BitBoard, maxBishopAttacks* boardSize>& bishopAttacks() const;

	const std::array<BitBoard, maxRookAttacks* boardSize>& rookAttacks() const;
};

